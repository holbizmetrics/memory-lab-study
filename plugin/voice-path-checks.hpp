#pragma once

void voicePathChecks(){
 auto p=std::make_unique<MemoryLabProcessor>();
 auto q=std::make_unique<MemoryLabProcessor>();
 for(int part=0;part<4;++part){
  const auto id=MemoryLabProcessor::partId(part,"postFilterAmp");
  auto* param=p->state.getParameter(id);
  check(param&&param==p->getParameters()[497+part]&&param->isAutomatable()&&param->getDefaultValue()==0,
        "amp position append/default");
  set(*p,id.toRawUTF8(),1);
 }
 set(*p,"osc1PwmEnv3",.2f);
 const auto current=memorylab::patch::encode(*p);
 juce::MemoryBlock host;
 p->getStateInformation(host);
 q->setStateInformation(host.getData(),int(host.getSize()));
 check(memorylab::patch::encode(*q)==current,"voice path host recall");
 auto older=juce::parseXML(current);
 older->setAttribute("schemaVersion",19);
 for(int i=older->getNumChildElements()-1;i>=0;--i){
  auto* child=older->getChildElement(i);
  if((child->getStringAttribute("id").endsWith("postFilterAmp")||child->getStringAttribute("id").endsWith("stereoSpread")))older->removeChildElement(child,true);
 }
 check(memorylab::patch::apply(*q,older->toString()).wasOk(),"schema19 migration");
 for(int part=0;part<4;++part)
  check(q->state.getRawParameterValue(MemoryLabProcessor::partId(part,"postFilterAmp"))->load()==0,
        "legacy patch retained post-filter amp");
 check(std::abs(q->state.getRawParameterValue("osc1PwmEnv3")->load()-.2f)<1e-6,
       "legacy migration lost PWM");
 check(memorylab::patch::apply(*q,current).wasOk()&&memorylab::patch::encode(*q)==current,
       "voice path patch recall");
 q->editingPart=2;
 auto editor=std::unique_ptr<juce::AudioProcessorEditor>(q->createEditor());
 auto* drive=dynamic_cast<juce::Button*>(find(*editor,"drivePage"));
 check(drive&&drive->onClick,"drive navigation");
 drive->onClick();
 check(memorylab::patch::encode(*q)==current,"drive navigation changed voice path");
 auto* knob=dynamic_cast<juce::Slider*>(find(*editor,"part3__postFilterAmp"));
 check(knob!=nullptr,"amp position missing for selected part");
 knob->setValue(0,juce::sendNotificationSync);
 check(q->state.getRawParameterValue("part3__postFilterAmp")->load()==0&&
       q->state.getRawParameterValue("postFilterAmp")->load()==1,"voice path crossed parts");
 q->copyPart(0,2);
 check(q->state.getRawParameterValue("part3__postFilterAmp")->load()==1,"voice path copy missing");
 std::cout<<"PASS voice path parameter append, legacy migration, recall and selected-part editing\n";
}
