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
template<class T>void put(std::ofstream& f,T v){f.write(reinterpret_cast<const char*>(&v),sizeof(v));}
void wav(const char* path,const std::vector<float>&audio){
 std::ofstream f(path,std::ios::binary);require(bool(f),"open WAV");
 f.write("RIFF",4);put<std::uint32_t>(f,36+std::uint32_t(audio.size()*4));f.write("WAVEfmt ",8);put<std::uint32_t>(f,16);
 put<std::uint16_t>(f,3);put<std::uint16_t>(f,1);put<std::uint32_t>(f,48000);put<std::uint32_t>(f,192000);put<std::uint16_t>(f,4);put<std::uint16_t>(f,32);
 f.write("data",4);put<std::uint32_t>(f,std::uint32_t(audio.size()*4));f.write(reinterpret_cast<const char*>(audio.data()),audio.size()*4);require(bool(f),"write WAV");
}
int main(int argc,char**argv){try{
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
