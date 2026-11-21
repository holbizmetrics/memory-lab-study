#include "voice.hpp"
#include <array>
#include <iostream>
#include <memory>
#include <vector>

using namespace memorylab;

int main(){
 const std::array routes{Param::Osc1Lfo1Pwm,Param::Osc1Lfo2Pwm,Param::Osc1Lfo3Pwm,
                         Param::Osc1Lfo4Pwm,Param::Osc1Env3Pwm,Param::Osc1Env4Pwm};
 auto synth=std::make_unique<Synth>();
 auto render=[&](double rate,bool osc1,Param route,double depth,bool changeHeld=false,bool oldPwm=false){
  synth->prepare(rate);
  auto set=[&](Param id,double value){synth->dispatch({0,EventKind::Parameter,0,value,0,id});};
  set(Param::Drive,0);
  set(Param::SawLevel,0);
  set(Param::Osc1PulseLevel,osc1?.35:0);
  set(Param::PulseLevel,osc1?0:.35);
  set(Param::Cutoff,5000);
  if(oldPwm)set(Param::PwmLfo,depth);
  else if(!changeHeld)set(route,depth);
  synth->primeParameters();
  synth->dispatch({0,EventKind::NoteOn,48,1});
  std::vector<float> audio(std::size_t(rate*.5));
  const auto split=audio.size()/2;
  synth->render(audio.data(),split);
  if(changeHeld)set(route,depth);
  synth->render(audio.data()+split,audio.size()-split);
  return audio;
 };
 for(const double rate:{44100.,48000.,96000.,192000.}){
  const auto osc1=render(rate,true,routes[0],0);
  const auto osc2=render(rate,false,routes[0],0);
  for(const auto route:routes){
   const auto modulated=render(rate,true,route,.30);
   const auto negative=render(rate,true,route,-.25);
   if(modulated==osc1||negative==modulated||render(rate,false,route,.30)!=osc2||
      !std::all_of(modulated.begin(),modulated.end(),[](float v){return std::isfinite(v)&&std::abs(v)<1;})){
    std::cerr<<"OSC 1 PWM routing failed at "<<rate<<" Hz\n";
    return 1;
   }
  }
  const auto held=render(rate,true,routes[0],.3,true);
  const auto halfway=osc1.size()/2;
  if(!std::equal(osc1.begin(),osc1.begin()+halfway,held.begin())||
     std::equal(osc1.begin()+halfway,osc1.end(),held.begin()+halfway)||
     render(rate,true,routes[0],.3,false,true)!=osc1||
     render(rate,false,routes[0],.3,false,true)==osc2){
   std::cerr<<"Held PWM or legacy OSC 2 PWM failed at "<<rate<<" Hz\n";
   return 1;
  }
 }
 auto bank=std::make_unique<OscillatorBank>();
 bank->prepare();
 constexpr int rate=44100, carrier=3675, modulation=2;
 std::vector<double> bright(rate);
 double energy=0;
 for(int i=0;i<rate;++i){
  const double t=double(i)/rate;
  bright[i]=bank->pulse(double(i%12)/12.,1./12.,.5+.28*std::sin(2*pi*modulation*t));
  energy+=bright[i]*bright[i];
 }
 auto binEnergy=[&](int frequency){
  double re=0,im=0;
  const double angle=2*pi*frequency/rate;
  for(int n=0;n<rate;++n){re+=bright[n]*std::cos(angle*n);im+=bright[n]*std::sin(angle*n);}
  return 2*(re*re+im*im)/rate;
 };
 double explained=0;
 for(int harmonic=1;harmonic<=4;++harmonic)
  for(int offset=-20;offset<=20;++offset)
   explained+=binEnergy(carrier*harmonic+offset*modulation);
 if(binEnergy(carrier+modulation)<energy*.001||(energy-explained)/energy>1e-5){
  std::cerr<<"High-note PWM sideband or out-of-band energy check failed\n";
  return 1;
 }
 std::cout<<"PASS six independent OSC 1 PWM routes, held edits, OSC 2 isolation, and high-note sine PWM spectrum\n";
}
