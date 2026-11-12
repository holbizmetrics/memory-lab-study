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
 struct Assignment {int slot=0,part=0,note=0,channel=0;bool replacing=false;};
 struct NoteResult {std::array<Assignment,4> assignments{};int count=0;};
 using Layout=std::array<PartZone,4>;
private:
 Layout zones{{{8,0,127,-1},{},{},{}}};
 std::array<Slot,8> slots{};
 std::array<bool,16> sustain{};
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
 NoteResult noteOn(int note,int channel,double velocity){
  NoteResult out;
  if(note<0||note>127||channel<0||channel>15||!std::isfinite(velocity))return out;
  if(velocity<=0){noteOff(note,channel);return out;}
  int begin=0;
  for(int part=0;part<4;++part){
   const auto&p=zones[part];const int end=begin+p.voices;
   if(p.voices&&note>=p.low&&note<=p.high&&(p.channel<0||p.channel==channel)){
    int chosen=-1;
    for(int i=begin;i<end;++i)if(slots[i].active&&slots[i].note==note&&slots[i].channel==channel){chosen=i;break;}
    if(chosen<0)for(int i=begin;i<end;++i)if(!slots[i].active){chosen=i;break;}
    if(chosen<0)for(int i=begin;i<end;++i)if(!slots[i].gate&&(chosen<0||slots[i].level<slots[chosen].level)){chosen=i;}
    if(chosen<0){chosen=begin;for(int i=begin+1;i<end;++i)if(slots[i].age<slots[chosen].age)chosen=i;}
    const bool replacing=slots[chosen].active;
    slots[chosen]={true,true,true,part,note,channel,++clock,0};
    out.assignments[out.count++]={chosen,part,note,channel,replacing};
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

