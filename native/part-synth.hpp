#pragma once
#include "voice.hpp"
#include "part-allocator.hpp"
#include <array>
#include <algorithm>

namespace memorylab {
// Native integration study, not wired into the VST3. Four independent sound
// engines, but only eight globally reserved voice slots can be active.
class PartSynth {
 std::array<Synth,4> parts{};
 PartAllocator allocator;
 bool ready=false;
 int start(int part)const{int n=0;for(int p=0;p<part;++p)n+=allocator.layout()[p].voices;return n;}
 void sync(){
  int global=0;
  for(int p=0;p<4;++p)for(int local=0;local<allocator.layout()[p].voices;++local,++global){
   const auto status=parts[p].voiceStatus(local);
   if(status.active)allocator.level(global,status.level);else allocator.finished(global);
  }
 }
 bool range(float*out,std::size_t frames){
  int only=-1,enabled=0;
  for(int p=0;p<4;++p)if(allocator.layout()[p].voices){only=p;++enabled;}
  // No arithmetic or extra latency in the one-part route.
  if(enabled==1){const bool ok=parts[only].render(out,frames);sync();return ok;}
  std::fill_n(out,frames,0.f);
  if(enabled==0)return true;
  std::array<float,256> scratch{};
  for(std::size_t offset=0;offset<frames;offset+=scratch.size()){
   const auto n=std::min(scratch.size(),frames-offset);
   for(int p=0;p<4;++p)if(allocator.layout()[p].voices){
    if(!parts[p].render(scratch.data(),n))return false;
    for(std::size_t i=0;i<n;++i)out[offset+i]+=scratch[i];
   }
  }
  sync();return true;
 }
public:
 bool prepare(double sr){
  if(!std::isfinite(sr)||sr<44100||sr>192000)return false;
  for(auto&p:parts)if(!p.prepare(sr))return false;
  allocator.configure({{{8,0,127,-1},{},{},{}}});ready=true;return true;
 }
 const PartAllocator::Layout& layout()const{return allocator.layout();}
 bool configure(const PartAllocator::Layout& next){
  const auto prior=allocator.layout();
  if(!allocator.configure(next))return false;
  for(int p=0;p<4;++p){parts[p].dispatch({0,EventKind::AllSoundOff});if(prior[p].voices==0&&next[p].voices>0)parts[p].primeParameters();}
  return true;
 }
 bool parameter(int part,Param parameter,double value){
  if(part<0||part>3||int(parameter)<0||int(parameter)>int(Param::Lfo4Unipolar)||!std::isfinite(value))return false;
  parts[part].dispatch({0,EventKind::Parameter,0,value,0,parameter});return true;
 }
 std::size_t activeVoices()const{std::size_t n=0;for(const auto&p:parts)n+=p.activeVoices();return n;}
 std::size_t activeVoices(int part)const{return part>=0&&part<4?parts[part].activeVoices():0;}
 bool sounding(int part,int note,int channel)const{return part>=0&&part<4&&parts[part].sounding(note,channel);}
 void dispatch(const Event&e){
  if(e.kind==EventKind::Parameter){parameter(0,e.parameter,e.value);return;}
  if(e.kind==EventKind::AllSoundOff){allocator.allSoundOff();for(auto&p:parts)p.dispatch(e);return;}
  if(e.kind==EventKind::AllOff){allocator.allNotesOff();for(auto&p:parts)p.dispatch(e);return;}
  if(e.channel<0||e.channel>15||!std::isfinite(e.value))return;
  if(e.kind==EventKind::NoteOn||e.kind==EventKind::NoteOff){
   if(e.note<0||e.note>127)return;
   if(e.kind==EventKind::NoteOn&&e.value>0){
    sync();const auto result=allocator.noteOn(e.note,e.channel,e.value);
    for(int i=0;i<result.count;++i){const auto&a=result.assignments[i];parts[a.part].assignVoice(a.slot-start(a.part),e,std::uint32_t(a.part));}
    return;
   }
   allocator.noteOff(e.note,e.channel);
  }else if(e.kind==EventKind::Sustain)allocator.pedal(e.channel,e.value>=.5);
  else if(e.kind==EventKind::ChannelAllOff)allocator.allNotesOff(e.channel);
  else if(e.kind==EventKind::ChannelAllSoundOff)allocator.allSoundOff(e.channel);
  for(auto&p:parts)p.dispatch(e);
 }
 // Same offset contract as Synth: validate the whole block before edits.
 // EventKind::Parameter targets part 1 for legacy compatibility; other parts
 // can be set explicitly between render ranges with parameter().
 bool render(float*out,std::size_t frames,const Event*events=nullptr,std::size_t count=0){
  if(!out)return false;
  if(!ready||(count&&!events)){std::fill_n(out,frames,0.f);return false;}
  for(std::size_t i=0;i<count;++i)if(events[i].offset>=frames||(i&&events[i].offset<events[i-1].offset)){std::fill_n(out,frames,0.f);return false;}
  std::size_t cursor=0;
  for(std::size_t i=0;i<count;++i){
   if(events[i].offset>cursor&&!range(out+cursor,events[i].offset-cursor))return false;
   cursor=events[i].offset;dispatch(events[i]);
  }
  if(cursor<frames)return range(out+cursor,frames-cursor);
  return true;
 }
};
}

