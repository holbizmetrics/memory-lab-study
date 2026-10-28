#pragma once

void automationChecks(){
 auto processor=std::make_unique<MemoryLabProcessor>();
 auto editor=std::unique_ptr<juce::AudioProcessorEditor>(processor->createEditor());
 const char*ids[]{"drive","depth","recovery","detune","width","cutoff","resonance","volume","profile","lfoRate","vibrato","pwm","filterLfo","filterEnv","pressure","phaseReset","ampDelay","ampAttack","ampHold","ampDecay","ampSustain","ampRelease","fltDelay","fltAttack","fltHold","fltDecay","fltSustain","fltRelease","arpOn","arpSync","arpBpm","arpRate","arpOrder","arpOctaves","arpGate"};
 check(processor->getParameters().size()==35,"automation parameter count changed");
 for(int i=0;i<35;++i){auto*p=processor->state.getParameter(ids[i]);check(p&&p==processor->getParameters()[i],"automation ID/order changed");check(p->isAutomatable(),"parameter not automatable");}
 struct Monitor:juce::AudioProcessorParameter::Listener{
  std::string events;
  void parameterValueChanged(int,float)override{events+='V';}
  void parameterGestureChanged(int,bool start)override{events+=start?'B':'E';}
 };
 auto page=[&](const char*id){auto*b=dynamic_cast<juce::Button*>(find(*editor,id));check(b&&bool(b->onClick),"automation page missing");b->onClick();};
 auto exercise=[&](const char*id,double first,double second,double host){
  auto*s=dynamic_cast<juce::Slider*>(find(*editor,id));check(s,"automated slider missing");
  auto*p=processor->state.getParameter(id);Monitor monitor;p->addListener(&monitor);
  {juce::Slider::ScopedDragNotification drag(*s);s->setValue(first,juce::sendNotificationSync);s->setValue(second,juce::sendNotificationSync);}
  const auto gestures=monitor.events;monitor.events.clear();
  set(*processor,id,float(host));
  const auto hostEvents=monitor.events;const double displayed=s->getValue();p->removeListener(&monitor);
  check(gestures=="BVVE","knob gesture is not one begin/two values/end");
  check(hostEvents=="V","host update echoed an edit or gesture");
  check(std::abs(displayed-host)<.02,"host automation did not update knob");
 };
 exercise("cutoff",400,1800,2800);exercise("resonance",1,3,2);
 page("motionPage");exercise("vibrato",3,8,12);
 // A hidden parameter must retain host edits and rebind without host feedback.
 set(*processor,"cutoff",3300);page("voicePage");
 auto*cutoff=dynamic_cast<juce::Slider*>(find(*editor,"cutoff"));check(cutoff&&std::abs(cutoff->getValue()-3300)<1,"hidden automation lost on page return");
 page("envelopePage");exercise("ampAttack",20,80,120);
 page("filterEnvelopePage");exercise("fltDecay",100,200,300);
 page("arpPage");exercise("arpGate",20,40,70);exercise("arpOrder",1,2,0);
 juce::MemoryBlock state;processor->getStateInformation(state);
 auto restored=std::make_unique<MemoryLabProcessor>();restored->setStateInformation(state.getData(),int(state.getSize()));
 check(std::abs(restored->state.getRawParameterValue("cutoff")->load()-3300)<1,"automation value state recall");
 check(restored->state.getRawParameterValue("arpGate")->load()==70,"arp automation value state recall");
 std::cout<<"PASS automation foundation: 35 stable automatable IDs/order, balanced knob gestures, parameter-to-editor updates without gesture echo, hidden-page rebinding and value recall. FL host recording/playback not covered.\n";
}
