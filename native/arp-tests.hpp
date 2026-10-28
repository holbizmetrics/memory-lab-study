#pragma once
#include "arpeggiator.hpp"

void arpeggiatorTests(){
 struct Logged{int sample,kind,note,channel;double velocity;};
 struct Log{std::array<Logged,4096> events{};int count=0,cursor=0;
  void emit(memorylab::EventKind k,int note,double v,int channel){require(count<int(events.size()),"arp log overflow");events[count++]={cursor,int(k),note,channel,v};}
 };
 for(double sr:{44100.,48000.,96000.})for(int mode=0;mode<3;++mode)for(int division=0;division<6;++division){
  memorylab::Arpeggiator whole,split;Log a,b;
  auto ea=[&](auto k,int n,double v,int c){a.emit(k,n,v,c);};auto eb=[&](auto k,int n,double v,int c){b.emit(k,n,v,c);};
  for(int n:{60,64,67}){whole.note(0,n,.8,ea);split.note(0,n,.8,eb);}
  const int length=int(sr*2);
  const auto before=allocations;watchAlloc=true;
  whole.render(length,sr,137.3,division,mode,2,.37,ea,[&](int,int n){a.cursor+=n;});
  for(int p=0;p<length;p+=127)split.render(std::min(127,length-p),sr,137.3,division,mode,2,.37,eb,[&](int,int n){b.cursor+=n;});
  watchAlloc=false;require(allocations==before,"arp render allocation");require(a.count==b.count,"arp partition count");
  const int notes[]{60,64,67,72,76,79};int step=0;double lastStart=0;
  const double interval=sr*60/137.3*memorylab::Arpeggiator::beats(division);
  for(int i=0;i<a.count;++i){const auto&x=a.events[i];const auto&y=b.events[i];
   require(x.sample==y.sample&&x.kind==y.kind&&x.note==y.note&&x.channel==y.channel&&x.velocity==y.velocity,"arp partition events");
   if(x.kind==int(memorylab::EventKind::NoteOn)){
    require(x.note==notes[memorylab::Arpeggiator::index(step,6,mode)],"arp note order");
    require(x.sample==int(std::ceil(step*interval-1e-8)),"arp fractional clock drift");lastStart=x.sample;++step;
   }else require(std::abs(x.sample-(lastStart+interval*.37))<=1,"arp gate timing");
  }
 }
 memorylab::Arpeggiator arp;Log log;auto emit=[&](auto k,int n,double v,int c){log.emit(k,n,v,c);};
 arp.note(0,60,.8,emit);arp.sustain(0,true,emit);arp.note(0,60,0,emit);require(arp.heldCount()==1,"arp sustain lost note");
 arp.render(1,48000,130,2,0,1,.5,emit,[&](int,int n){log.cursor+=n;});
 arp.sustain(0,false,emit);require(arp.heldCount()==0&&log.events[log.count-1].kind==int(memorylab::EventKind::NoteOff),"arp sustain release stuck");
 arp.note(1,127,.7,emit);arp.note(2,60,.6,emit);arp.clearChannel(1,emit);require(arp.heldCount()==1,"arp channel isolation");
 arp.clearChannel(2,emit);arp.note(0,127,.7,emit);const int first=log.count;
 arp.render(48000,48000,130,2,0,3,.95,emit,[&](int,int n){log.cursor+=n;});
 for(int i=first;i<log.count;++i)require(log.events[i].note==127,"arp high note range");
 arp.note(0,127,0,emit);require(arp.heldCount()==0,"arp last note release");
 std::cout<<"PASS arpeggiator: 54 mode/rate/sample-rate cases, exact fractional timing and block partitions, gate timing, sustain, channel isolation, range limits and no tested render allocations\n";
}
