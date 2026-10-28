#pragma once
#include <array>
#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>

namespace memorylab {
constexpr double pi=3.14159265358979323846;
struct Parameters {
 double drive=5,depth=.8,recovery=250,detune=7,pulseWidth=.43,cutoff=1450,q=2.4,volume=.25;
 bool retrigger=true;
};
class MemoryDrive {
 double sr=48000,env=0,previous=0,dcX=0,dcY=0,d=5,dep=.8,rel=250;
public:
 void prepare(double rate){sr=rate;reset();}
 void reset(){env=previous=dcX=dcY=0;d=5;dep=.8;rel=250;}
 double tick(double input,const Parameters& p){
  const double s=1-std::exp(-1/(sr*.02));
  d+=(p.drive-d)*s;dep+=(p.depth-dep)*s;rel+=(p.recovery-rel)*s;
  double out=0;
  for(int k=1;k<=4;++k){
   const double u=previous+(input-previous)*k/4,level=std::abs(u)/.25;
   const double c=std::exp(-1/(sr*4*(level>env?.005:rel*.001)));
   env=c*env+(1-c)*level;
   const double t=std::min(1.,d/2),fade=t*t*(3-2*t),v=u/(1+dep*fade*env);
   out+=d<1e-7?v:std::tanh(d*v)/d;
  }
  previous=input;out/=4;
  const double y=out-dcX+std::exp(-2*pi*8/sr)*dcY;dcX=out;dcY=y;
  return y;
 }
};
enum class EventKind {NoteOn,NoteOff,AllOff,Parameter};
enum class Param {Drive,Depth,Recovery,Detune,PulseWidth,Cutoff,Q,Volume,Retrigger};
struct Event {std::size_t offset=0;EventKind kind=EventKind::AllOff;int note=60;double value=0;int channel=0;Param parameter=Param::Drive;};
struct Voice {
 bool active=false,gate=false;int note=0,channel=0;std::uint64_t age=0;
 double phase1=0,phase2=.17,env=0,velocity=0,elapsed=0,releaseStep=0,ic1=0,ic2=0;
};
inline double blep(double t,double dt){
 if(t<dt){t/=dt;return t+t-t*t-1;}
 if(t>1-dt){t=(t-1)/dt;return t*t+t+t+1;}
 return 0;
}
inline double sawSample(double phase,double dt){return 2*phase-1-blep(phase,dt);}
inline double pulseSample(double phase,double dt,double width){
 double shifted=phase-width;if(shifted<0)shifted+=1;
 return (phase<width?1.:-1.)+blep(phase,dt)-blep(shifted,dt)-(2*width-1);
}
class Synth {
 double sr=48000;std::uint64_t clock=0;std::array<Voice,8> voices{};MemoryDrive memory;
 Parameters target{},current{};
 void release(Voice& v){v.gate=false;v.releaseStep=v.env/(sr*.5);}
 void apply(const Event& e){
  if(e.kind==EventKind::Parameter){
   if(!std::isfinite(e.value))return;
   switch(e.parameter){
    case Param::Drive:target.drive=std::clamp(e.value,0.,12.);break;
    case Param::Depth:target.depth=std::clamp(e.value,0.,1.5);break;
    case Param::Recovery:target.recovery=std::clamp(e.value,50.,400.);break;
    case Param::Detune:target.detune=std::clamp(e.value,-30.,30.);break;
    case Param::PulseWidth:target.pulseWidth=std::clamp(e.value,.1,.9);break;
    case Param::Cutoff:target.cutoff=std::clamp(e.value,30.,std::min(18000.,sr*.4));break;
    case Param::Q:target.q=std::clamp(e.value,.5,8.);break;
    case Param::Volume:target.volume=std::clamp(e.value,0.,.8);break;
    case Param::Retrigger:target.retrigger=e.value>=.5;break;
   }return;
  }
  if(e.kind==EventKind::AllOff){for(auto& v:voices)if(v.active)release(v);return;}
  if(e.note<0||e.note>127||e.channel<0||e.channel>15||!std::isfinite(e.value))return;
  if(e.kind==EventKind::NoteOff||(e.kind==EventKind::NoteOn&&e.value<=0)){
   for(auto& v:voices)if(v.active&&v.note==e.note&&v.channel==e.channel)release(v);return;
  }
  Voice* chosen=nullptr;
  for(auto& v:voices)if(v.active&&v.note==e.note&&v.channel==e.channel){chosen=&v;break;}
  if(!chosen)for(auto& v:voices)if(!v.active){chosen=&v;break;}
  if(!chosen)chosen=&*std::min_element(voices.begin(),voices.end(),[](const Voice&a,const Voice&b){return a.age<b.age;});
  const double phase1=chosen->phase1,phase2=chosen->phase2;
  *chosen=Voice{};chosen->active=chosen->gate=true;chosen->note=e.note;chosen->channel=e.channel;
  chosen->velocity=std::clamp(e.value,0.,1.);chosen->age=++clock;
  if(!target.retrigger){chosen->phase1=phase1;chosen->phase2=phase2;}
 }
 double voiceTick(Voice& v){
  const double f=440*std::exp2((v.note-69)/12.),dt=std::min(.45,f/sr),dt2=std::min(.45,f*std::exp2(current.detune/1200)/sr);
  double saw=sawSample(v.phase1,dt);
  double pulse=pulseSample(v.phase2,dt2,current.pulseWidth);
  v.phase1+=dt;v.phase1-=std::floor(v.phase1);v.phase2+=dt2;v.phase2-=std::floor(v.phase2);
  if(v.gate){v.elapsed+=1/sr;v.env=v.elapsed<.012?v.elapsed/.012:v.elapsed<.192?1-.35*(v.elapsed-.012)/.18:.65;}
  else{v.env=std::max(0.,v.env-v.releaseStep);if(v.env<1e-8){v.active=false;return 0;}}
  const double x=(.42*saw+.28*pulse)*v.env*v.velocity;
  const double g=std::tan(pi*current.cutoff/sr),k=1/current.q;
  const double hp=(x-v.ic2-(k+g)*v.ic1)/(1+g*(g+k));
  const double bp=g*hp+v.ic1,lp=g*bp+v.ic2;
  v.ic1=2*bp-v.ic1;v.ic2=2*lp-v.ic2;return lp;
 }
public:
 bool prepare(double rate){if(!std::isfinite(rate)||rate<44100||rate>192000)return false;sr=rate;reset();return true;}
 void reset(){voices={};clock=0;target=current=Parameters{};memory.prepare(sr);}
 std::size_t activeVoices()const{std::size_t n=0;for(const auto&v:voices)n+=v.active;return n;}
 // Events must be sorted, with offsets in [0,frames). Invalid blocks are silent,
 // return false and leave state unchanged. Caller retains ownership of buffers.
 bool render(float* output,std::size_t frames,const Event* events=nullptr,std::size_t count=0){
  if(!output)return false;
  if(count&&!events){std::fill_n(output,frames,0.f);return false;}
  for(std::size_t j=0;j<count;++j)if(events[j].offset>=frames||(j&&events[j].offset<events[j-1].offset)){std::fill_n(output,frames,0.f);return false;}
  std::size_t j=0;const double smooth=1-std::exp(-1/(sr*.02));
  for(std::size_t i=0;i<frames;++i){
   while(j<count&&events[j].offset==i)apply(events[j++]);
   current.detune+=(target.detune-current.detune)*smooth;current.pulseWidth+=(target.pulseWidth-current.pulseWidth)*smooth;
   current.cutoff+=(target.cutoff-current.cutoff)*smooth;current.q+=(target.q-current.q)*smooth;current.volume+=(target.volume-current.volume)*smooth;
   double sum=0;for(auto&v:voices)if(v.active)sum+=voiceTick(v);
   output[i]=static_cast<float>(memory.tick(sum,target)*current.volume);
  }return true;
 }
};
}
