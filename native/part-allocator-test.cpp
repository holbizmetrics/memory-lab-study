#include "part-allocator.hpp"
#include <iostream>
#include <stdexcept>
#include <limits>
#include <random>
#include <cstdlib>
#include <new>
static bool countAllocations=false;
static std::size_t allocations=0;
void* operator new(std::size_t n){if(countAllocations)++allocations;if(void*p=std::malloc(n?n:1))return p;throw std::bad_alloc();}
void operator delete(void*p)noexcept{std::free(p);}
void operator delete(void*p,std::size_t)noexcept{std::free(p);}
void require(bool p,const char*why){if(!p)throw std::runtime_error(why);}
using memorylab::PartAllocator;
int main(){try{
 PartAllocator a;
 for(int n=60;n<68;++n){auto r=a.noteOn(n,0,1);require(r.count==1&&r.assignments[0].slot==n-60,"default allocation order");}
 require(a.activeCount()==8,"default cap");auto ninth=a.noteOn(72,0,1);
 require(ninth.count==1&&ninth.assignments[0].slot==0&&ninth.assignments[0].replacing,"oldest steal");
 a.noteOff(60,0);require(a.state()[0].keyDown,"stolen old note-off released new note");
 a.noteOff(61,0);a.noteOff(62,0);a.level(1,.8);a.level(2,.1);
 require(a.noteOn(73,0,1).assignments[0].slot==2,"quiet released voice not preferred");
 PartAllocator::Layout split{{{4,0,59,-1},{4,60,127,-1},{},{}}};
 require(a.configure(split)&&a.activeCount()==0,"split configuration");
 require(a.noteOn(59,0,1).assignments[0].part==0,"lower boundary");
 require(a.noteOn(60,0,1).assignments[0].part==1,"upper boundary");
 for(int n=0;n<128;++n){auto r=a.noteOn(n,0,1);require(r.count==1&&r.assignments[0].part==(n<60?0:1),"exhaustive split routing");}
 PartAllocator::Layout layer{{{2,0,127,-1},{2,0,127,-1},{2,0,127,-1},{2,0,127,-1}}};
 require(a.configure(layer),"layer config");
 for(int n=60;n<72;++n){auto r=a.noteOn(n,0,1);require(r.count==4&&a.activeCount()<=8,"layer global cap");for(int p=0;p<4;++p)require(r.assignments[p].slot/2==p,"layer reservation crossed");}
 a.pedal(0,true);a.noteOff(71,0);for(const auto&s:a.state())if(s.note==71)require(!s.keyDown&&s.gate,"layer sustain");
 a.pedal(0,false);for(const auto&s:a.state())if(s.note==71)require(!s.gate,"pedal release");
 a.allSoundOff();require(a.activeCount()==0,"global all sound off");
 auto channels=layer;for(int p=0;p<4;++p)channels[p].channel=p;
 require(a.configure(channels),"channel layout");
 for(int c=0;c<16;++c)for(int note=0;note<128;++note){
  auto r=a.noteOn(note,c,1);require(r.count==(c<4?1:0),"channel isolation");
  if(r.count)require(r.assignments[0].part==c,"wrong channel part");
 }
 a.allNotesOff(0);for(const auto&s:a.state())if(s.active)require(s.channel==0?!s.gate:s.gate,"channel all notes off leaked");
 a.allSoundOff(1);for(const auto&s:a.state())require(!s.active||s.channel!=1,"channel all sound off");
 auto invalid=channels;invalid[3].voices=8;const auto before=a.state();require(!a.configure(invalid),"overbooked layout accepted");
 require(a.activeCount()==6,"invalid layout changed active count");
 for(int i=0;i<8;++i)require(a.state()[i].age==before[i].age&&a.state()[i].active==before[i].active,"invalid layout mutated state");
 invalid=channels;invalid[0].low=128;require(!a.configure(invalid),"bad range accepted");
 invalid=channels;invalid[0].channel=16;require(!a.configure(invalid),"bad channel accepted");
 const auto count=a.activeCount();require(a.noteOn(200,0,1).count==0&&a.noteOn(60,16,1).count==0&&a.noteOn(60,0,std::numeric_limits<double>::quiet_NaN()).count==0&&a.activeCount()==count,"invalid event mutated state");
 require(a.configure(layer),"reset layer");a.noteOn(60,0,1);a.noteOn(60,1,1);a.noteOff(60,0);
 for(const auto&s:a.state())if(s.active)require(s.channel==0?!s.gate:s.gate,"same-note channel release leaked");
 a.allSoundOff();a.noteOn(60,0,1);require(a.noteOn(60,0,0).count==0,"velocity-zero note-on");
 for(const auto&s:a.state())if(s.active)require(!s.gate,"velocity-zero did not release");
 // Deterministic stress of real policy paths, with a fixed global eight-slot invariant.
 std::mt19937 rng(731);countAllocations=true;
 for(int n=0;n<100000;++n){
  const int channel=int(rng()%16),note=int(rng()%128);
  switch(rng()%6){case 0:a.noteOn(note,channel,.8);break;case 1:a.noteOff(note,channel);break;case 2:a.pedal(channel,rng()%2);break;case 3:a.allNotesOff(channel);break;case 4:a.allSoundOff(channel);break;case 5:a.finished(int(rng()%8));break;}
  require(a.activeCount()<=8,"stress exceeded global cap");
 }
 countAllocations=false;require(allocations==0,"allocator allocated during events");
 std::cout<<"PASS four-part allocator policy: eight-slot total, reservations, exhaustive splits/channels, layering, stealing/release priority, sustain, note-off ownership, channel panic, atomic layout rejection, 100000 events without allocations\n";return 0;
}catch(const std::exception&e){countAllocations=false;std::cerr<<"FAIL: "<<e.what()<<'\n';return 1;}}

