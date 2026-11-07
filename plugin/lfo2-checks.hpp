#pragma once
#include "Patch.hpp"
void lfo2Checks(){
 using P=memorylab::Param;
 auto newerLfo=[](const juce::String&id){return id.startsWith("lfo2")||id.startsWith("lfo3")||id.startsWith("lfo4")||id.startsWith("env3")||id.startsWith("env4");};
 auto render=[](double sr,int block,int source,int route,int shape,bool sync,bool retrigger,int delay,int division=2,double bpm=120){
  auto synth=std::make_unique<memorylab::Synth>();synth->prepare(sr);
  auto param=[&](P id,double value){synth->dispatch({0,memorylab::EventKind::Parameter,0,value,0,id});};
  param(P::Drive,0);param(P::Cutoff,1000);param(P::Q,.8);param(P::AmpSustain,.8);
  for(auto id:{P::LfoRate,P::Lfo2Rate})param(id,1.3);
  for(auto id:{P::LfoShape,P::Lfo2Shape})param(id,shape);
  for(auto id:{P::LfoSync,P::Lfo2Sync})param(id,sync?1:0);
  for(auto id:{P::LfoRetrigger,P::Lfo2Retrigger})param(id,retrigger?1:0);
  for(auto id:{P::LfoDivision,P::Lfo2Division})param(id,division);
  for(auto id:{P::LfoBpm,P::Lfo2Bpm})param(id,bpm);
  const P old[]{P::PitchLfo,P::PwmLfo,P::CutoffLfo},second[]{P::Lfo2Pitch,P::Lfo2Pwm,P::Lfo2Cutoff};const double amounts[]{12,.12,1};
  if(source==1)param(old[route],amounts[route]);if(source==2)param(second[route],amounts[route]);
  if(source>=3){param(P::LfoRate,4);param(P::Lfo2Rate,.7);if(source!=5)param(P::PitchLfo,12);if(source!=4)param(P::Lfo2Cutoff,1);}
  std::vector<float>silent(std::size_t(sr)+delay);synth->render(silent.data(),silent.size());synth->dispatch({0,memorylab::EventKind::NoteOn,60,.8});
  std::vector<float>out(std::size_t(sr*.25));for(std::size_t n=0;n<out.size();n+=block)synth->render(out.data()+n,std::min(std::size_t(block),out.size()-n));
  for(float v:out)check(std::isfinite(v)&&std::abs(v)<1,"LFO 2 bad audio");return out;
 };
 for(double sr:{44100.,48000.,96000.,192000.}){
  for(int shape=0;shape<4;++shape)for(int route=0;route<3;++route){
   const auto first=render(sr,512,1,route,shape,false,true,0),second=render(sr,512,2,route,shape,false,true,0);
   check(second!=render(sr,512,0,route,shape,false,true,0),"LFO 2 route silent");
   for(std::size_t n=0;n<first.size();++n)check(std::abs(first[n]-second[n])<2e-6,"LFO 2 route differs from equivalent LFO 1");
  }
  const auto combined=render(sr,512,3,0,0,false,true,0);check(combined==render(sr,127,3,0,0,false,true,0),"two-LFO block partition mismatch");
  check(combined!=render(sr,512,4,0,0,false,true,0)&&combined!=render(sr,512,5,2,0,false,true,0),"combined LFOs lost a source");
  check(render(sr,512,0,0,0,false,true,0)==render(sr,512,0,0,3,true,false,0),"inactive LFO choices changed sound");
  for(int division=0;division<6;++division)check(render(sr,512,2,2,0,true,false,0,division)==render(sr,127,2,2,0,true,false,0,division),"LFO 2 synced/free block mismatch");
  check(render(sr,512,2,2,0,true,true,0,0,120)==render(sr,512,2,2,0,true,true,0,1,60),"LFO 2 equivalent clock divisions differ");
  check(render(sr,512,2,2,0,true,true,0,0)==render(sr,512,2,2,0,true,true,int(sr*.125),0),"LFO 2 retrigger depends on silence");
  // Quarter-cycle delay at 2 Hz, not a full cycle at the default 8 Hz.
  check(render(sr,512,2,2,0,true,false,0,0)!=render(sr,512,2,2,0,true,false,int(sr*.125),0),"LFO 2 free run did not advance through silence");
 }
 auto p=std::make_unique<MemoryLabProcessor>();set(*p,"lfo2Pitch",17);set(*p,"lfo2Pwm",.07f);set(*p,"lfo2Cutoff",.8f);set(*p,"lfo2Sync",1);set(*p,"lfo2Bpm",97);
 const auto encoded=memorylab::patch::encode(*p);auto root=juce::parseXML(encoded);root->setAttribute("schemaVersion",8);
 for(int i=root->getNumChildElements()-1;i>=0;--i)if(newerLfo(root->getChildElement(i)->getStringAttribute("id")))root->removeChildElement(root->getChildElement(i),true);
 check(memorylab::patch::apply(*p,root->toString()).wasOk(),"schema 8 patch migration failed");
 // JUCE's bipolar PWM range roundtrip yields ~2.98e-8 at displayed zero.
 // The wrapper already canonicalizes defaults within step*.001. Verify the
 // raw tolerance AND exact rendered bypass rather than demanding bitwise raw 0.
 for(const char*id:{"lfo2Pitch","lfo2Pwm","lfo2Cutoff","lfo2Sync"})check(std::abs(p->state.getRawParameterValue(id)->load())<1e-6,"old patch retained active LFO 2");
 check(p->state.getRawParameterValue("lfo2Bpm")->load()==130,"old patch retained fallback BPM 2");
 {
  auto factory=std::make_unique<MemoryLabProcessor>();p->prepareToPlay(48000,512);factory->prepareToPlay(48000,512);
  juce::AudioBuffer<float>migrated(2,512),reference(2,512);juce::MidiBuffer a,b;
  a.addEvent(juce::MidiMessage::noteOn(1,60,(juce::uint8)100),0);b.addEvent(juce::MidiMessage::noteOn(1,60,(juce::uint8)100),0);
  p->processBlock(migrated,a);factory->processBlock(reference,b);
  for(int i=0;i<512;++i)check(migrated.getSample(0,i)==reference.getSample(0,i),"legacy patch zero depths changed actual audio");
 }
 check(memorylab::patch::apply(*p,encoded).wasOk(),"schema 9 restore failed");auto legacy=p->state.copyState();
 for(int i=legacy.getNumChildren()-1;i>=0;--i)if(newerLfo(legacy.getChild(i).getProperty("id").toString()))legacy.removeChild(i,nullptr);
 auto xml=legacy.createXml();juce::MemoryBlock state;juce::AudioProcessor::copyXmlToBinary(*xml,state);p->setStateInformation(state.getData(),int(state.getSize()));
 check(p->state.getRawParameterValue("lfo2Pitch")->load()==0&&p->state.getRawParameterValue("lfo2Cutoff")->load()==0,"legacy host retained LFO 2");
 struct Host:juce::AudioPlayHead{double bpm=137;bool available=true;juce::Optional<PositionInfo>getPosition()const override{PositionInfo pos;if(available)pos.setBpm(bpm);return pos;}}host;
 p->setPlayHead(&host);p->prepareToPlay(48000,512);set(*p,"lfo2Bpm",97);set(*p,"lfoBpm",91);set(*p,"arpSync",0);juce::AudioBuffer<float>a(2,512);juce::MidiBuffer m;p->processBlock(a,m);
 check(p->lfo2Tempo.load()==137&&p->lfo2HasHostTempo.load(),"LFO 2 host BPM");host.available=false;p->processBlock(a,m);
 check(p->lfo2Tempo.load()==97&&p->lfoTempo.load()==91&&!p->lfo2HasHostTempo.load(),"independent LFO fallbacks");host.available=true;host.bpm=std::numeric_limits<double>::quiet_NaN();p->processBlock(a,m);check(p->lfo2Tempo.load()==97,"LFO 2 invalid host BPM");p->setPlayHead(nullptr);
 auto editor=std::unique_ptr<juce::AudioProcessorEditor>(p->createEditor());auto*page=dynamic_cast<juce::Button*>(find(*editor,"motionPage"));check(page,"MOTION missing");page->onClick();
 auto*second=dynamic_cast<juce::Button*>(find(*editor,"motion2ClockPage"));check(second&&second->isVisible(),"CLOCK 2 navigation missing");second->onClick();
 auto*rate=find(*editor,"lfo2Rate"),*division=find(*editor,"lfo2Division");check(rate&&division&&rate->isEnabled()&&!division->isEnabled(),"LFO 2 free enabled state");set(*p,"lfo2Sync",1);
 const auto deadline=juce::Time::getMillisecondCounterHiRes()+1000;
 while(rate->isEnabled()&&juce::Time::getMillisecondCounterHiRes()<deadline){
#if JUCE_WINDOWS
  uia_checks::pump();
#endif
  juce::Thread::sleep(5);juce::Timer::callPendingTimersSynchronously();
 }
 check(!rate->isEnabled()&&division->isEnabled(),"LFO 2 sync enabled states did not follow host");
 std::cout<<"PASS LFO 2: three routes/four shapes at four rates, equivalent LFO 1 response, two-source block parity, six sync divisions, retrigger/free run, schema-8 and host migration, independent host/fallback BPM and editor bindings\n";
}
