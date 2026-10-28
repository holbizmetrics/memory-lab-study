#include "voice.hpp"
#include <fstream>
#include <iostream>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <new>
#include <string>
#include <stdexcept>
static bool watchAlloc=false;static std::size_t allocations=0;
void* operator new(std::size_t n){if(watchAlloc)++allocations;if(auto p=std::malloc(n?n:1))return p;throw std::bad_alloc();}
void operator delete(void*p)noexcept{std::free(p);}
void operator delete(void*p,std::size_t)noexcept{std::free(p);}
using namespace memorylab;
void require(bool ok,const char* why){if(!ok)throw std::runtime_error(why);}
std::vector<Event> phrase(double sr){
 const double rows[][4]={{0,48,.32,.45},{.52,55,.3,.65},{1.04,60,.42,.9},{1.65,63,.25,.55},{2.08,67,.65,1},{3,60,1.2,.82},{4.5,48,1.2,1}};
 std::vector<Event> e;
 for(auto&r:rows){e.push_back({std::size_t(std::llround(r[0]*sr)),EventKind::NoteOn,int(r[1]),r[3]});e.push_back({std::size_t(std::llround((r[0]+r[2])*sr)),EventKind::NoteOff,int(r[1])});}
 std::stable_sort(e.begin(),e.end(),[](const Event&a,const Event&b){return a.offset<b.offset;});return e;
}
std::vector<Event> motion(double sr,int scenario){
 std::vector<Event> events{{0,EventKind::NoteOn,60,.85}};
 for(int i=0;i<3;++i){
  const std::size_t offset=std::size_t((.5+i*.5)*sr);
  if(scenario!=1)events.push_back({offset,EventKind::Parameter,0,i==0?.1:i==1?.9:.43,0,Param::PulseWidth});
  if(scenario!=0)events.push_back({offset,EventKind::Parameter,0,i==0?-30.:i==1?30.:7.,0,Param::Detune});
 }
 events.push_back({std::size_t(sr*2),EventKind::NoteOff,60});return events;
}
void tests(){
 for(double sr:{44100.,48000.,96000.}){
  Synth s;require(s.prepare(sr),"prepare");auto events=phrase(sr);const auto n=std::size_t(sr*6.8);
  std::vector<float>a(n),b(n);watchAlloc=true;require(s.render(a.data(),n,events.data(),events.size()),"render");watchAlloc=false;
  require(allocations==0,"render allocated");
  for(float v:a)require(std::isfinite(v)&&std::abs(v)<1,"nonfinite or excessive output");
  s.reset();s.render(b.data(),n,events.data(),events.size());require(a==b,"reset reproducibility");
  s.reset();std::size_t pos=0,j=0;
  while(pos<n){auto length=std::min<std::size_t>(127,n-pos);std::array<Event,32> local{};std::size_t count=0;
   while(j<events.size()&&events[j].offset<pos+length){local[count]=events[j++];local[count++].offset-=pos;}
   require(s.render(b.data()+pos,length,local.data(),count),"partition render");pos+=length;}
  require(a==b,"block partition invariance");require(s.activeVoices()==0,"release completion");
  std::array<Event,9> chord{};for(int i=0;i<9;++i)chord[i]={0,EventKind::NoteOn,60+i,.8};
  s.reset();s.render(b.data(),128,chord.data(),chord.size());require(s.activeVoices()==8,"voice capacity");
  Event invalid{129,EventKind::NoteOn,60,1};require(!s.render(b.data(),128,&invalid,1),"invalid offset accepted");
  require(s.activeVoices()==8,"invalid event mutated state");
  Event off{0,EventKind::AllOff};s.render(b.data(),std::size_t(sr),&off,1);require(!s.activeVoices(),"all off");
  s.reset();Event channels[]={{0,EventKind::NoteOn,60,.8,0},{0,EventKind::NoteOn,60,.8,1}};
  s.render(b.data(),128,channels,2);Event oneOff{0,EventKind::NoteOff,60,0,0};
  s.render(b.data(),std::size_t(sr),&oneOff,1);require(s.activeVoices()==1,"channel ownership");
  s.reset();Event repeat[]={{0,EventKind::NoteOn,60,.8},{1,EventKind::NoteOn,60,.9}};
  s.render(b.data(),128,repeat,2);require(s.activeVoices()==1,"repeated-note policy");
  require(!s.render(b.data(),128,nullptr,1),"null events accepted");
  require(std::all_of(b.begin(),b.begin()+128,[](float x){return x==0;}),"invalid block not silent");
 }
 Synth s;require(!s.prepare(0)&&!s.prepare(NAN),"invalid sample rate");
 std::cout<<"PASS: three sample rates, reset, block partitioning, finite/headroom, eight-voice cap, invalid-event atomicity, all-off, no render new allocations\n";
}
void motionTests(){
 OscillatorBank bank;bank.prepare();double worstBoundary=0,worstWidth=0;
 // Compare both sides at fixed phase: removes the normal waveform time change.
 for(int level=1;level<=9;++level)for(int i=0;i<257;++i){
  const double phase=(i+.37)/257,dt=.45/(1<<level);
  worstBoundary=std::max(worstBoundary,std::abs(bank.saw(phase,dt*(1-1e-9))-bank.saw(phase,dt*(1+1e-9))));
  worstWidth=std::max(worstWidth,std::abs(bank.pulse(phase,dt,.43-1e-9)-bank.pulse(phase,dt,.43+1e-9)));
 }
 require(worstBoundary<1e-6,"bank boundary discontinuity");require(worstWidth<1e-5,"width discontinuity");
 for(double sr:{44100.,48000.,96000.})for(int scenario=0;scenario<3;++scenario){
  Synth synth;synth.prepare(sr);const std::size_t n=std::size_t(sr*3);auto events=motion(sr,scenario);
  std::vector<float>full(n),split(n);watchAlloc=true;require(synth.render(full.data(),n,events.data(),events.size()),"motion render");watchAlloc=false;require(allocations==0,"motion allocation");
  for(float x:full)require(std::isfinite(x)&&std::abs(x)<1,"motion headroom");
  for(std::size_t block:{17u,127u,512u}){
   synth.reset();std::size_t j=0;
   for(std::size_t pos=0;pos<n;pos+=block){const auto size=std::min(block,n-pos);std::array<Event,16>local{};std::size_t count=0;
    while(j<events.size()&&events[j].offset<pos+size){local[count]=events[j++];local[count++].offset-=pos;}
    require(synth.render(split.data()+pos,size,local.data(),count),"motion block");
   }require(full==split,"motion block partition mismatch");
  }
  require(synth.activeVoices()==0,"motion stuck note");
 }
 std::cout<<"PASS motion: 9 rate/scenario cases, 27 partition comparisons, no render new allocations; bank boundary "<<worstBoundary<<", width continuity "<<worstWidth<<"\n";
}
void profileTests(){
 double peak=0;
 for(double sr:{44100.,48000.,96000.}){
  ClassicFilter filter;double cutoff=1450,q=2.4,profile=1;const double smooth=1-std::exp(-1/(sr*.02));
  for(int i=0;i<int(sr*3);++i){const int step=int(i/(sr*.1))%2;
   cutoff+=((step?sr*.4:30)-cutoff)*smooth;q+=((step?8.:.5)-q)*smooth;profile+=((step?1.:0)-profile)*smooth;
   const double input=.15*std::sin(2*pi*215*i/sr)+.05*std::sin(2*pi*4317*i/sr);
   const double y=filter.tick(input,sr,cutoff,q,profile);require(std::isfinite(y)&&std::abs(y)<30,"filter automation bound");peak=std::max(peak,std::abs(y));
  }
  std::vector<Event> events{{0,EventKind::NoteOn,60,.85},{std::size_t(sr*.5),EventKind::Parameter,0,0,0,Param::Profile},
   {std::size_t(sr),EventKind::Parameter,0,.5,0,Param::Profile},{std::size_t(sr*1.5),EventKind::Parameter,0,1,0,Param::Profile},{std::size_t(sr*2),EventKind::NoteOff,60}};
  const std::size_t n=std::size_t(sr*3);Synth synth;synth.prepare(sr);std::vector<float>a(n),b(n);
  watchAlloc=true;synth.render(a.data(),n,events.data(),events.size());watchAlloc=false;require(allocations==0,"profile render allocation");
  for(std::size_t block:{17u,127u,512u}){synth.reset();std::size_t j=0;
   for(std::size_t pos=0;pos<n;pos+=block){auto size=std::min(block,n-pos);std::array<Event,8>local{};std::size_t count=0;
    while(j<events.size()&&events[j].offset<pos+size){local[count]=events[j++];local[count++].offset-=pos;}
    synth.render(b.data()+pos,size,local.data(),count);
   }require(a==b,"profile event partition");
  }require(!synth.activeVoices(),"profile stuck note");
 }
 std::cout<<"PASS profile: three 3-second cutoff/Q/profile stress traces, nine event partition comparisons, no tested render new allocations; peak "<<peak<<"\n";
}
std::vector<Event> modulation(double sr,int mode,bool preRoll=true){
 auto events=phrase(sr);if(preRoll)for(auto&e:events)e.offset+=std::size_t(sr*.1);
 if(mode==1||mode==5)events.insert(events.begin(),{0,EventKind::Parameter,0,1.5,0,Param::FilterEnvelope});
 if(mode==2||mode==5)events.insert(events.begin(),{0,EventKind::Parameter,0,8,0,Param::PitchLfo});
 if(mode==3||mode==5)events.insert(events.begin(),{0,EventKind::Parameter,0,.15,0,Param::PwmLfo});
 if(mode==4||mode==5)events.insert(events.begin(),{0,EventKind::Parameter,0,.5,0,Param::CutoffLfo});
 return events;
}
void modulationTests(){
 for(double sr:{44100.,48000.,96000.}){
  FilterEnvelope env;env.noteOn();double worst=0;const int hold=int(sr*.6);
  for(int i=0;i<hold;++i){const double t=(i+1)/sr,expected=t<.02?t/.02:t<.37?1-.85*(t-.02)/.35:.15;worst=std::max(worst,std::abs(env.tick(sr)-expected));}
  require(worst<1e-9,"filter envelope contour");env.noteOff(sr);
  for(int i=0;i<int(sr*.4);++i){const double expected=std::max(0.,.15*(1-(i+1)/(sr*.4)));require(std::abs(env.tick(sr)-expected)<1e-9,"filter envelope release");}
  for(int mode=0;mode<6;++mode){Synth synth;synth.prepare(sr);const auto n=std::size_t(sr*6.8);auto events=modulation(sr,mode,false);
   std::vector<float>a(n),b(n);watchAlloc=true;synth.render(a.data(),n,events.data(),events.size());watchAlloc=false;require(allocations==0,"mod render allocation");
   for(float x:a)require(std::isfinite(x)&&std::abs(x)<1,"modulation boundedness");
   for(std::size_t block:{17u,127u,512u}){synth.reset();std::size_t j=0;
    for(std::size_t pos=0;pos<n;pos+=block){const auto size=std::min(block,n-pos);std::array<Event,16> local{};std::size_t count=0;
     while(j<events.size()&&events[j].offset<pos+size){local[count]=events[j++];local[count++].offset-=pos;}
     synth.render(b.data()+pos,size,local.data(),count);
    }require(a==b,"modulation partition mismatch");
   }require(!synth.activeVoices(),"modulation stuck note");
  }
 }
 std::cout<<"PASS modulation: envelope contour/release at three rates, 18 mode/rate cases, 54 partition comparisons, no tested render new allocations\n";
}
std::vector<Event> performance(double sr,int mode){
 std::vector<Event> e;
 auto add=[&](double t,EventKind kind,int note,double value=0,int channel=0){e.push_back({std::size_t(std::llround(t*sr)),kind,note,value,channel});};
 if(mode==0){add(0,EventKind::NoteOn,60,.85);add(.25,EventKind::Sustain,0,1);add(.75,EventKind::NoteOff,60);add(2.5,EventKind::Sustain,0,0);}
 if(mode==1||mode==2){add(0,EventKind::NoteOn,60,.85);add(3.2,EventKind::NoteOff,60);
  if(mode==2)e.push_back({0,EventKind::Parameter,0,1,0,Param::PressureDepth});
  const auto kind=mode==1?EventKind::PitchBend:EventKind::ChannelPressure;
  add(.5,kind,0,1);add(1.5,kind,0,mode==1?-1:0);add(2.5,kind,0,mode==1?0:1);
 }
 if(mode==3){const int notes[]={48,52,55,59,60,64,67,71,72};for(int i=0;i<9;++i)add(i*.16,EventKind::NoteOn,notes[i],.75);
  add(1.6,EventKind::NoteOn,67,.9);add(2,EventKind::NoteOff,67);add(2.1,EventKind::NoteOff,48);add(2.5,EventKind::AllOff,0);
 }
 std::stable_sort(e.begin(),e.end(),[](const Event&a,const Event&b){return a.offset<b.offset;});return e;
}
void performanceTests(){
 for(double sr:{44100.,48000.,96000.}){
  Synth synth;synth.prepare(sr);std::vector<float>a(std::size_t(sr*4)),b(a.size());
  Event start[]={{0,EventKind::NoteOn,60,.8,0},{0,EventKind::NoteOn,60,.8,1},{0,EventKind::Sustain,0,1,0},
   {std::size_t(sr*.2),EventKind::NoteOff,60,0,0},{std::size_t(sr*.2),EventKind::NoteOff,60,0,1}};
  synth.render(a.data(),std::size_t(sr),start,5);require(synth.activeVoices()==1&&synth.heldVoices()==0&&synth.sounding(60,0),"sustain channel ownership");
  Event up{0,EventKind::Sustain,0,0,0};synth.render(a.data(),std::size_t(sr),&up,1);require(!synth.activeVoices(),"pedal release stuck note");
  synth.reset();std::array<Event,9>chord{};for(int i=0;i<9;++i)chord[i]={0,EventKind::NoteOn,48+i,.8};
  synth.render(a.data(),128,chord.data(),chord.size());require(synth.activeVoices()==8&&!synth.sounding(48,0)&&synth.sounding(56,0),"oldest held stealing");
  Event stale{0,EventKind::NoteOff,48};synth.render(a.data(),128,&stale,1);require(synth.heldVoices()==8,"stale stolen note-off affected voice");
  Event panic{0,EventKind::AllSoundOff};synth.render(a.data(),128,&panic,1);require(!synth.activeVoices()&&std::all_of(a.begin(),a.begin()+128,[](float v){return v==0;}),"panic not silent");
  synth.reset();Event single{0,EventKind::NoteOn,60,.8,1};synth.render(a.data(),std::size_t(sr),&single,1);
  synth.reset();Event isolated[]={{0,EventKind::NoteOn,60,.8,1},{100,EventKind::PitchBend,0,1,0},{100,EventKind::ChannelPressure,0,1,0},{100,EventKind::Sustain,0,1,0}};
  synth.render(b.data(),std::size_t(sr),isolated,4);require(std::equal(a.begin(),a.begin()+int(sr),b.begin()),"controller leaked across channels");
  for(int mode=0;mode<4;++mode){auto events=performance(sr,mode);synth.reset();watchAlloc=true;synth.render(a.data(),a.size(),events.data(),events.size());watchAlloc=false;
   require(allocations==0,"performance allocation");for(float value:a)require(std::isfinite(value)&&std::abs(value)<1,"performance boundedness");
   require(!synth.activeVoices()&&!synth.heldVoices(),"performance stuck voices");
   for(std::size_t block:{17u,127u,512u}){synth.reset();std::size_t j=0;
    for(std::size_t pos=0;pos<a.size();pos+=block){const auto size=std::min(block,a.size()-pos);std::array<Event,24>local{};std::size_t count=0;
     while(j<events.size()&&events[j].offset<pos+size){local[count]=events[j++];local[count++].offset-=pos;}
     synth.render(b.data()+pos,size,local.data(),count);
    }require(a==b,"performance event partition");
   }
  }
 }
 std::cout<<"PASS performance: sustain/channel ownership, pedal release, stale note-offs, panic silence, channel isolation; 12 mode/rate cases, 36 partition comparisons, no tested render new allocations\n";
}
template<class T>void put(std::ofstream& f,T v){f.write(reinterpret_cast<const char*>(&v),sizeof(v));}
void wav(const char* path,const std::vector<float>&audio){
 std::ofstream f(path,std::ios::binary);require(bool(f),"open WAV");
 f.write("RIFF",4);put<std::uint32_t>(f,36+std::uint32_t(audio.size()*4));f.write("WAVEfmt ",8);put<std::uint32_t>(f,16);
 put<std::uint16_t>(f,3);put<std::uint16_t>(f,1);put<std::uint32_t>(f,48000);put<std::uint32_t>(f,192000);put<std::uint16_t>(f,4);put<std::uint16_t>(f,32);
 f.write("data",4);put<std::uint32_t>(f,std::uint32_t(audio.size()*4));f.write(reinterpret_cast<const char*>(audio.data()),audio.size()*4);require(bool(f),"write WAV");
}
int main(int argc,char**argv){try{
 if(argc==2&&std::strcmp(argv[1],"--performance-test")==0){performanceTests();return 0;}
 if(argc==4&&std::strcmp(argv[1],"--performance-voice")==0){const int mode=std::stoi(argv[3]);require(mode>=0&&mode<=3,"performance mode");
  Synth synth;synth.prepare(48000);auto events=performance(48000,mode);std::vector<float>audio(192000);synth.render(audio.data(),audio.size(),events.data(),events.size());
  std::ofstream f(argv[2],std::ios::binary);require(bool(f),"performance open");f.write(reinterpret_cast<const char*>(audio.data()),audio.size()*sizeof(float));require(bool(f),"performance write");return 0;}
 if(argc==2&&std::strcmp(argv[1],"--mod-test")==0){modulationTests();return 0;}
 if(argc==4&&std::strcmp(argv[1],"--mod-voice")==0){const int mode=std::stoi(argv[3]);require(mode>=0&&mode<=5,"mod mode");
  Synth synth;synth.prepare(48000);auto events=modulation(48000,mode);std::vector<float>audio(331200);synth.render(audio.data(),audio.size(),events.data(),events.size());
  std::ofstream f(argv[2],std::ios::binary);require(bool(f),"mod open");f.write(reinterpret_cast<const char*>(audio.data()+4800),326400*sizeof(float));require(bool(f),"mod write");return 0;}
 if(argc==2&&std::strcmp(argv[1],"--profile-test")==0){profileTests();return 0;}
 if(argc==7&&std::strcmp(argv[1],"--filter-impulse")==0){
  const double sr=std::stod(argv[3]),cutoff=std::stod(argv[4]),q=std::stod(argv[5]),profile=std::stod(argv[6]);
  require(sr>=44100&&sr<=96000&&cutoff>=30&&cutoff<=sr*.4&&q>=.5&&q<=8&&profile>=0&&profile<=1,"filter range");
  ClassicFilter filter;std::ofstream file(argv[2],std::ios::binary);require(bool(file),"filter open");
  for(int i=0;i<int(sr*2);++i){const double y=filter.tick(i==0?1.:0.,sr,cutoff,q,profile);file.write(reinterpret_cast<const char*>(&y),sizeof(y));}
  require(bool(file),"filter write");return 0;
 }
 if(argc==4&&std::strcmp(argv[1],"--profile-voice")==0){
  const int mode=std::stoi(argv[3]);require(mode>=0&&mode<=3,"profile mode");const double sr=48000;
  auto events=phrase(sr);const double profile=mode==0||mode==3?0:mode==1?.5:1;
  // Pre-roll lets the shared 20ms control smoother settle before identical notes.
  for(auto&e:events)e.offset+=4800;
  events.insert(events.begin(),{0,EventKind::Parameter,0,profile,0,Param::Profile});
  if(mode==3)for(int i=0;i<=200;++i)events.push_back({std::size_t(4800+i*48000*.03),EventKind::Parameter,0,i/200.,0,Param::Profile});
  std::stable_sort(events.begin(),events.end(),[](const Event&a,const Event&b){return a.offset<b.offset;});
  Synth synth;synth.prepare(sr);std::vector<float>audio(331200);synth.render(audio.data(),audio.size(),events.data(),events.size());
  std::ofstream f(argv[2],std::ios::binary);require(bool(f),"profile voice open");f.write(reinterpret_cast<const char*>(audio.data()+4800),326400*sizeof(float));require(bool(f),"profile voice write");return 0;
 }
 if(argc==2&&std::strcmp(argv[1],"--motion-test")==0){motionTests();return 0;}
 if(argc==5&&std::strcmp(argv[1],"--motion-voice")==0){const double sr=std::stod(argv[3]);const int scenario=std::stoi(argv[4]);
  require(sr==44100||sr==48000||sr==96000,"motion rate");require(scenario>=0&&scenario<=2,"motion scenario");Synth synth;synth.prepare(sr);
  auto events=motion(sr,scenario);std::vector<float>audio(std::size_t(sr*3));synth.render(audio.data(),audio.size(),events.data(),events.size());
  std::ofstream f(argv[2],std::ios::binary);require(bool(f),"motion open");f.write(reinterpret_cast<const char*>(audio.data()),audio.size()*sizeof(float));require(bool(f),"motion write");return 0;}
 if(argc==4&&std::strcmp(argv[1],"--bank-motion")==0){const double sr=std::stod(argv[3]);require(sr==44100||sr==48000||sr==96000,"motion rate");
  OscillatorBank bank;bank.prepare();MemoryDrive drive;drive.prepare(sr);Parameters p;
  double phase=0,phase2=.17,detune=7,width=.43;const double smooth=1-std::exp(-1/(sr*.02));
  std::ofstream file(argv[2],std::ios::binary);require(bool(file),"bank motion open");
  for(int i=0;i<int(sr*2);++i){const double t=i/sr;
   const double targetD=t<.5?7:t<1?-30:t<1.5?30:7,targetW=t<.5?.43:t<1?.1:t<1.5?.9:.43;
   detune+=(targetD-detune)*smooth;width+=(targetW-width)*smooth;
   const double dt=.45/16*std::exp2(detune/1200),a=bank.saw(phase,dt),b=bank.pulse(phase2,dt,width),mix=.42*a+.28*b;
   const double row[]={a,b,phase,phase2,dt,width,mix,drive.tick(mix,p)};
   file.write(reinterpret_cast<const char*>(row),sizeof(row));phase+=dt;phase-=std::floor(phase);phase2+=dt;phase2-=std::floor(phase2);
  }require(bool(file),"bank motion write");return 0;}
 if(argc==6&&(std::strcmp(argv[1],"--osc-fixture")==0||std::strcmp(argv[1],"--table-fixture")==0)){
  const double sr=std::stod(argv[3]),note=std::stod(argv[4]),width=std::stod(argv[5]);
  require(sr>=44100&&sr<=96000&&note>=24&&note<=108&&width>=.1&&width<=.9,"fixture range");
  double phase1=0,phase2=.17;const double f=440*std::exp2((note-69)/12.),dt=f/sr,dt2=f*std::exp2(7./1200)/sr;
  const bool table=std::strcmp(argv[1],"--table-fixture")==0;OscillatorBank bank;bank.prepare();
  MemoryDrive drive;drive.prepare(sr);Parameters p;std::ofstream file(argv[2],std::ios::binary);require(bool(file),"osc fixture open");
  for(int i=0;i<int(sr*2);++i){
   const double a=table?bank.saw(phase1,dt):sawSample(phase1,dt),b=table?bank.pulse(phase2,dt2,width):pulseSample(phase2,dt2,width),mix=.42*a+.28*b;
   const float row[]={float(a),float(b),float(mix),float(drive.tick(mix,p))};
   file.write(reinterpret_cast<const char*>(row),sizeof(row));
   phase1+=dt;phase1-=std::floor(phase1);phase2+=dt2;phase2-=std::floor(phase2);
  }require(bool(file),"osc fixture write");return 0;
 }
 if(argc==2&&std::strcmp(argv[1],"--test")==0){tests();return 0;}
 if(argc==3&&std::strcmp(argv[1],"--drive-fixture")==0){MemoryDrive d;d.prepare(48000);Parameters p;std::ofstream f(argv[2],std::ios::binary);require(bool(f),"fixture open");for(int i=0;i<48000;++i){if(i==16000)p.drive=0;if(i==32000){p.drive=12;p.depth=1.5;p.recovery=50;}float input=float(.4*std::sin(i*2*pi*220/48000));float y=float(d.tick(input,p));f.write(reinterpret_cast<char*>(&y),4);}return 0;}
 if(argc!=2){std::cout<<"Usage: memorylab_native --test | output.wav\n";return 1;}
 Synth s;s.prepare(48000);auto e=phrase(48000);std::vector<float>audio(326400);s.render(audio.data(),audio.size(),e.data(),e.size());wav(argv[1],audio);std::cout<<"Rendered native phrase: "<<argv[1]<<"\n";return 0;
 }catch(const std::exception&e){std::cerr<<e.what()<<'\n';return 2;}}
