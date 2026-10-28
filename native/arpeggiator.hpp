#pragma once
#include "voice.hpp"

namespace memorylab {
// Fixed storage and a fractional sample clock; no MIDI buffer growth in render.
class Arpeggiator {
 std::array<double,2048> velocity{};
 std::array<bool,2048> down{};
 std::array<bool,16> pedal{};
 double stepLeft=0,gateLeft=0;
 int active=-1;std::uint64_t step=0;
public:
 static double beats(int division){constexpr double values[]{1,.5,.25,.125,1./3,1./6};return values[std::clamp(division,0,5)];}
 static int index(std::uint64_t stepNumber,int count,int mode){
  if(count<=1)return 0;
  if(mode==1)return count-1-stepNumber%count;
  if(mode==2){const int x=stepNumber%(2*count-2);return x<count?x:2*count-2-x;}
  return stepNumber%count;
 }
 int position()const{return step>0?int((step-1)%16):0;}
 int heldCount()const{int n=0;for(double v:velocity)if(v>0)++n;return n;}
 void reset(){velocity.fill(0);down.fill(false);pedal.fill(false);stepLeft=gateLeft=0;active=-1;step=0;}
 template<class Emit>void stop(Emit&&emit){if(active>=0)emit(EventKind::NoteOff,active%128,0.,active/128);active=-1;gateLeft=0;}
 template<class Emit>void restart(Emit&&emit){stop(emit);stepLeft=0;step=0;}
 template<class Emit>void note(int channel,int pitch,double value,Emit&&emit){
  if(channel<0||channel>15||pitch<0||pitch>127||!std::isfinite(value))return;
  const bool empty=heldCount()==0;const int key=channel*128+pitch;
  down[key]=value>0;if(value>0)velocity[key]=std::clamp(value,0.,1.);else if(!pedal[channel])velocity[key]=0;
  if(empty&&value>0){step=0;stepLeft=0;}
  if(heldCount()==0){stop(emit);step=0;stepLeft=0;}
 }
 template<class Emit>void sustain(int channel,bool value,Emit&&emit){
  pedal[channel]=value;if(!value)for(int n=0;n<128;++n)if(!down[channel*128+n])velocity[channel*128+n]=0;
  if(heldCount()==0){stop(emit);step=0;stepLeft=0;}
 }
 template<class Emit>void clearChannel(int channel,Emit&&emit){
  for(int n=0;n<128;++n){velocity[channel*128+n]=0;down[channel*128+n]=false;}pedal[channel]=false;
  if(active/128==channel&&active>=0)stop(emit);
  if(heldCount()==0){step=0;stepLeft=0;}
 }
 template<class Emit,class Render>void render(int frames,double sampleRate,double bpm,int division,int mode,int octaves,double gate,Emit&&emit,Render&&audio){
  const double interval=sampleRate*60/std::clamp(bpm,20.,400.)*beats(division);
  int offset=0;
  while(offset<frames){
   if(active>=0&&gateLeft<=1e-8)stop(emit);
   if(heldCount()==0){audio(offset,frames-offset);return;}
   if(stepLeft<=1e-8){
    stop(emit);
    // Octave layers, then ascending pitch, then MIDI channel. Out-of-range
    // transpositions are omitted, never clamped into duplicate high notes.
    int total=0;
    for(int o=0;o<std::clamp(octaves,1,3);++o)for(int n=0;n+12*o<128;++n)for(int c=0;c<16;++c)if(velocity[c*128+n]>0)++total;
    int target=index(step,total,mode),seen=0;
    for(int o=0;o<std::clamp(octaves,1,3);++o)for(int n=0;n+12*o<128;++n)for(int c=0;c<16;++c)if(velocity[c*128+n]>0){
     if(seen++==target){active=c*128+n+12*o;emit(EventKind::NoteOn,n+12*o,velocity[c*128+n],c);}
    }
    ++step;stepLeft+=interval;gateLeft=std::max(1.,interval*std::clamp(gate,.05,.95));
   }
   const double next=active>=0?std::min(stepLeft,gateLeft):stepLeft;
   const int count=std::min(frames-offset,std::max(1,int(std::ceil(next-1e-8))));
   audio(offset,count);offset+=count;stepLeft-=count;if(active>=0)gateLeft-=count;
  }
 }
};
}
