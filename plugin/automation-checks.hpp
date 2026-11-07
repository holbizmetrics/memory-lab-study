#pragma once

void automationChecks(){
 auto processor=std::make_unique<MemoryLabProcessor>();
 auto editor=std::unique_ptr<juce::AudioProcessorEditor>(processor->createEditor());
 const char*ids[]{"drive","depth","recovery","detune","width","cutoff","resonance","volume","profile","lfoRate","vibrato","pwm","filterLfo","filterEnv","pressure","phaseReset","ampDelay","ampAttack","ampHold","ampDecay","ampSustain","ampRelease","fltDelay","fltAttack","fltHold","fltDecay","fltSustain","fltRelease","arpOn","arpSync","arpBpm","arpRate","arpOrder","arpOctaves","arpGate","voiceDrive","sawLevel","pulseLevel","osc2Interval","noiseLevel","filterFamily","lfoShape","lfoSync","lfoDivision","lfoRetrigger","lfoBpm","keyTrack","ampVelocity","filterVelocity","lfo2Rate","lfo2Pitch","lfo2Pwm","lfo2Cutoff","lfo2Shape","lfo2Sync","lfo2Division","lfo2Retrigger","lfo2Bpm","lfo3Rate","lfo3Pitch","lfo3Pwm","lfo3Cutoff","lfo3Shape","lfo3Sync","lfo3Division","lfo3Retrigger","lfo3Bpm","lfo4Rate","lfo4Pitch","lfo4Pwm","lfo4Cutoff","lfo4Shape","lfo4Sync","lfo4Division","lfo4Retrigger","lfo4Bpm","env3Delay","env3Attack","env3Hold","env3Decay","env3Sustain","env3Release","env3Pitch","env3Cutoff","env4Delay","env4Attack","env4Hold","env4Decay","env4Sustain","env4Release","env4Pitch","env4Cutoff","lfoAmp","lfo2Amp","lfo3Amp","lfo4Amp","ampPitch","fltPitch","env3Pwm","env4Pwm"};
 check(processor->getParameters().size()==100,"automation parameter count changed");
 for(int i=0;i<100;++i){auto*p=processor->state.getParameter(ids[i]);check(p&&p==processor->getParameters()[i],"automation ID/order changed");check(p->isAutomatable(),"parameter not automatable");}
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
 page("expressionPage");exercise("keyTrack",.2,.6,.8);exercise("ampVelocity",.4,.7,1);exercise("filterVelocity",.3,.6,1);page("voicePage");exercise("cutoff",400,1800,2800);exercise("resonance",1,3,2);
 page("motionPage");exercise("vibrato",3,8,12);
 // A hidden parameter must retain host edits and rebind without host feedback.
 set(*processor,"cutoff",3300);page("voicePage");
 auto*cutoff=dynamic_cast<juce::Slider*>(find(*editor,"cutoff"));check(cutoff&&std::abs(cutoff->getValue()-3300)<1,"hidden automation lost on page return");
 page("envelopePage");exercise("ampAttack",20,80,120);
 page("filterEnvelopePage");exercise("fltDecay",100,200,300);
 page("arpPage");exercise("arpGate",20,40,70);exercise("arpOrder",1,2,0);
 page("motionPage");exercise("lfoShape",1,2,3);page("motionClockPage");exercise("lfoSync",1,0,1);exercise("lfoDivision",1,3,4);exercise("lfoRetrigger",0,1,0);exercise("lfoBpm",90,110,140);page("motion2Page");exercise("lfo2Rate",.5,2,3);exercise("lfo2Pitch",5,12,8);exercise("lfo2Pwm",.05,.1,.15);exercise("lfo2Cutoff",.2,.8,1);exercise("lfo2Shape",1,2,3);page("motion2ClockPage");exercise("lfo2Sync",1,0,1);exercise("lfo2Division",1,3,4);exercise("lfo2Retrigger",0,1,0);exercise("lfo2Bpm",90,110,140);page("motion3Page");exercise("lfo3Rate",.5,2,3);exercise("lfo3Pitch",5,12,8);exercise("lfo3Pwm",.05,.1,.15);exercise("lfo3Cutoff",.2,.8,1);exercise("lfo3Shape",1,2,3);page("motion3ClockPage");exercise("lfo3Sync",1,0,1);exercise("lfo3Division",1,3,4);exercise("lfo3Retrigger",0,1,0);exercise("lfo3Bpm",90,110,140);page("motion4Page");exercise("lfo4Rate",.5,2,3);exercise("lfo4Pitch",5,12,8);exercise("lfo4Pwm",.05,.1,.15);exercise("lfo4Cutoff",.2,.8,1);exercise("lfo4Shape",1,2,3);page("motion4ClockPage");exercise("lfo4Sync",1,0,1);exercise("lfo4Division",1,3,4);exercise("lfo4Retrigger",0,1,0);exercise("lfo4Bpm",90,110,140);page("drivePage");exercise("voiceDrive",8,24,12);page("oscPage");exercise("sawLevel",.1,.8,.42);exercise("pulseLevel",.2,.9,.28);exercise("osc2Interval",-12,7,12);exercise("noiseLevel",.08,.35,.1);page("familyPage");exercise("filterFamily",1,2,3);page("env3Page");exercise("env3Delay",100,200,300);exercise("env3Attack",100,200,300);exercise("env3Hold",100,200,300);exercise("env3Decay",100,200,300);exercise("env3Release",100,200,300);exercise("env3Sustain",.2,.7,.4);exercise("env3Pitch",100,-200,300);exercise("env3Cutoff",1,-2,.5);page("env4Page");exercise("env4Delay",100,200,300);exercise("env4Attack",100,200,300);exercise("env4Hold",100,200,300);exercise("env4Decay",100,200,300);exercise("env4Release",100,200,300);exercise("env4Sustain",.2,.7,.4);exercise("env4Pitch",100,-200,300);exercise("env4Cutoff",1,-2,.5);page("motionPage");exercise("lfoAmp",.2,-.5,.7);page("motion2Page");exercise("lfo2Amp",.2,-.5,.7);page("motion3Page");exercise("lfo3Amp",.2,-.5,.7);page("motion4Page");exercise("lfo4Amp",.2,-.5,.7);page("envelopeRoutesPage");exercise("ampPitch",100,-200,300);exercise("fltPitch",100,-200,300);exercise("env3Pwm",.1,-.2,.05);exercise("env4Pwm",.1,-.2,.05);juce::MemoryBlock state;processor->getStateInformation(state);
 auto restored=std::make_unique<MemoryLabProcessor>();restored->setStateInformation(state.getData(),int(state.getSize()));
 check(std::abs(restored->state.getRawParameterValue("cutoff")->load()-3300)<1,"automation value state recall");
 check(restored->state.getRawParameterValue("arpGate")->load()==70,"arp automation value state recall");
 check(restored->state.getRawParameterValue("voiceDrive")->load()==12,"voice drive automation state recall");
 std::cout<<"PASS automation foundation: 100 stable automatable IDs/order, balanced knob gestures, parameter-to-editor updates without gesture echo, hidden-page rebinding and value recall. FL host recording/playback not covered.\n";
}
