#pragma once
#include <array>
#include <algorithm>
#include <cmath>
#include <cstdint>

namespace memorylab {
// Fixed voice reservations: four parts share eight slots, never four pools of
// eight. Policy only; oscillator/envelope state remains owned by the DSP.
struct PartZone {
 int voices=0,low=0,high=127,channel=-1; // -1 = omni, otherwise zero-based
};
class PartAllocator {
public:
 struct Slot {
  bool active=false,keyDown=false,gate=false;
  int part=0,note=0,channel=0;
  std::uint64_t age=0;double level=0;
 };
 struct Assignment {int slot=0,part=0,note=0,channel=0;bool replacing=false;int stackSize=1,member=0;};
 struct NoteResult {std::array<Assignment,8> assignments{};int count=0;};
 using Layout=std::array<PartZone,4>;
private:
 Layout zones{{{8,0,127,-1},{},{},{}}};
 std::array<Slot,8> slots{};
 std::array<bool,16> sustain{};
 std::array<int,4> unisonSize{{1,1,1,1}};
 std::uint64_t clock=0;
public:
 const Layout& layout()const{return zones;}
 const std::array<Slot,8>& state()const{return slots;}
 void clear(){slots={};sustain={};clock=0;}
 // Configure between audio blocks. An accepted layout clears old voices so a
 // later note-off cannot strand a voice in a reassigned zone. Invalid = no edits.
 bool configure(const Layout& next){
  int total=0;
  for(const auto&p:next){
   if(p.voices<0||p.voices>8||p.low<0||p.high>127||p.low>p.high||p.channel< -1||p.channel>15)return false;
   total+=p.voices;
  }
  if(total>8)return false;
  zones=next;clear();return true;
 }
 int activeCount()const{int n=0;for(const auto&s:slots)n+=s.active;return n;}
 void level(int index,double value){if(index>=0&&index<8&&std::isfinite(value))slots[index].level=std::max(0.,value);}
 // The DSP calls finished only when the voice's amplitude release is silent.
 void finished(int index){if(index>=0&&index<8)slots[index]=Slot{};}
 bool setUnisonSize(int part,int size){
  if(part<0||part>=4||(size!=1&&size!=2&&size!=4))return false;
  if(unisonSize[part]==size)return true;
  int begin=0;
  for(int p=0;p<part;++p)begin+=zones[p].voices;
  for(int i=begin;i<begin+zones[part].voices;++i)slots[i]=Slot{};
  unisonSize[part]=size;
  return true;
 }
 int requestedUnison(int part)const{return unisonSize[part];}
 bool pedalDown(int channel)const{return sustain[channel];}
 NoteResult noteOn(int note,int channel,double velocity){
  NoteResult out;
  if(note<0||note>127||channel<0||channel>15||!std::isfinite(velocity))return out;
  if(velocity<=0){noteOff(note,channel);return out;}
  int begin=0;
  for(int part=0;part<4;++part){
   const auto&p=zones[part];const int end=begin+p.voices;
   if(p.voices&&note>=p.low&&note<=p.high&&(p.channel<0||p.channel==channel)){
    const int limit=std::min(unisonSize[part],p.voices);
    const int size=limit>=4?4:limit>=2?2:1;
    // A partially released stack still owns its group of slots.
    int chosen=-1,priority=4;
    double quietest=0;
    std::uint64_t oldest=0;
    for(int first=begin;first+size<=end;first+=size){
     bool occupied=false,held=false,same=false;
     double level=0;
     std::uint64_t age=0;
     for(int i=first;i<first+size;++i)if(const auto&slot=slots[i];slot.active){
      occupied=true;held|=slot.gate;same|=slot.note==note&&slot.channel==channel;
      level=std::max(level,slot.level);age=std::max(age,slot.age);
     }
     const int rank=same?0:!occupied?1:!held?2:3;
     if(rank<priority||(rank==2&&priority==2&&level<quietest)||(rank==3&&priority==3&&age<oldest)){
      chosen=first;priority=rank;quietest=level;oldest=age;
      if(rank==0)break;
     }
    }
    const auto age=++clock;
    for(int member=0;member<size;++member){
     const int i=chosen+member;
     const bool replacing=slots[i].active;
     slots[i]={true,true,true,part,note,channel,age,0};
     out.assignments[out.count++]={i,part,note,channel,replacing,size,member};
    }
   }
   begin=end;
  }
  return out;
 }
 void noteOff(int note,int channel){
  if(note<0||note>127||channel<0||channel>15)return;
  for(auto&s:slots)if(s.active&&s.note==note&&s.channel==channel&&s.keyDown){
   s.keyDown=false;if(!sustain[channel])s.gate=false;
  }
 }
 void pedal(int channel,bool down){
  if(channel<0||channel>15)return;sustain[channel]=down;
  if(!down)for(auto&s:slots)if(s.active&&s.channel==channel&&!s.keyDown)s.gate=false;
 }
 void allNotesOff(int channel=-1){
  if(channel< -1||channel>15)return;
  if(channel<0)sustain={};else sustain[channel]=false;
  for(auto&s:slots)if(s.active&&(channel<0||s.channel==channel))s.keyDown=s.gate=false;
 }
 void allSoundOff(int channel=-1){
  if(channel< -1||channel>15)return;
  if(channel<0){clear();return;}
  sustain[channel]=false;
  for(auto&s:slots)if(s.active&&s.channel==channel)s=Slot{};
 }
};
}

