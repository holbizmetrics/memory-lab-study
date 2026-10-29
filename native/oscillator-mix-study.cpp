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
struct Setting{const char*name;double saw,pulse,interval;};
const Setting settings[]{
 {"original-balance",.42,.28,0},{"saw-only",.42,0,0},
 {"pulse-only",0,.28,0},{"octave-down",.42,.28,-12},
 {"fifth-up",.42,.28,7},{"octave-up",.42,.28,12}
};
std::vector<float> render(double sr,bool chord,const Setting&s,int block){
 auto synth=std::make_unique<Synth>();require(synth->prepare(sr),"prepare");
 require(synth->setStudyOscillators(s.saw,s.pulse,s.interval),"configure");
 require(!synth->setStudyOscillators(-1,0,0),"negative mixer accepted");
 require(!synth->setStudyOscillators(0,0,std::numeric_limits<double>::quiet_NaN()),"nonfinite interval accepted");
 std::vector<Event>events{{0,EventKind::Parameter,0,0,0,Param::Drive},{0,EventKind::Parameter,0,0,0,Param::Depth},{0,EventKind::Parameter,0,1.2,0,Param::Q}};
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
int main(int argc,char**argv){try{
 // Check the audible fundamental of oscillator 2 in isolation, not just the
 // requested parameter. The comparison frequencies sit 10 Hz to either side.
 for(double interval:{-12.,0.,7.,12.}){
  const auto a=render(48000,false,{"pitch-check",0,.28,interval},512);
  const double expected=440*std::exp2((52-69+interval)/12.+7./1200);
  auto magnitude=[&](double hz){double re=0,im=0;for(int i=67200;i<86400;++i){const double angle=2*pi*hz*i/48000.;re+=a[i]*std::cos(angle);im+=a[i]*std::sin(angle);}return re*re+im*im;};
  require(magnitude(expected)>4*std::max(magnitude(expected-10),magnitude(expected+10)),"oscillator 2 interval fundamental missing");
 }
 for(double sr:{44100.,48000.,96000.})for(bool chord:{false,true}){
  const auto baseline=render(sr,chord,settings[0],512);
  for(const auto&s:settings){const auto whole=render(sr,chord,s,512),split=render(sr,chord,s,127);require(whole==split,"block partition mismatch");if(&s!=&settings[0])require(whole!=baseline,"setting has no effect");}
  const auto silent=render(sr,chord,{"silent",0,0,0},127);for(float x:silent)require(x==0,"muted oscillators not silent");
 }
 std::cout<<"PASS: 36 oscillator/fixture/rate cases, exact block parity, finite audio, release, mute and invalid configuration rejection\n";
 require(argc==2,"Expected --test or new output folder");if(std::string(argv[1])=="--test")return 0;
 const fs::path folder(argv[1]);require(fs::create_directory(folder),"output folder exists or parent missing");
 std::ofstream html(folder/"index.html"),report(folder/"measurements.csv");
 html<<"<!doctype html><meta charset='utf-8'><title>Oscillator mix and intervals</title><style>body{font:18px system-ui;background:#101819;color:#e5eee8;max-width:850px;margin:40px auto;padding:16px}section{padding:20px;background:#1a2729;margin:16px 0}audio{width:100%}</style><h1>Oscillator mix and intervals</h1><p>Oscillator 1: saw. Oscillator 2: pulse, retaining +7 cents detune. Interval shifts oscillator 2 only. Same low-pass sweep and notes, drive stages bypassed. Listening files matched to -24 dBFS RMS; raw renders retained. No autoplay.</p>";
 report<<"fixture,setting,saw_level,pulse_level,osc2_semitones,raw_rms,raw_peak,playback_gain\n";
 for(bool chord:{false,true})for(const auto&s:settings){auto a=render(48000,chord,s,512);const std::string name=std::string(chord?"chord-":"single-")+s.name;wav(folder/(name+"-raw.wav"),a);double energy=0,peak=0;for(float x:a){energy+=double(x)*x;peak=std::max(peak,std::abs(double(x)));}const double rms=std::sqrt(energy/a.size()),gain=std::pow(10.,-24./20)/rms;require(peak*gain<.95,"matched headroom");for(auto&x:a)x=float(x*gain);wav(folder/(name+".wav"),a);
  report<<(chord?"chord":"single")<<','<<s.name<<','<<s.saw<<','<<s.pulse<<','<<s.interval<<','<<rms<<','<<peak<<','<<gain<<'\n';html<<"<section><h2>"<<(chord?"E minor chord":"Single E3")<<" / "<<s.name<<"</h2><audio controls preload='none' src='"<<name<<".wav'></audio></section>";
 }
 html<<"<script>document.addEventListener('play',e=>{document.querySelectorAll('audio').forEach(a=>{if(a!==e.target)a.pause()})},true)</script>";
 require(bool(html)&&bool(report),"report write");std::cout<<"Wrote 12 matched clips, 12 raw WAVs, measurements and player: "<<folder<<'\n';return 0;
}catch(const std::exception&e){std::cerr<<"FAIL: "<<e.what()<<'\n';return 1;}}
