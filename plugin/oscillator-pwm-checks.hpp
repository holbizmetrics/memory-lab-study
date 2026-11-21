#pragma once

void oscillatorPwmChecks(){
 const std::array ids{"osc1PwmLfo1","osc1PwmLfo2","osc1PwmLfo3",
                      "osc1PwmLfo4","osc1PwmEnv3","osc1PwmEnv4"};
 auto p=std::make_unique<MemoryLabProcessor>();
 for(int part=0;part<4;++part)for(std::size_t i=0;i<ids.size();++i){
  const auto id=MemoryLabProcessor::partId(part,ids[i]);
  auto* param=p->state.getParameter(id);
  check(param&&param==p->getParameters()[473+part*6+int(i)]&&param->isAutomatable(),
        "OSC 1 PWM parameter order");
  check(std::abs(param->convertFrom0to1(param->getDefaultValue()))<1e-6,"OSC 1 PWM default");
  set(*p,id.toRawUTF8(),i%2?.15f:-.15f);
 }
 set(*p,"pwm",.23f);
 const auto encoded=memorylab::patch::encode(*p);
 juce::MemoryBlock host;
 p->getStateInformation(host);
 auto q=std::make_unique<MemoryLabProcessor>();
 q->setStateInformation(host.getData(),int(host.getSize()));
 check(memorylab::patch::encode(*q)==encoded,"OSC 1 PWM host recall");
 auto older=juce::parseXML(encoded);
 older->setAttribute("schemaVersion",18);
 for(int i=older->getNumChildElements()-1;i>=0;--i){
  auto* child=older->getChildElement(i);
  if(child->getStringAttribute("id").contains("osc1Pwm"))older->removeChildElement(child,true);
 }
 check(memorylab::patch::apply(*q,older->toString()).wasOk(),"schema18 migration");
 for(int part=0;part<4;++part)for(auto id:ids)
  check(std::abs(q->state.getRawParameterValue(MemoryLabProcessor::partId(part,id))->load())<1e-6,
        "legacy patch retained a PWM depth");
 check(std::abs(q->state.getRawParameterValue("pwm")->load()-.23f)<1e-6,"legacy OSC 2 PWM changed");
 check(memorylab::patch::apply(*q,encoded).wasOk()&&memorylab::patch::encode(*q)==encoded,
       "OSC 1 PWM patch recall");
 q->editingPart=2;
 auto editor=std::unique_ptr<juce::AudioProcessorEditor>(q->createEditor());
 auto* osc=dynamic_cast<juce::Button*>(find(*editor,"oscPage"));
 auto* pwm=dynamic_cast<juce::Button*>(find(*editor,"osc1PwmPage"));
 check(osc&&pwm&&osc->onClick&&pwm->onClick,"missing PWM navigation");
 osc->onClick();
 pwm->onClick();
 check(memorylab::patch::encode(*q)==encoded,"PWM navigation changed patch");
 auto* slider=dynamic_cast<juce::Slider*>(find(*editor,"part3__osc1PwmLfo1"));
 check(slider!=nullptr,"PWM knob missing for selected part");
 slider->setValue(.2,juce::sendNotificationSync);
 check(std::abs(q->state.getRawParameterValue("part3__osc1PwmLfo1")->load()-.2f)<1e-6
       &&std::abs(q->state.getRawParameterValue("osc1PwmLfo1")->load()+.15f)<1e-6,
       "PWM edit crossed parts");
 std::cout<<"PASS OSC 1 PWM append, patch/host recall, schema18 migration, and selected-part editing\n";
}
