#pragma once

void stereoSpreadChecks(){
 auto p=std::make_unique<MemoryLabProcessor>();
 auto q=std::make_unique<MemoryLabProcessor>();
 for(int part=0;part<4;++part){
  const auto id=MemoryLabProcessor::partId(part,"stereoSpread");
  auto* param=p->state.getParameter(id);
  check(param&&param==p->getParameters()[501+part]&&param->isAutomatable()&&param->getDefaultValue()==0,
        "stereo spread append/default");
  set(*p,id.toRawUTF8(),.6f);
 }
 set(*p,"postFilterAmp",1);
 const auto current=memorylab::patch::encode(*p);
 juce::MemoryBlock host;
 p->getStateInformation(host);
 q->setStateInformation(host.getData(),int(host.getSize()));
 check(memorylab::patch::encode(*q)==current,"stereo spread host recall");
 auto older=juce::parseXML(current);
 older->setAttribute("schemaVersion",20);
 for(int i=older->getNumChildElements()-1;i>=0;--i){
  auto* child=older->getChildElement(i);
  if((child->getStringAttribute("id").endsWith("stereoSpread")||child->getStringAttribute("id").endsWith("unisonSize")||child->getStringAttribute("id").endsWith("unisonDetune")))older->removeChildElement(child,true);
 }
 check(memorylab::patch::apply(*q,older->toString()).wasOk(),"schema20 migration");
 for(int part=0;part<4;++part)
  check(q->state.getRawParameterValue(MemoryLabProcessor::partId(part,"stereoSpread"))->load()==0,
        "legacy patch retained stereo spread");
 check(q->state.getRawParameterValue("postFilterAmp")->load()==1,"legacy migration lost amp position");
 check(memorylab::patch::apply(*q,current).wasOk(),"stereo spread patch recall");
 q->editingPart=2;
 auto editor=std::unique_ptr<juce::AudioProcessorEditor>(q->createEditor());
 auto* expression=dynamic_cast<juce::Button*>(find(*editor,"expressionPage"));
 check(expression&&expression->onClick,"expression navigation");
 expression->onClick();
 check(memorylab::patch::encode(*q)==current,"expression navigation changed spread");
 auto* knob=dynamic_cast<juce::Slider*>(find(*editor,"part3__stereoSpread"));
 check(knob!=nullptr,"stereo spread missing for selected part");
 knob->setValue(.25,juce::sendNotificationSync);
 check(std::abs(q->state.getRawParameterValue("part3__stereoSpread")->load()-.25f)<1e-6
       &&std::abs(q->state.getRawParameterValue("stereoSpread")->load()-.6f)<1e-6,"spread edit crossed parts");
 std::cout<<"PASS spread append, saved state, schema20 migration and selected-part editing\n";
}
