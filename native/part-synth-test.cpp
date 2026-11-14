#include "part-synth.hpp"
#include <iostream>
#include <stdexcept>
#include <vector>
#include <memory>
#include <cstdlib>
#include <new>
#include <fstream>
#include <string>
static bool counting=false;
static std::size_t allocations=0;
void* operator new(std::size_t n){if(counting)++allocations;if(void*p=std::malloc(n?n:1))return p;throw std::bad_alloc();}
void operator delete(void*p)noexcept{std::free(p);}
void operator delete(void*p,std::size_t)noexcept{std::free(p);}
void require(bool p,const char*why){if(!p)throw std::runtime_error(why);}
using namespace memorylab;
std::vector<Event> sequence(int sr){
 return {{0,EventKind::NoteOn,60,.8},{17,EventKind::NoteOn,64,.7},{53,EventKind::NoteOn,67,.9},
 {std::size_t(sr/8),EventKind::PitchBend,0,.2},{std::size_t(sr/7),EventKind::ChannelPressure,0,.4},
 {std::size_t(sr/6),EventKind::Sustain,0,1},{std::size_t(sr/5),EventKind::NoteOff,60},
 {std::size_t(sr/4),EventKind::NoteOff,64},{std::size_t(sr/3),EventKind::Sustain,0,0},
 {std::size_t(sr/2),EventKind::NoteOff,67},{std::size_t(sr*3/4),EventKind::AllSoundOff}};
}
template<class Engine>std::vector<float> render(Engine&e,int count,int block,const std::vector<Event>&events){
 std::vector<float>out(count);
 for(int pos=0;pos<count;){
  const int n=std::min(block,count-pos);std::vector<Event>local;
  for(auto event:events)if(event.offset>=std::size_t(pos)&&event.offset<std::size_t(pos+n)){event.offset-=pos;local.push_back(event);}
  require(e.render(out.data()+pos,n,local.data(),local.size()),"render failed");pos+=n;
 }
 return out;
}
void tests(){
 double peak=0;
 for(int sr:{44100,48000,96000,192000}){
  auto parts=std::make_unique<PartSynth>();auto single=std::make_unique<Synth>();
  require(parts->prepare(sr)&&single->prepare(sr),"prepare");
  auto events=sequence(sr);
  // Non-default settings verify that the legacy parameter-event route still
  // addresses part 1 and modern/oscillator features pass through unchanged.
  events.insert(events.begin(),{0,EventKind::Parameter,0,.2,0,Param::Osc1PulseLevel});
  events.insert(events.begin(),{0,EventKind::Parameter,0,1,0,Param::ModernEnabled});
  events.insert(events.begin(),{0,EventKind::Parameter,0,.3,0,Param::ModernProfile});
  auto a=render(*parts,sr,257,events),b=render(*single,sr,127,events);
  require(a==b,"one-part sound differs from single engine");
  require(parts->activeVoices()==0,"one-part panic stuck");
  PartAllocator::Layout layer{{{2,0,127,-1},{2,0,127,-1},{2,0,127,-1},{2,0,127,-1}}};
  auto full=std::make_unique<PartSynth>();auto split=std::make_unique<PartSynth>();
  require(full->prepare(sr)&&split->prepare(sr),"layer prepare");
  for(auto*p:{full.get(),split.get()}){
   require(p->configure(layer),"layer configure");
   for(int part=0;part<4;++part){
    p->parameter(part,Param::Drive,0);p->parameter(part,Param::Volume,.025);
    p->parameter(part,Param::Cutoff,400+part*700);
    p->parameter(part,Param::AmpAttack,10+part*30);
    if(part%2){p->parameter(part,Param::SawLevel,0);p->parameter(part,Param::Osc2SawLevel,.25);p->parameter(part,Param::Osc2Interval,12);}
   }
  }
  auto layered=render(*full,sr,1024,sequence(sr)),chunks=render(*split,sr,73,sequence(sr));
  require(layered==chunks,"layer block partition mismatch");
  double energy=0;for(float x:layered){require(std::isfinite(x)&&std::abs(x)<1,"layer finite/headroom");energy+=x*x;peak=std::max(peak,double(std::abs(x)));}
  require(energy>1e-6,"layers silent");
  require(full->activeVoices()==0,"layer all sound off stuck");
 }
 // An explicit one-note layer equals the sample-wise sum of four independently
 // configured references, proving sound settings do not bleed between parts.
 auto multi=std::make_unique<PartSynth>();multi->prepare(48000);
 PartAllocator::Layout layout{{{2,0,127,-1},{2,0,127,-1},{2,0,127,-1},{2,0,127,-1}}};
 require(multi->configure(layout),"reference layout");
 std::array<std::unique_ptr<Synth>,4> refs;
 constexpr int length=24000;
 std::vector<float> expected(length,0),scratch(length),actual(length);
 for(int part=0;part<4;++part){
  refs[part]=std::make_unique<Synth>();refs[part]->prepare(48000);
  for(auto setting:std::array<std::pair<Param,double>,5>{{{Param::Drive,0},{Param::Volume,.02},{Param::Cutoff,300+part*900},{Param::AmpAttack,part*30.},{Param::PulseWidth,.2+part*.15}}}){
   multi->parameter(part,setting.first,setting.second);refs[part]->dispatch({0,EventKind::Parameter,0,setting.second,0,setting.first});
  }
  refs[part]->dispatch({0,EventKind::NoteOn,60,.8});refs[part]->render(scratch.data(),length);
  for(int i=0;i<length;++i)expected[i]+=scratch[i];
 }
 multi->dispatch({0,EventKind::NoteOn,60,.8});multi->render(actual.data(),length);
 require(actual==expected,"part settings or output mix differ from independent references");
 auto noiseLayer=std::make_unique<PartSynth>();auto noiseSingle=std::make_unique<Synth>();
 noiseLayer->prepare(48000);noiseSingle->prepare(48000);
 require(noiseLayer->configure({{{4,0,127,-1},{4,0,127,-1},{},{}}}),"noise layer layout");
 for(auto setting:std::array<std::pair<Param,double>,5>{{{Param::Drive,0},{Param::Volume,.02},{Param::SawLevel,0},{Param::PulseLevel,0},{Param::NoiseLevel,.2}}}){
  for(int part=0;part<2;++part)noiseLayer->parameter(part,setting.first,setting.second);
  noiseSingle->dispatch({0,EventKind::Parameter,0,setting.second,0,setting.first});
 }
 noiseLayer->render(actual.data(),length);noiseSingle->render(expected.data(),length);
 noiseLayer->dispatch({0,EventKind::NoteOn,60,.8});noiseSingle->dispatch({0,EventKind::NoteOn,60,.8});
 noiseLayer->render(actual.data(),length);noiseSingle->render(expected.data(),length);
 bool independent=false;for(int i=0;i<length;++i)independent|=actual[i]!=(expected[i]+expected[i]);
 require(independent,"noise layers use the same stream");
 // Fixed reservations cap audio voices even when all parts overlap.
 for(int note=61;note<80;++note){
  multi->dispatch({0,EventKind::NoteOn,note,.8});
  require(multi->activeVoices()==8,"layer exceeded global budget");
  for(int part=0;part<4;++part)require(multi->activeVoices(part)==2,"part reservation exceeded");
 }
 multi->dispatch({0,EventKind::AllSoundOff});
 layout={{{4,0,59,0},{4,60,127,1},{},{}}};require(multi->configure(layout),"split layout");
 multi->dispatch({0,EventKind::NoteOn,59,.8,0});multi->dispatch({0,EventKind::NoteOn,60,.8,1});multi->dispatch({0,EventKind::NoteOn,60,.8,0});
 require(multi->activeVoices()==2&&multi->sounding(0,59,0)&&multi->sounding(1,60,1),"split/channel routing");
 multi->dispatch({0,EventKind::Sustain,0,1,0});multi->dispatch({0,EventKind::NoteOff,59,0,0});
 multi->render(actual.data(),length);require(multi->sounding(0,59,0),"sustain lost split note");
 multi->dispatch({0,EventKind::ChannelAllSoundOff,0,0,0});
 require(multi->activeVoices(0)==0&&multi->activeVoices(1)==1,"channel panic crossed parts");
 auto invalid=layout;invalid[1].voices=8;
 require(!multi->configure(invalid)&&multi->activeVoices(1)==1,"invalid layout killed audio");
 require(multi->configure(layout)&&multi->activeVoices()==0,"layout change stranded audio");
 // Rejected block leaves audio state untouched; follow with matching notes.
 auto control=std::make_unique<PartSynth>();control->prepare(48000);control->configure(layout);
 Event bad{std::size_t(length),EventKind::NoteOn,60,.8};
 require(!multi->render(actual.data(),length,&bad,1),"invalid offsets accepted");
 require(std::all_of(actual.begin(),actual.end(),[](float x){return x==0;}),"invalid block not silent");
 // Parameter/smoothing history differs on this edited engine; use fresh pair.
 auto clean=std::make_unique<PartSynth>();clean->prepare(48000);clean->configure(layout);
 require(!clean->render(actual.data(),length,&bad,1),"fresh invalid offset accepted");
 clean->dispatch({0,EventKind::NoteOn,50,.8,0});control->dispatch({0,EventKind::NoteOn,50,.8,0});
 clean->render(actual.data(),length);control->render(expected.data(),length);require(actual==expected,"invalid block changed state");
 // Every appended native control must remain reachable through the part guard.
 for(int part=0;part<4;++part)for(int parameter=0;parameter<=int(Param::Lfo4Unipolar);++parameter)require(multi->parameter(part,Param(parameter),0),"valid appended part parameter rejected");
 require(!multi->parameter(0,Param(int(Param::Lfo4Unipolar)+1),0),"invalid parameter accepted");
 // Audio dispatch and rendering must not allocate after preparation.
 allocations=0;counting=true;
 for(int n=0;n<100;++n){multi->dispatch({0,EventKind::NoteOn,50+n%8,.8,0});multi->render(actual.data(),256);multi->dispatch({0,EventKind::NoteOff,50+n%8,0,0});}
 counting=false;require(allocations==0,"audio path allocated");
 std::cout<<"PASS part DSP: exact single-part parity at four rates, independent four-sound reference sum, layered block parity/headroom, eight-voice cap, splits/channels/sustain/panic, atomic bad blocks/layouts, no audio allocations; fixture peak="<<peak<<"\n";
}
int main(){try{tests();return 0;}catch(const std::exception&e){counting=false;std::cerr<<"FAIL: "<<e.what()<<'\n';return 1;}}

