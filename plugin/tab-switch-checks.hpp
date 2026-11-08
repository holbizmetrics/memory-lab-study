#pragma once
void tabSwitchChecks(){
 for(float family:{0.f,1.f,2.f,3.f}){
  auto p=std::make_unique<MemoryLabProcessor>();set(*p,"filterFamily",family);set(*p,"cutoff",987);set(*p,"resonance",3.2f);set(*p,"noiseLevel",.08f);
  std::vector<float>before;for(auto*parameter:p->getParameters())before.push_back(parameter->getValue());
  struct Monitor:juce::AudioProcessorParameter::Listener{int writes=0;void parameterValueChanged(int,float)override{++writes;}void parameterGestureChanged(int,bool)override{}}monitor;
  for(auto*parameter:p->getParameters())parameter->addListener(&monitor);
  auto editor=std::unique_ptr<juce::AudioProcessorEditor>(p->createEditor());
  for(int cycle=0;cycle<4;++cycle)for(const char*id:{"voicePage","expressionPage","voiceSoundPage","oscPage","oscTuningPage","oscMixPage","motionPage","motionClockPage","motion2Page","motion2ClockPage","motion3Page","motion3ClockPage","motion4Page","motion4ClockPage","envelopePage","filterEnvelopePage","env3Page","env4Page","envelopeRoutesPage","familyPage","arpPage","dicePage","presetPage","drivePage"}){auto*b=dynamic_cast<juce::Button*>(find(*editor,id));check(b&&bool(b->onClick),"tab missing");b->onClick();}
  auto*profile=dynamic_cast<juce::Slider*>(find(*editor,"profile"));check(profile&&profile->isEnabled()==(family==0),"Classic Profile active in wrong family");
  set(*p,"filterFamily",family==0?3.f:0.f);
  const auto deadline=juce::Time::getMillisecondCounterHiRes()+1000.;
  while(profile->isEnabled()!=(family!=0)&&juce::Time::getMillisecondCounterHiRes()<deadline){
#if JUCE_WINDOWS
   // Dispatch only this isolated test process's messages; no FL UI Automation.
   uia_checks::pump();
#endif
   juce::Thread::sleep(5);juce::Timer::callPendingTimersSynchronously();
  }
  check(profile->isEnabled()==(family!=0),"Classic Profile did not follow host family change");
  set(*p,"filterFamily",family);
  // The two intentional host writes above are excluded from the editor-write check.
  monitor.writes-=2;
  editor.reset();for(auto*parameter:p->getParameters())parameter->removeListener(&monitor);
  check(monitor.writes==0,"opening editor or switching tabs wrote a parameter");for(int i=0;i<p->getParameters().size();++i)check(p->getParameters()[i]->getValue()==before[std::size_t(i)],"tab switch changed sound settings");
 }
 std::cout<<"PASS: opening/closing editor and 384 page switches preserve every parameter, with zero parameter writes, across all four families\n";
}
