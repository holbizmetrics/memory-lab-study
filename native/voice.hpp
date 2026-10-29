#pragma once
#include <array>
#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>

namespace memorylab {
constexpr double pi=3.14159265358979323846;
struct EnvelopeSettings {double delay=0,attack=.02,hold=0,decay=.35,sustain=.15,release=.4;};
inline double envelopeLevel(double elapsed,const EnvelopeSettings&s){
 const double t=elapsed-s.delay;if(t<0)return 0;
 if(s.attack>0&&t<s.attack)return t/s.attack;
 if(t<s.attack+s.hold)return 1;
 if(s.decay>0&&t<s.attack+s.hold+s.decay)return 1-(1-s.sustain)*(t-s.attack-s.hold)/s.decay;
 return s.sustain;
}
struct Parameters {
 double drive=5,depth=.8,recovery=250,detune=7,pulseWidth=.43,cutoff=1450,q=2.4,volume=.25,profile=1;
 bool retrigger=true;
 double lfoRate=4.5,pitchLfo=0,pwmLfo=0,cutoffLfo=0,filterEnvelope=0;
 double pressureDepth=0,voiceDrive=0;
 EnvelopeSettings ampEnvelope{0,.012,0,.18,.65,.5},filterEnvelopeShape{};
};
class MemoryDrive {
 double sr=48000,env=0,previous=0,dcX=0,dcY=0,d=5,dep=.8,rel=250;
public:
 void prepare(double rate){sr=rate;reset();}
 void reset(){env=previous=dcX=dcY=0;d=5;dep=.8;rel=250;}
 void clearSignal(){env=previous=dcX=dcY=0;}
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
enum class EventKind {NoteOn,NoteOff,AllOff,Parameter,Sustain,PitchBend,ChannelPressure,AllSoundOff,ChannelAllOff,ChannelAllSoundOff};
enum class Param {Drive,Depth,Recovery,Detune,PulseWidth,Cutoff,Q,Volume,Retrigger,Profile,LfoRate,PitchLfo,PwmLfo,CutoffLfo,FilterEnvelope,PressureDepth,AmpDelay,AmpAttack,AmpHold,AmpDecay,AmpSustain,AmpRelease,FilterDelay,FilterAttack,FilterHold,FilterDecay,FilterSustain,FilterRelease,VoiceDrive};
struct Event {std::size_t offset=0;EventKind kind=EventKind::AllOff;int note=60;double value=0;int channel=0;Param parameter=Param::Drive;};
class ClassicFilter {
 double ic1=0,ic2=0;
public:
 void reset(){ic1=ic2=0;}
 double tick(double x,double sr,double cutoff,double q,double profile){
  const double g=std::tan(pi*cutoff/sr),k=1/q;
  const double hp=(x-ic2-(k+g)*ic1)/(1+g*(g+k));
  const double bp=g*hp+ic1,lp=g*bp+ic2;
  ic1=2*bp-ic1;ic2=2*lp-ic2;
  // Linear HP -> (HP+LP) notch -> LP blend. Shared state, no path restart.
  const double highWeight=profile<=.5?1:2-2*profile;
  const double lowWeight=profile<=.5?2*profile:1;
  return highWeight*hp+lowWeight*lp;
 }
};
class FilterEnvelope {
 bool gate=false;double time=0,value=0,step=0;
 EnvelopeSettings settings{};
public:
 void noteOn(const EnvelopeSettings& next=EnvelopeSettings{}){settings=next;gate=true;time=value=step=0;}
 void noteOff(double sr){gate=false;if(settings.release==0){value=step=0;}else step=value/(sr*settings.release);}
 double tick(double sr){
  if(gate){time+=1/sr;value=envelopeLevel(time,settings);}
  else value=std::max(0.,value-step);
  return value;
 }
};
struct Voice {
 bool active=false,gate=false;int note=0,channel=0;std::uint64_t age=0;
 double phase1=0,phase2=.17,env=0,velocity=0,elapsed=0,releaseStep=0;
 ClassicFilter filter;
 FilterEnvelope filterEnv,ampEnv;double lfoPhase=0;
 bool keyDown=false;double lastOutput=0,transitionValue=0;int transitionLeft=0;
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
// Original Fourier mip bank. Construct during prepare, never on the audio thread.
class OscillatorBank {
 static constexpr int size=4096,levels=10;
 std::array<std::array<float,size>,levels> tables{};
 bool ready=false;
 double lookup(int level,double phase)const{
  phase-=std::floor(phase);const double index=phase*size;const int i=int(index);const double t=index-i;
  return tables[level][i]*(1-t)+tables[level][(i+1)%size]*t;
 }
public:
 void prepare(){if(ready)return;
  for(int i=0;i<size;++i){double sum=0;int level=0;
   for(int h=1;h<=512;++h){sum-=2*std::sin(2*pi*h*i/size)/(pi*h);
    if(h==(1<<level)){tables[level][i]=float(sum);++level;}
   }
  }ready=true;
 }
 double saw(double phase,double dt)const{
  const double allowed=std::max(1.,.45/dt),octave=std::log2(allowed);
  const int level=std::min(levels-1,int(octave));
  if(level==0)return lookup(0,phase);
  const double blend=std::clamp(octave-level,0.,1.);
  // Both banks used are below the frequency guard. Blend continuously across
  // harmonic-count boundaries; highest bank stays fixed after its fade-in.
  return lookup(level-1,phase)*(1-blend)+lookup(level,phase)*blend;
 }
 double pulse(double phase,double dt,double width)const{return saw(phase-width,dt)-saw(phase,dt);}
};
class Synth {
#if MEMORYLAB_EXPERIMENTAL_OSCILLATOR_MIX
 double studySawLevel=.42,studyPulseLevel=.28,studyInterval=0;
#endif
#if MEMORYLAB_EXPERIMENTAL_VOICE_DRIVE
 bool studyDriveEnabled=false;double studyDriveGain=1;std::uint64_t studyClipped=0;
#endif
 double sr=48000;std::uint64_t clock=0;std::array<Voice,8> voices{};MemoryDrive memory;
 Parameters target{},current{};OscillatorBank bank;
 double voiceDriveGain=1,voiceDriveMix=0;
 std::array<bool,16> sustain{};
 std::array<double,16> bend{},bendTarget{},pressure{},pressureTarget{};
 void release(Voice& v){v.gate=false;v.ampEnv.noteOff(sr);v.filterEnv.noteOff(sr);}
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
    case Param::Profile:target.profile=std::clamp(e.value,0.,1.);break;
    case Param::LfoRate:target.lfoRate=std::clamp(e.value,.05,20.);break;
    case Param::PitchLfo:target.pitchLfo=std::clamp(e.value,-50.,50.);break;
    case Param::PwmLfo:target.pwmLfo=std::clamp(e.value,-.35,.35);break;
    case Param::CutoffLfo:target.cutoffLfo=std::clamp(e.value,-2.,2.);break;
    case Param::FilterEnvelope:target.filterEnvelope=std::clamp(e.value,-4.,4.);break;
    case Param::PressureDepth:target.pressureDepth=std::clamp(e.value,-2.,2.);break;
    case Param::AmpDelay:target.ampEnvelope.delay=std::clamp(e.value,0.,5000.)/1000.;break;
    case Param::AmpAttack:target.ampEnvelope.attack=std::clamp(e.value,0.,10000.)/1000.;break;
    case Param::AmpHold:target.ampEnvelope.hold=std::clamp(e.value,0.,5000.)/1000.;break;
    case Param::AmpDecay:target.ampEnvelope.decay=std::clamp(e.value,0.,10000.)/1000.;break;
    case Param::AmpSustain:target.ampEnvelope.sustain=std::clamp(e.value,0.,1.);break;
    case Param::AmpRelease:target.ampEnvelope.release=std::clamp(e.value,0.,10000.)/1000.;break;
    case Param::FilterDelay:target.filterEnvelopeShape.delay=std::clamp(e.value,0.,5000.)/1000.;break;
    case Param::FilterAttack:target.filterEnvelopeShape.attack=std::clamp(e.value,0.,10000.)/1000.;break;
    case Param::FilterHold:target.filterEnvelopeShape.hold=std::clamp(e.value,0.,5000.)/1000.;break;
    case Param::FilterDecay:target.filterEnvelopeShape.decay=std::clamp(e.value,0.,10000.)/1000.;break;
    case Param::FilterSustain:target.filterEnvelopeShape.sustain=std::clamp(e.value,0.,1.);break;
    case Param::FilterRelease:target.filterEnvelopeShape.release=std::clamp(e.value,0.,10000.)/1000.;break;
    case Param::VoiceDrive:target.voiceDrive=std::clamp(e.value,0.,24.);break;
   }return;
  }
  if(e.kind==EventKind::AllOff){sustain.fill(false);for(auto& v:voices)if(v.active){v.keyDown=false;release(v);}return;}
  if(e.kind==EventKind::AllSoundOff){voices={};sustain.fill(false);memory.clearSignal();return;}
  if(e.channel<0||e.channel>15||!std::isfinite(e.value))return;
  if(e.kind==EventKind::ChannelAllOff){sustain[e.channel]=false;for(auto&v:voices)if(v.active&&v.channel==e.channel){v.keyDown=false;release(v);}return;}
  if(e.kind==EventKind::ChannelAllSoundOff){sustain[e.channel]=false;bool any=false;for(auto&v:voices){if(v.channel==e.channel)v=Voice{};any|=v.active;}if(!any)memory.clearSignal();return;}
  if(e.kind==EventKind::Sustain){const bool down=e.value>=.5;sustain[e.channel]=down;
   if(!down)for(auto&v:voices)if(v.active&&v.channel==e.channel&&!v.keyDown&&v.gate)release(v);return;}
  if(e.kind==EventKind::PitchBend){bendTarget[e.channel]=std::clamp(e.value,-1.,1.);return;}
  if(e.kind==EventKind::ChannelPressure){pressureTarget[e.channel]=std::clamp(e.value,0.,1.);return;}
  if(e.kind!=EventKind::NoteOn&&e.kind!=EventKind::NoteOff)return;
  if(e.note<0||e.note>127||e.channel<0||e.channel>15||!std::isfinite(e.value))return;
  if(e.kind==EventKind::NoteOff||(e.kind==EventKind::NoteOn&&e.value<=0)){
   for(auto& v:voices)if(v.active&&v.note==e.note&&v.channel==e.channel&&v.keyDown){v.keyDown=false;if(!sustain[e.channel])release(v);}return;
  }
  Voice* chosen=nullptr;
  for(auto& v:voices)if(v.active&&v.note==e.note&&v.channel==e.channel){chosen=&v;break;}
  if(!chosen)for(auto& v:voices)if(!v.active){chosen=&v;break;}
  if(!chosen)for(auto&v:voices)if(!v.gate&&(!chosen||v.env<chosen->env))chosen=&v;
  if(!chosen)chosen=&*std::min_element(voices.begin(),voices.end(),[](const Voice&a,const Voice&b){return a.age<b.age;});
  const double phase1=chosen->phase1,phase2=chosen->phase2;
  const bool replacing=chosen->active;const double oldOutput=chosen->lastOutput;
  *chosen=Voice{};chosen->active=chosen->gate=true;chosen->note=e.note;chosen->channel=e.channel;
  chosen->velocity=std::clamp(e.value,0.,1.);chosen->age=++clock;
  chosen->filterEnv.noteOn(target.filterEnvelopeShape);chosen->ampEnv.noteOn(target.ampEnvelope);
  chosen->keyDown=true;if(replacing){chosen->transitionLeft=int(sr*.005);chosen->transitionValue=oldOutput;}
  if(!target.retrigger){chosen->phase1=phase1;chosen->phase2=phase2;}
 }
 double voiceTick(Voice& v){
  const double lfo=std::sin(2*pi*v.lfoPhase),filterEnv=v.filterEnv.tick(sr);
  v.lfoPhase+=current.lfoRate/sr;v.lfoPhase-=std::floor(v.lfoPhase);
  const double f=440*std::exp2((v.note-69)/12.)*std::exp2((current.pitchLfo*lfo+200*bend[v.channel])/1200),dt=std::min(.45,f/sr);
#if MEMORYLAB_EXPERIMENTAL_OSCILLATOR_MIX
  const double dt2=std::min(.45,f*std::exp2(studyInterval/12.+current.detune/1200)/sr);
#else
  const double dt2=std::min(.45,f*std::exp2(current.detune/1200)/sr);
#endif
  double saw=bank.saw(v.phase1,dt);
  double pulse=bank.pulse(v.phase2,dt2,std::clamp(current.pulseWidth+current.pwmLfo*lfo,.1,.9));
  v.phase1+=dt;v.phase1-=std::floor(v.phase1);v.phase2+=dt2;v.phase2-=std::floor(v.phase2);
  v.env=v.ampEnv.tick(sr);if(!v.gate&&v.env<1e-8){v.active=false;return 0;}
#if MEMORYLAB_EXPERIMENTAL_OSCILLATOR_MIX
  const double x=(studySawLevel*saw+studyPulseLevel*pulse)*v.env*v.velocity;
#else
  const double x=(.42*saw+.28*pulse)*v.env*v.velocity;
#endif
  const double cutoff=std::clamp(current.cutoff*std::exp2(current.cutoffLfo*lfo+current.filterEnvelope*filterEnv*v.velocity+current.pressureDepth*pressure[v.channel]),30.,std::min(18000.,sr*.4));
  double output=v.filter.tick(x,sr,cutoff,current.q,current.profile);
  // Bypass is exact. Fade in over the first dB so turning the stage on cannot
  // suddenly truncate a high-resonance signal already above the clip threshold.
  if(voiceDriveMix>0)output+=(std::clamp(output*voiceDriveGain,-1.,1.)-output)*voiceDriveMix;
#if MEMORYLAB_EXPERIMENTAL_VOICE_DRIVE
  if(studyDriveEnabled){output*=studyDriveGain;if(std::abs(output)>1)++studyClipped;output=std::clamp(output,-1.,1.);}
#endif
  if(v.transitionLeft>0){const double weight=double(v.transitionLeft--)/int(sr*.005);output=output*(1-weight)+v.transitionValue*weight;}
  v.lastOutput=output;return output;
 }
public:
#if MEMORYLAB_EXPERIMENTAL_OSCILLATOR_MIX
 // Offline configuration only. No live control or smoothing contract yet.
 bool setStudyOscillators(double sawLevel,double pulseLevel,double interval){
  if(!std::isfinite(sawLevel)||!std::isfinite(pulseLevel)||!std::isfinite(interval)||sawLevel<0||sawLevel>1||pulseLevel<0||pulseLevel>1||interval<-24||interval>24)return false;
  studySawLevel=sawLevel;studyPulseLevel=pulseLevel;studyInterval=interval;return true;
 }
#endif
#if MEMORYLAB_EXPERIMENTAL_VOICE_DRIVE
 // Offline experiment only: not compiled into the plugin or standard native core.
 // Configure between renders, never concurrently with the audio thread.
 bool setStudyVoiceDrive(bool enabled,double db){
  if(!std::isfinite(db)||db<0||db>24)return false;
  studyDriveEnabled=enabled;studyDriveGain=std::pow(10.,db/20.);studyClipped=0;return true;
 }
 std::uint64_t studyClippedVoiceSamples()const{return studyClipped;}
#endif
 bool prepare(double rate){if(!std::isfinite(rate)||rate<44100||rate>192000)return false;sr=rate;bank.prepare();reset();return true;}
 void reset(){voices={};clock=0;target=current=Parameters{};voiceDriveGain=1;voiceDriveMix=0;memory.prepare(sr);sustain.fill(false);bend={};bendTarget={};pressure={};pressureTarget={};}
 std::size_t activeVoices()const{std::size_t n=0;for(const auto&v:voices)n+=v.active;return n;}
 std::size_t heldVoices(int channel=-1)const{std::size_t n=0;for(const auto&v:voices)if(v.active&&v.keyDown&&(channel<0||v.channel==channel))++n;return n;}
 bool sounding(int note,int channel)const{for(const auto&v:voices)if(v.active&&v.note==note&&v.channel==channel)return true;return false;}
 // Audio-thread-only event dispatch for a wrapper that renders between events.
 void dispatch(const Event& event){apply(event);}
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
   current.profile+=(target.profile-current.profile)*smooth;
   current.lfoRate+=(target.lfoRate-current.lfoRate)*smooth;current.pitchLfo+=(target.pitchLfo-current.pitchLfo)*smooth;
   current.pwmLfo+=(target.pwmLfo-current.pwmLfo)*smooth;current.cutoffLfo+=(target.cutoffLfo-current.cutoffLfo)*smooth;
   current.filterEnvelope+=(target.filterEnvelope-current.filterEnvelope)*smooth;
   current.pressureDepth+=(target.pressureDepth-current.pressureDepth)*smooth;
   current.voiceDrive+=(target.voiceDrive-current.voiceDrive)*smooth;
   if(target.voiceDrive==0&&current.voiceDrive<1e-6)current.voiceDrive=0;
   voiceDriveMix=std::min(1.,current.voiceDrive);
   voiceDriveGain=voiceDriveMix>0?std::pow(10.,current.voiceDrive/20.):1;
   for(int channel=0;channel<16;++channel){bend[channel]+=(bendTarget[channel]-bend[channel])*smooth;pressure[channel]+=(pressureTarget[channel]-pressure[channel])*smooth;}
   double sum=0;for(auto&v:voices)if(v.active)sum+=voiceTick(v);
   output[i]=static_cast<float>(memory.tick(sum,target)*current.volume);
  }return true;
 }
};
}
