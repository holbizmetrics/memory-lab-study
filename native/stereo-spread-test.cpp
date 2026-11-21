#include "part-synth.hpp"
#include <cmath>
#include <iostream>
#include <memory>
#include <vector>

using namespace memorylab;

int main(){
 auto synth=std::make_unique<PartSynth>();
 struct Audio{std::vector<float> left,right;};
 auto render=[&](double sr,double spread,bool stereo,int block,bool layers,double drive){
  if(!synth->prepare(sr))throw 1;
  if(layers&&!synth->configure({{{4,0,127,-1},{4,0,127,-1},{},{}}}))throw 2;
  for(int part=0;part<(layers?2:1);++part){
   synth->parameter(part,Param::StereoSpread,part==0?spread:0);
   synth->parameter(part,Param::Drive,drive);
   synth->parameter(part,Param::Volume,.045);
   synth->parameter(part,Param::AmpAttack,1);
  }
  synth->dispatch({0,EventKind::NoteOn,48,1});
  synth->dispatch({0,EventKind::NoteOn,55,1});
  const int frames=int(sr*.12);
  Audio out{std::vector<float>(frames),std::vector<float>(frames)};
  for(int pos=0;pos<frames;pos+=block)
   if(!synth->render(out.left.data()+pos,std::min(block,frames-pos),nullptr,0,
                     stereo?out.right.data()+pos:nullptr))throw 3;
  return out;
 };
 for(double sr:{44100.,48000.,96000.,192000.})for(bool layers:{false,true}){
  const auto mono=render(sr,0,false,127,layers,0);
  const auto centered=render(sr,0,true,127,layers,0);
  const auto wide=render(sr,1,true,127,layers,0);
  const auto differentBlocks=render(sr,1,true,257,layers,0);
  const auto stillMono=render(sr,1,false,127,layers,0);
  if(centered.left!=mono.left||centered.right!=mono.left||stillMono.left!=mono.left||
     wide.left==wide.right||wide.left!=differentBlocks.left||wide.right!=differentBlocks.right){
   std::cerr<<"Stereo, mono fallback, or block parity failed\n";return 1;
  }
  const auto driven=render(sr,1,true,257,layers,6);
  if(driven.left==driven.right||synth->activeVoices()!=(layers?4u:2u))return 2;
  for(auto* channel:{&driven.left,&driven.right})
   for(float sample:*channel)if(!std::isfinite(sample)||std::abs(sample)>=1)return 3;
  synth->dispatch({0,EventKind::AllSoundOff});
  std::array<float,64> left{},right{};
  synth->render(left.data(),left.size(),nullptr,0,right.data());
  for(std::size_t i=0;i<left.size();++i)if(left[i]!=0||right[i]!=0)return 4;
 }
 synth->prepare(48000);
 synth->parameter(0,Param::Drive,0);
 synth->parameter(0,Param::Volume,.04);
 synth->dispatch({0,EventKind::NoteOn,48,1});
 std::array<float,24000> left{},right{};
 auto block=[&]{return synth->render(left.data(),left.size(),nullptr,0,right.data());};
 if(!block()||left!=right)return 5;
 synth->parameter(0,Param::StereoSpread,1);
 if(!block()||left==right)return 6;
 double eLeft=0,eRight=0;
 for(std::size_t i=12000;i<left.size();++i){eLeft+=left[i]*left[i];eRight+=right[i]*right[i];}
 if(eLeft<4*eRight)return 7;
 synth->parameter(0,Param::StereoSpread,0);
 if(!block()||!block()||left!=right||synth->activeVoices()!=1)return 8;
 std::cout<<"PASS stereo separation, centered/mono parity, held spread and return, layers and panic\n";
}
