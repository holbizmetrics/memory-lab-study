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
void require(bool ok,const char* message){if(!ok)throw std::runtime_error(message);}

std::vector<Event> fixture(double sr,bool chord){
 std::vector<Event> events{{0,EventKind::Parameter,0,0,0,Param::Drive},{0,EventKind::Parameter,0,0,0,Param::Depth},
  {0,EventKind::Parameter,0,2.72,0,Param::Q},{0,EventKind::Parameter,0,.394,0,Param::AmpSustain},
  {0,EventKind::Parameter,0,80,0,Param::AmpAttack},{0,EventKind::Parameter,0,300,0,Param::AmpRelease}};
 if(chord){for(int n:{52,55,59}){events.push_back({std::size_t(sr*.15),EventKind::NoteOn,n,.8});events.push_back({std::size_t(sr*2.3),EventKind::NoteOff,n});}}
 else{
  events.push_back({0,EventKind::Parameter,0,2,0,Param::AmpAttack});events.push_back({0,EventKind::Parameter,0,160,0,Param::AmpDecay});events.push_back({0,EventKind::Parameter,0,0,0,Param::AmpSustain});events.push_back({0,EventKind::Parameter,0,120,0,Param::AmpRelease});
  for(double t:{.15,.55,.95,1.35,1.75}){events.push_back({std::size_t(sr*t),EventKind::NoteOn,52,.8});events.push_back({std::size_t(sr*(t+.18)),EventKind::NoteOff,52});}
 }
 for(int i=0;i<=100;++i){
  const double t=i/100.;
  events.push_back({std::size_t(sr*(.15+2*t)),EventKind::Parameter,0,120*std::pow(50.,t),0,Param::Cutoff});
 }
 std::stable_sort(events.begin(),events.end(),[](auto&a,auto&b){return a.offset<b.offset;});return events;
}

std::vector<float> render(double sr,bool chord,int mode,int block,int chain=0){
 auto synth=std::make_unique<Synth>();require(synth->prepare(sr),"prepare failed");
#if MEMORYLAB_EXPERIMENTAL_NOISE
 require(synth->setStudyNoise(mode==0?0:mode==1?.08:.35,mode==2),"noise configuration failed");require(!synth->setStudyNoise(std::numeric_limits<double>::quiet_NaN(),false)&&!synth->setStudyNoise(-1,false)&&!synth->setStudyNoise(2,false),"invalid noise level accepted");
#else
 synth->dispatch({0,EventKind::Parameter,0,mode==0?0:mode==1?.08:.35,0,Param::NoiseLevel});
 if(mode==2){synth->dispatch({0,EventKind::Parameter,0,0,0,Param::SawLevel});synth->dispatch({0,EventKind::Parameter,0,0,0,Param::PulseLevel});}
 // Settle the live smoothing before note-on, without advancing any voice RNG.
 std::vector<float>preroll(std::size_t(sr*2));require(synth->render(preroll.data(),preroll.size()),"preroll failed");
#endif
 auto events=fixture(sr,chord);
 if(chain){events[0].value=5;events[1].value=.8;}
 if(chain==2)events.insert(events.begin(),{0,EventKind::Parameter,0,24,0,Param::VoiceDrive});
 std::vector<float> audio(std::size_t(sr*3));
 std::size_t pos=0,eventIndex=0;
 while(pos<audio.size()){
  const auto count=std::min<std::size_t>(block,audio.size()-pos);std::array<Event,128> local{};std::size_t used=0;
  while(eventIndex<events.size()&&events[eventIndex].offset<pos+count){local[used]=events[eventIndex++];local[used++].offset-=pos;}
  require(synth->render(audio.data()+pos,count,local.data(),used),"study render failed");pos+=count;
 }
 for(float x:audio)require(std::isfinite(x)&&std::abs(x)<8,"nonfinite or excessive study output");
 require(synth->activeVoices()==0,"study left active voices");
 
 return audio;
}

template<class T>void put(std::ofstream& f,T value){f.write(reinterpret_cast<const char*>(&value),sizeof(value));}
void wav(const fs::path& path,const std::vector<float>& audio){
 std::ofstream f(path,std::ios::binary);require(bool(f),"open WAV failed");
 f.write("RIFF",4);put<std::uint32_t>(f,36+std::uint32_t(audio.size()*4));f.write("WAVEfmt ",8);put<std::uint32_t>(f,16);
 put<std::uint16_t>(f,3);put<std::uint16_t>(f,1);put<std::uint32_t>(f,48000);put<std::uint32_t>(f,192000);
 put<std::uint16_t>(f,4);put<std::uint16_t>(f,32);f.write("data",4);put<std::uint32_t>(f,std::uint32_t(audio.size()*4));
 f.write(reinterpret_cast<const char*>(audio.data()),audio.size()*sizeof(float));require(bool(f),"write WAV failed");
}

int main(int argc,char**argv){try{
 for(double sr:{44100.,48000.,96000.})for(bool chord:{false,true}){
  const auto baseline=render(sr,chord,0,512);
  for(int mode=0;mode<3;++mode){
   const auto whole=render(sr,chord,mode,512),split=render(sr,chord,mode,127);
   require(whole==split,"noise study changed with block partition");
   if(mode){double difference=0;for(std::size_t i=0;i<whole.size();++i)difference+=std::pow(whole[i]-baseline[i],2);
    require(difference/whole.size()>1e-7,"noise study did not change sound");}
  }
 }
 std::cout<<"PASS: 18 noise/fixture/sample-rate cases finite, release complete, exact 512/127 block agreement; enabled modes differ from bypass.\n";
 if(argc==2&&std::string(argv[1])=="--test")return 0;
 require(argc==2,"Expected --test or a new output folder");const fs::path folder(argv[1]);
 require(fs::create_directory(folder),"Output folder exists or its parent is missing; refusing overwrite");
 std::ofstream html(folder/"index.html"),report(folder/"measurements.csv");
 html<<"<!doctype html><meta charset='utf-8'><title>Per-voice noise study</title><style>body{font:18px system-ui;background:#101819;color:#e5eee8;max-width:850px;margin:40px auto;padding:16px}section{padding:20px;background:#1a2729;margin:16px 0}audio{width:100%}</style><h1>Noise before the filter</h1><p>Same low-pass sweep within each fixture. Shared drive and memory depth are zero. Compare oscillator-only, a little noise mixed in, and noise alone. Each listening clip is matched to -24 dBFS RMS; raw files are retained.</p><p>Deterministic per-voice pseudorandom white noise before the existing amplitude envelope and filter. Existing oscillator balance is retained except in noise-solo clips. No hardware noise-circuit model. No autoplay.</p>";
 report<<"fixture,mode,raw_rms,raw_peak,playback_gain,playback_peak\n";
 for(bool chord:{false,true})for(int mode=0;mode<3;++mode){
  auto audio=render(48000,chord,mode,512);const std::string name=std::string(chord?"chord":"single")+"-"+(mode==0?"no-noise":mode==1?"air-blend":"noise-solo");
  wav(folder/(name+"-raw.wav"),audio);double squares=0,peak=0;for(float x:audio){squares+=x*x;peak=std::max(peak,std::abs(double(x)));}
  const double rms=std::sqrt(squares/audio.size()),gain=std::pow(10.,-24./20)/rms;
  require(peak*gain<.95,"Matched clip lacks peak headroom");for(auto&x:audio)x=float(x*gain);wav(folder/(name+".wav"),audio);
  report<<(chord?"chord":"single")<<','<<mode<<','<<rms<<','<<peak<<','<<gain<<','<<peak*gain<<'\n';
  html<<"<section><h2>"<<(chord?"E minor chord":"Percussive E notes")<<" / "<<(mode==0?"oscillators only":mode==1?"oscillators + air blend":"noise solo")<<"</h2><audio controls preload='none' src='"<<name<<".wav'></audio></section>";
 }
 html<<"<script>document.addEventListener('play',e=>{document.querySelectorAll('audio').forEach(a=>{if(a!==e.target)a.pause()})},true)</script>";require(bool(html)&&bool(report),"write report failed");std::cout<<"Wrote six matched listening clips, six raw WAVs, measurements and player: "<<folder<<'\n';return 0;
}catch(const std::exception&e){std::cerr<<"FAIL: "<<e.what()<<'\n';return 1;}}
