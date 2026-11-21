#include "voice.hpp"
#include <iostream>
#include <memory>
#include <stdexcept>
#include <vector>

using namespace memorylab;

int main(){
 auto synth=std::make_unique<Synth>();
 auto set=[&](Param p,double value){synth->dispatch({0,EventKind::Parameter,0,value,0,p});};
 for(double rate:{44100.,48000.,96000.,192000.}){
  auto record=[&](bool post,bool switchHeld,bool noise,int family,double drive){
   synth->prepare(rate);
   set(Param::Drive,0);
   set(Param::VoiceDrive,drive);
   set(Param::Volume,.035);
   set(Param::Cutoff,420);
   set(Param::Q,3);
   set(Param::AmpDelay,35);
   set(Param::AmpAttack,90);
   set(Param::AmpSustain,1);
   set(Param::AmpRelease,65);
   set(Param::SawLevel,noise?0:.4);
   set(Param::PulseLevel,0);
   set(Param::NoiseLevel,noise?.3:0);
   set(Param::FilterFamily,family==1?2:0);
   set(Param::ModernEnabled,family==2?1:0);
   set(Param::PostFilterAmp,post?1:0);
   synth->primeParameters();
   synth->dispatch({0,EventKind::NoteOn,45,.8});
   std::vector<float> samples(std::size_t(rate*.85));
   const auto split=std::size_t(rate*.16),release=std::size_t(rate*.35);
   synth->render(samples.data(),split);
   if(switchHeld)set(Param::PostFilterAmp,post?0:1);
   synth->render(samples.data()+split,release-split);
   synth->dispatch({0,EventKind::NoteOff,45});
   synth->render(samples.data()+release,samples.size()-release);
   if(synth->activeVoices()!=0)throw std::runtime_error("Released voice remained active");
   return samples;
  };
  for(int family=0;family<3;++family)for(bool noise:{false,true})for(double drive:{0.,16.}){
   const auto legacy=record(false,false,noise,family,drive);
   const auto post=record(true,false,noise,family,drive);
   if(legacy==post||post!=record(true,true,noise,family,drive)||legacy!=record(false,true,noise,family,drive)){
    std::cerr<<"Voice path failed to differ or changed a held voice\n";return 1;
   }
   const auto silent=[](float x){return x==0;};
   const auto quietEnd=std::size_t(rate*.8),delay=std::size_t(rate*.03);
   // The shared DC blocker can retain a decaying residual after voice release.
   const auto settled=[](float x){return std::abs(x)<1e-8f;};
   if(!std::all_of(post.begin(),post.begin()+delay,silent)||
      std::all_of(post.begin()+delay,post.begin()+quietEnd,silent)||
      !std::all_of(post.begin()+quietEnd,post.end(),settled)||
      !std::all_of(post.begin(),post.end(),[](float v){return std::isfinite(v)&&std::abs(v)<1;})){
    std::cerr<<"Post-filter envelope timing/output failed\n";return 1;
   }
  }
 }
 std::cout<<"PASS note-latched post-filter amplitude, oscillator/noise clean/drive, three filter paths, four rates\n";
}
