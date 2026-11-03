#pragma once
#include <vector>

void expressionChecks(){
 auto render=[](double sr,int block,int note,double velocity,double cutoff,double tracking,double amp,double filter,double amount,bool noise){
  auto synth=std::make_unique<memorylab::Synth>();synth->prepare(sr);
  auto param=[&](memorylab::Param id,double value){synth->dispatch({0,memorylab::EventKind::Parameter,0,value,0,id});};
  param(memorylab::Param::Drive,0);param(memorylab::Param::Depth,0);param(memorylab::Param::Cutoff,cutoff);param(memorylab::Param::Q,.71);
  param(memorylab::Param::KeyTrack,tracking);param(memorylab::Param::AmpVelocity,amp);param(memorylab::Param::FilterVelocity,filter);param(memorylab::Param::FilterEnvelope,amount);
  if(noise){param(memorylab::Param::SawLevel,0);param(memorylab::Param::PulseLevel,0);param(memorylab::Param::NoiseLevel,.2);}
  std::vector<float>settle(std::size_t(sr),0);synth->render(settle.data(),settle.size());synth->dispatch({0,memorylab::EventKind::NoteOn,note,velocity});
  std::vector<float>out(std::size_t(sr*.3));for(std::size_t pos=0;pos<out.size();pos+=block)synth->render(out.data()+pos,std::min(std::size_t(block),out.size()-pos));
  for(float v:out)check(std::isfinite(v)&&std::abs(v)<1,"expression fixture bad audio");return out;
 };
 auto energy=[](const std::vector<float>&x){double sum=0;for(float v:x)sum+=v*v;return sum;};
 auto close=[](const std::vector<float>&a,const std::vector<float>&b){check(a.size()==b.size(),"expression fixture length");for(std::size_t i=0;i<a.size();++i)check(std::abs(a[i]-b[i])<2e-7,"keyboard tracking wrong cutoff ratio");};
 for(double sr:{44100.,48000.,96000.,192000.}){
  // Same note, expected cutoff: C4 anchor, +/- octave and half tracking.
  close(render(sr,512,60,.75,1450,1,1,1,0,false),render(sr,512,60,.75,1450,0,1,1,0,false));
  close(render(sr,512,72,.75,1450,1,1,1,0,false),render(sr,512,72,.75,2900,0,1,1,0,false));
  close(render(sr,512,48,.75,1450,1,1,1,0,false),render(sr,512,48,.75,725,0,1,1,0,false));
  close(render(sr,512,84,.75,1450,.5,1,1,0,false),render(sr,512,84,.75,2900,0,1,1,0,false));
  for(bool noise:{false,true}){
   check(render(sr,512,60,.25,1450,0,0,1,0,noise)==render(sr,512,60,.75,1450,0,0,1,0,noise),"zero amp sensitivity still responds to velocity");
   const auto low=render(sr,512,60,.25,1450,0,1,1,0,noise),high=render(sr,512,60,.75,1450,0,1,1,0,noise);
   check(energy(low)>1e-9&&std::abs(std::sqrt(energy(high)/energy(low))-3)<1e-5,"legacy amplitude velocity ratio changed");
  }
  const auto low=render(sr,512,60,.25,700,0,0,0,2,false),high=render(sr,512,60,.75,700,0,0,0,2,false);
  check(low==high,"zero filter sensitivity still responds to velocity");
  check(render(sr,512,60,.25,700,0,0,1,2,false)!=render(sr,512,60,.75,700,0,0,1,2,false),"filter sensitivity did not change envelope response");
  check(render(sr,512,84,.6,1450,.7,.4,.3,2,true)==render(sr,127,84,.6,1450,.7,.4,.3,2,true),"expression block parity");
  for(int note:{0,127})for(double tracking:{0.,1.})render(sr,512,note,1,18000,tracking,0,0,4,false);
 }
 auto p=std::make_unique<MemoryLabProcessor>();set(*p,"keyTrack",.75f);set(*p,"ampVelocity",.4f);set(*p,"filterVelocity",.6f);juce::MemoryBlock state;p->getStateInformation(state);
 auto restored=std::make_unique<MemoryLabProcessor>();restored->setStateInformation(state.getData(),int(state.getSize()));check(restored->state.getRawParameterValue("keyTrack")->load()==.75f&&std::abs(restored->state.getRawParameterValue("ampVelocity")->load()-.4f)<1e-6&&std::abs(restored->state.getRawParameterValue("filterVelocity")->load()-.6f)<1e-6,"expression host recall");
 auto legacy=p->state.copyState();for(const char*id:{"keyTrack","ampVelocity","filterVelocity"})legacy.removeChild(legacy.getChildWithProperty("id",id),nullptr);auto xml=legacy.createXml();juce::AudioProcessor::copyXmlToBinary(*xml,state);restored->setStateInformation(state.getData(),int(state.getSize()));
 check(restored->state.getRawParameterValue("keyTrack")->load()==0&&restored->state.getRawParameterValue("ampVelocity")->load()==1&&restored->state.getRawParameterValue("filterVelocity")->load()==1,"legacy expression host defaults");
 std::cout<<"PASS expression: C4/octave/half tracking ratios, oscillator/noise amp velocity, independent filter velocity, four sample rates, note/frequency edges, block parity and host migration\n";
}
