#pragma once
#include "Patch.hpp"
void fourLfoChecks(){
 using P=memorylab::Param;
 struct Source{P rate,pitch,pwm,cutoff,shape,sync,division,retrigger,bpm;};
 const std::array<Source,4> sources{{
  {P::LfoRate,P::PitchLfo,P::PwmLfo,P::CutoffLfo,P::LfoShape,P::LfoSync,P::LfoDivision,P::LfoRetrigger,P::LfoBpm},
  {P::Lfo2Rate,P::Lfo2Pitch,P::Lfo2Pwm,P::Lfo2Cutoff,P::Lfo2Shape,P::Lfo2Sync,P::Lfo2Division,P::Lfo2Retrigger,P::Lfo2Bpm},
  {P::Lfo3Rate,P::Lfo3Pitch,P::Lfo3Pwm,P::Lfo3Cutoff,P::Lfo3Shape,P::Lfo3Sync,P::Lfo3Division,P::Lfo3Retrigger,P::Lfo3Bpm},
  {P::Lfo4Rate,P::Lfo4Pitch,P::Lfo4Pwm,P::Lfo4Cutoff,P::Lfo4Shape,P::Lfo4Sync,P::Lfo4Division,P::Lfo4Retrigger,P::Lfo4Bpm}
 }};
 auto render=[&](double sr,int block,int mask,int route,int shape,bool sync,bool retrigger,int delay,int division=0,bool diverse=false,bool varyInactive=false){
  auto synth=std::make_unique<memorylab::Synth>();synth->prepare(sr);
  auto param=[&](P id,double value){synth->dispatch({0,memorylab::EventKind::Parameter,0,value,0,id});};
  param(P::Drive,0);param(P::Cutoff,1000);param(P::Q,.8);
  const double rates[]{4,.7,.35,1.1};
  for(int i=0;i<4;++i){const auto&s=sources[std::size_t(i)];const bool active=(mask&(1<<i))!=0;
   param(s.rate,diverse?rates[i]:2);param(s.shape,diverse?i:shape);param(s.sync,sync?1:0);param(s.division,division);param(s.bpm,120);param(s.retrigger,retrigger?1:0);
   if(varyInactive&&i>=2&&!active){param(s.rate,17);param(s.shape,3);param(s.sync,1);param(s.division,5);param(s.retrigger,0);}
   if(active){if(route==0)param(s.pitch,12);else if(route==1)param(s.pwm,.12);else if(route==2)param(s.cutoff,1);else if(i==0)param(s.pitch,8);else if(i==2)param(s.pwm,.06);else param(s.cutoff,i==1?.5:.3);}
  }
  std::vector<float>silent(std::size_t(sr)+delay);synth->render(silent.data(),silent.size());synth->dispatch({0,memorylab::EventKind::NoteOn,60,.8});
  std::vector<float>out(std::size_t(sr*.25));for(std::size_t n=0;n<out.size();n+=block)synth->render(out.data()+n,std::min(std::size_t(block),out.size()-n));
  for(float v:out)check(std::isfinite(v)&&std::abs(v)<1,"four-LFO fixture bad audio");return out;
 };
 for(double sr:{44100.,48000.,96000.,192000.}){
  for(int source:{2,3})for(int route=0;route<3;++route)for(int shape=0;shape<4;++shape){
   const auto reference=render(sr,512,2,route,shape,false,true,0),added=render(sr,512,1<<source,route,shape,false,true,0);
   check(added!=render(sr,512,0,route,shape,false,true,0),"new LFO route had no effect");
   for(std::size_t n=0;n<added.size();++n)check(std::abs(added[n]-reference[n])<2e-6,"new LFO differs from equivalent LFO 2");
  }
  const auto all=render(sr,512,15,-1,0,false,true,0,0,true);
  check(all==render(sr,127,15,-1,0,false,true,0,0,true),"four-LFO block partition mismatch");
  for(int source=0;source<4;++source)check(all!=render(sr,512,15^(1<<source),-1,0,false,true,0,0,true),"combined modulation lost a source");
  check(render(sr,512,2,2,0,false,true,0)==render(sr,512,2,2,0,false,true,0,0,false,true),"inactive LFO 3/4 choices changed LFO 2 audio");
  for(int source:{2,3}){
   for(int division=0;division<6;++division)check(render(sr,512,1<<source,2,0,true,false,0,division)==render(sr,127,1<<source,2,0,true,false,0,division),"new LFO clock partition mismatch");
   check(render(sr,512,1<<source,2,0,true,true,0)==render(sr,512,1<<source,2,0,true,true,int(sr*.125)),"new LFO note reset depends on silence");
   check(render(sr,512,1<<source,2,0,true,false,0)!=render(sr,512,1<<source,2,0,true,false,int(sr*.125)),"new LFO free phase did not advance");
  }
 }
 auto p=std::make_unique<MemoryLabProcessor>();set(*p,"lfo2Pitch",7);set(*p,"lfo2Rate",3);
 auto added=[](const juce::String&id){return id.startsWith("lfo3")||id.startsWith("lfo4")||id.startsWith("env3")||id.startsWith("env4")||id=="lfoAmp"||id=="lfo2Amp"||id=="lfo3Amp"||id=="lfo4Amp"||id=="ampPitch"||id=="fltPitch"||id=="env3Pwm"||id=="env4Pwm"||id=="osc1PulseLevel"||id=="osc2SawLevel"||id=="osc1Width";};
 auto defaults=[&]{for(auto*base:p->getParameters())if(auto*v=dynamic_cast<juce::RangedAudioParameter*>(base))if(added(v->paramID))check(std::abs(v->getValue()-v->getDefaultValue())<1e-6,"legacy load retained LFO 3/4 edits");};
 for(int source:{3,4}){const auto prefix="lfo"+std::to_string(source);set(*p,(prefix+"Pitch").c_str(),11);set(*p,(prefix+"Cutoff").c_str(),.6f);set(*p,(prefix+"Sync").c_str(),1);set(*p,(prefix+"Bpm").c_str(),95);}
 const auto encoded=memorylab::patch::encode(*p);auto root=juce::parseXML(encoded);root->setAttribute("schemaVersion",9);
 const auto unchanged=memorylab::patch::encode(*p);check(memorylab::patch::apply(*p,root->toString()).failed()&&memorylab::patch::encode(*p)==unchanged,"old schema accepted new controls or applied partially");
 for(int i=root->getNumChildElements()-1;i>=0;--i)if(added(root->getChildElement(i)->getStringAttribute("id")))root->removeChildElement(root->getChildElement(i),true);
 check(memorylab::patch::apply(*p,root->toString()).wasOk(),"schema 9 migration failed");defaults();check(p->state.getRawParameterValue("lfo2Pitch")->load()==7,"old patch lost active LFO 2");
 check(memorylab::patch::apply(*p,encoded).wasOk(),"schema 10 restore failed");auto legacy=p->state.copyState();legacy.setProperty("version",11,nullptr);
 for(int i=legacy.getNumChildren()-1;i>=0;--i)if(added(legacy.getChild(i).getProperty("id").toString()))legacy.removeChild(i,nullptr);
 auto xml=legacy.createXml();juce::MemoryBlock state;juce::AudioProcessor::copyXmlToBinary(*xml,state);p->setStateInformation(state.getData(),int(state.getSize()));defaults();check(p->state.getRawParameterValue("lfo2Pitch")->load()==7,"legacy host lost LFO 2");
 struct Host:juce::AudioPlayHead{double bpm=137;bool available=true;juce::Optional<PositionInfo>getPosition()const override{PositionInfo pos;if(available)pos.setBpm(bpm);return pos;}}host;
 p->setPlayHead(&host);p->prepareToPlay(48000,512);set(*p,"arpSync",0);set(*p,"lfoBpm",91);set(*p,"lfo2Bpm",97);set(*p,"lfo3Bpm",103);set(*p,"lfo4Bpm",109);
 juce::AudioBuffer<float>a(2,512);juce::MidiBuffer midi;p->processBlock(a,midi);check(p->lfo3Tempo.load()==137&&p->lfo4Tempo.load()==137&&p->lfo3HasHostTempo.load()&&p->lfo4HasHostTempo.load(),"new LFO host BPM");
 host.available=false;p->processBlock(a,midi);check(p->lfoTempo.load()==91&&p->lfo2Tempo.load()==97&&p->lfo3Tempo.load()==103&&p->lfo4Tempo.load()==109,"four independent fallback clocks");
 host.available=true;host.bpm=500;p->processBlock(a,midi);check(p->lfo3Tempo.load()==103&&p->lfo4Tempo.load()==109,"new LFO accepted invalid host BPM");p->setPlayHead(nullptr);
 auto editor=std::unique_ptr<juce::AudioProcessorEditor>(p->createEditor());auto*motion=dynamic_cast<juce::Button*>(find(*editor,"motionPage"));check(motion,"MOTION missing");motion->onClick();
 for(int source:{3,4}){const auto prefix="lfo"+std::to_string(source);auto*clock=dynamic_cast<juce::Button*>(find(*editor,"motion"+juce::String(source)+"ClockPage"));check(clock&&clock->isVisible(),"new clock navigation missing");clock->onClick();auto*rate=find(*editor,prefix+"Rate"),*division=find(*editor,prefix+"Division");check(rate&&division&&rate->isEnabled()&&!division->isEnabled(),"new free-rate enabled state");set(*p,(prefix+"Sync").c_str(),1);
  const auto deadline=juce::Time::getMillisecondCounterHiRes()+1000;while(rate->isEnabled()&&juce::Time::getMillisecondCounterHiRes()<deadline){
#if JUCE_WINDOWS
   uia_checks::pump();
#endif
   juce::Thread::sleep(5);juce::Timer::callPendingTimersSynchronously();
  }check(!rate->isEnabled()&&division->isEnabled(),"new clock enabled state did not follow host");
 }
 std::cout<<"PASS four LFOs: LFO 3/4 three destinations/four shapes/four sample rates, four-source independence and block parity, inactive isolation, six divisions, reset/free run, schema 9 and legacy-host migration, independent fallback clocks and editor bindings\n";
}
