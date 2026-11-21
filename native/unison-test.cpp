#include "part-synth.hpp"
#include <cstdlib>
#include <iostream>
#include <memory>

using namespace memorylab;

void check(bool ok,const char* message){
 if(!ok){std::cerr<<message<<"\n";std::exit(1);}
}

int main(){
 PartAllocator allocator;
 check(allocator.noteOn(48,0,1).count==1,"default stack");
 check(allocator.setUnisonSize(0,4),"set stack");
 const auto first=allocator.noteOn(48,0,1);
 check(first.count==4&&first.assignments[3].member==3,"four members");
 check(allocator.noteOn(55,0,1).count==4&&allocator.activeCount()==8,"two four-voice notes");
 const auto reused=allocator.noteOn(48,0,1);
 check(reused.count==4&&reused.assignments[0].slot==0,"retrigger reuses stack");
 const auto stolen=allocator.noteOn(60,0,1);
 check(stolen.count==4&&stolen.assignments[0].slot==4&&allocator.activeCount()==8,"steal oldest stack");
 allocator.pedal(0,true);
 allocator.noteOff(48,0);
 for(int i=0;i<4;++i)check(!allocator.state()[i].keyDown&&allocator.state()[i].gate,"pedal holds stack");
 allocator.pedal(0,false);
 for(int i=0;i<4;++i)check(!allocator.state()[i].gate,"pedal releases stack");
 check(allocator.noteOn(64,0,1).assignments[0].slot==0,"prefer releasing stack");

 const PartAllocator::Layout layout{{{2,0,127,-1},{2,0,127,-1},{2,0,127,-1},{2,0,127,-1}}};
 check(allocator.configure(layout),"four layers");
 for(int part=1;part<4;++part)check(allocator.setUnisonSize(part,4),"layer request");
 const auto layered=allocator.noteOn(60,0,1);
 check(layered.count==8,"four layers share eight");
 for(int i=0;i<8;++i)check(layered.assignments[i].stackSize==2&&layered.assignments[i].part==i/2,"layer cap");

 auto synth=std::make_unique<PartSynth>();
 using Audio=std::array<std::array<float,8192>,2>;
 auto render=[&](int size,double detune,double spread,int block){
  check(synth->prepare(48000)&&synth->setUnisonSize(0,size),"prepare render");
  synth->parameter(0,Param::UnisonDetune,detune);
  synth->parameter(0,Param::StereoSpread,spread);
  synth->parameter(0,Param::NoiseLevel,0);
  synth->parameter(0,Param::Drive,0);
  synth->parameter(0,Param::Volume,.08);
  Audio out{};
  check(synth->render(out[0].data(),out[0].size()),"settle parameters");
  synth->dispatch({0,EventKind::NoteOn,48,1});
  for(int pos=0;pos<int(out[0].size());pos+=block)
   check(synth->render(out[0].data()+pos,std::min(block,int(out[0].size())-pos),nullptr,0,out[1].data()+pos),"render");
  check(synth->activeVoices()==std::size_t(size),"DSP stack count");
  for(auto& channel:out)for(float sample:channel)check(std::isfinite(sample)&&std::abs(sample)<1,"headroom");
  return out;
 };
 const auto single=render(1,0,0,127);
 const auto stacked=render(4,0,0,127);
 for(std::size_t i=0;i<single[0].size();++i)check(std::abs(single[0][i]-stacked[0][i])<1e-6,"coherent gain");
 const auto doubleVoice=render(2,12,0,127);
 const auto wide=render(4,12,1,127);
 check(single!=doubleVoice&&doubleVoice[0]==doubleVoice[1]&&wide[0]!=wide[1],"detune and stereo");
 check(wide==render(4,12,1,256),"block parity");

 check(synth->configure(layout),"configure synth layers");
 synth->dispatch({0,EventKind::Sustain,0,1,0});
 synth->dispatch({0,EventKind::NoteOn,60,1});
 check(synth->setUnisonSize(0,1)&&synth->activeVoices(0)==0&&synth->activeVoices(1)==1,"size change is part-local");
 synth->dispatch({0,EventKind::NoteOn,64,1});
 synth->dispatch({0,EventKind::NoteOff,64,0});
 std::array<float,256> buffer{};
 for(int i=0;i<512;++i)check(synth->render(buffer.data(),buffer.size()),"held pedal render");
 check(synth->sounding(0,64,0),"size edit restores pedal");
 synth->dispatch({0,EventKind::Sustain,0,0,0});
 for(int i=0;i<512;++i)check(synth->render(buffer.data(),buffer.size()),"release render");
 check(!synth->sounding(0,64,0),"stack releases at pedal up");
 std::cout<<"PASS unison allocation, sustain, stealing, gain, stereo and block parity\n";
}
