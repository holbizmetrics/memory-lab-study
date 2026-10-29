#include "voice.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>
#include <memory>
#include <stdexcept>
#include <limits>
using namespace memorylab;
namespace fs=std::filesystem;
void require(bool ok,const char*why){if(!ok)throw std::runtime_error(why);}
struct Setting{const char*name;int family;};
const Setting settings[]={{"classic-lp2",0},{"steeper-lp4",1},{"bandpass-bp4",2},{"blend-xf4",3}};
std::vector<float> render(double sr,bool chord,const Setting&s,int block,bool driven=false){
 auto synth=std::make_unique<Synth>();require(synth->prepare(sr),"prepare");
 require(synth->setStudyFilter(s.family),"filter configuration");require(!synth->setStudyFilter(-1)&&!synth->setStudyFilter(4),"invalid filter family accepted");
 std::vector<Event>events{{0,EventKind::Parameter,0,driven?5.:0.,0,Param::Drive},{0,EventKind::Parameter,0,driven?.8:0.,0,Param::Depth},{0,EventKind::Parameter,0,1.2,0,Param::Q}};
 for(int note:chord?std::vector<int>{52,55,59}:std::vector<int>{52}){
  events.push_back({std::size_t(sr*.1),EventKind::NoteOn,note,.8});events.push_back({std::size_t(sr*1.9),EventKind::NoteOff,note});
 }
 for(int i=0;i<=80;++i)events.push_back({std::size_t(sr*(.1+1.7*i/80.)),EventKind::Parameter,0,300*std::pow(20.,i/80.),0,Param::Cutoff});
 std::stable_sort(events.begin(),events.end(),[](auto&a,auto&b){return a.offset<b.offset;});
 std::vector<float>audio(std::size_t(sr*2.7));std::size_t pos=0,index=0;
 while(pos<audio.size()){
  const auto count=std::min<std::size_t>(block,audio.size()-pos);std::array<Event,128>local{};std::size_t used=0;
  while(index<events.size()&&events[index].offset<pos+count){local[used]=events[index++];local[used++].offset-=pos;}
  require(synth->render(audio.data()+pos,count,local.data(),used),"render");pos+=count;
 }
 for(float x:audio)require(std::isfinite(x)&&std::abs(x)<2,"invalid audio");
 require(synth->activeVoices()==0,"stuck voices");return audio;
}
template<class T>void put(std::ofstream&f,T x){f.write(reinterpret_cast<const char*>(&x),sizeof(x));}
void wav(const fs::path&path,const std::vector<float>&a){
 std::ofstream f(path,std::ios::binary);require(bool(f),"open WAV");f.write("RIFF",4);put<std::uint32_t>(f,36+std::uint32_t(a.size()*4));f.write("WAVEfmt ",8);put<std::uint32_t>(f,16);put<std::uint16_t>(f,3);put<std::uint16_t>(f,1);put<std::uint32_t>(f,48000);put<std::uint32_t>(f,192000);put<std::uint16_t>(f,4);put<std::uint16_t>(f,32);f.write("data",4);put<std::uint32_t>(f,std::uint32_t(a.size()*4));f.write(reinterpret_cast<const char*>(a.data()),a.size()*4);require(bool(f),"write WAV");
}
#include "filter-family-checks.hpp"
int main(int argc,char**argv){try{
 filterChecks();
 for(double sr:{44100.,48000.,96000.})for(bool chord:{false,true}){
  const auto baseline=render(sr,chord,settings[0],512);
  for(const auto&s:settings){const auto whole=render(sr,chord,s,512),split=render(sr,chord,s,127);require(whole==split,"block partition mismatch");if(&s!=&settings[0])require(whole!=baseline,"setting has no effect");}

 }
 for(double sr:{44100.,48000.,96000.})for(const auto&s:settings)require(render(sr,true,s,512,true)==render(sr,true,s,127,true),"driven chord block mismatch");
 std::cout<<"PASS: 36 filter/fixture/drive/rate cases, exact block parity, finite audio, release, invalid configuration rejection\n";
 require(argc==2,"Expected --test or new output folder");if(std::string(argv[1])=="--test")return 0;
 const fs::path folder(argv[1]);require(fs::create_directory(folder),"output folder exists or parent missing");
 std::ofstream html(folder/"index.html"),report(folder/"measurements.csv");
 html<<"<!doctype html><meta charset='utf-8'><title>Filter family study</title><style>body{font:18px system-ui;background:#101819;color:#e5eee8;max-width:850px;margin:40px auto;padding:16px}section{padding:20px;background:#1a2729;margin:16px 0}audio{width:100%}</style><h1>Filter family study</h1><p>Same original saw/pulse mix, envelope, notes and cutoff sweep. Compare classic LP2, cascaded LP4, normalized BP4, and a 50/50 BP4 + LP2 blend. The first eight clips bypass both drive stages; the last four use shared drive 5, memory .8. Per-voice drive and noise remain off. This is an original filter experiment, not a measured hardware model. Listening files matched to -24 dBFS RMS; raw renders retained. No autoplay.</p>";
 report<<"fixture,setting,family,raw_rms,raw_peak,playback_gain\n";
 for(int context=0;context<3;++context)for(const auto&s:settings){const bool chord=context!=0,driven=context==2;auto a=render(48000,chord,s,512,driven);const std::string name=std::string(driven?"chord-shared-":chord?"chord-":"single-")+s.name;wav(folder/(name+"-raw.wav"),a);double energy=0,peak=0;for(float x:a){energy+=double(x)*x;peak=std::max(peak,std::abs(double(x)));}const double rms=std::sqrt(energy/a.size()),gain=std::pow(10.,-24./20)/rms;require(peak*gain<.95,"matched headroom");for(auto&x:a)x=float(x*gain);wav(folder/(name+".wav"),a);
  report<<(chord?"chord":"single")<<','<<s.name<<','<<s.family<<','<<rms<<','<<peak<<','<<gain<<'\n';html<<"<section><h2>"<<(driven?"E minor chord / shared drive 5, memory .8":chord?"E minor chord / drives bypassed":"Single E3 / drives bypassed")<<" / "<<s.name<<"</h2><audio controls preload='none' src='"<<name<<".wav'></audio></section>";
 }
 html<<"<script>document.addEventListener('play',e=>{document.querySelectorAll('audio').forEach(a=>{if(a!==e.target)a.pause()})},true)</script>";
 require(bool(html)&&bool(report),"report write");std::cout<<"Wrote 12 matched clips, 12 raw WAVs, measurements and player: "<<folder<<'\n';return 0;
}catch(const std::exception&e){std::cerr<<"FAIL: "<<e.what()<<'\n';return 1;}}
