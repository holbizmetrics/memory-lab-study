#pragma once

void unisonChecks(){
 auto p=std::make_unique<MemoryLabProcessor>();
 auto q=std::make_unique<MemoryLabProcessor>();
 check(p->getParameters().size()==530,"unison count");
 for(int part=0;part<4;++part){
  const auto modeId=MemoryLabProcessor::partId(part,"unisonSize");
  const auto detuneId=MemoryLabProcessor::partId(part,"unisonDetune");
  auto* mode=p->state.getParameter(modeId);
  auto* detune=p->state.getParameter(detuneId);
  check(mode&&mode==p->getParameters()[505+2*part]&&mode->getDefaultValue()==0,"unison append order");
  check(detune&&detune==p->getParameters()[506+2*part]&&
        std::abs(detune->convertFrom0to1(detune->getDefaultValue())-12)<1e-5,"detune append order");
  check(mode->getText(.5f,8)=="2"&&mode->getText(1,8)=="4"&&mode->getValueForText("4")==1,"mode text mapping");
  set(*p,modeId.toRawUTF8(),2);
  set(*p,detuneId.toRawUTF8(),18.f+part);
 }
 set(*p,"stereoSpread",.75f);
 const auto encoded=memorylab::patch::encode(*p);
 check(memorylab::patch::apply(*q,encoded).wasOk()&&memorylab::patch::encode(*q)==encoded,"unison patch recall");
 juce::MemoryBlock host;
 p->getStateInformation(host);
 q->setStateInformation(host.getData(),int(host.getSize()));
 check(memorylab::patch::encode(*q)==encoded,"unison host recall");
 auto older=juce::parseXML(encoded);
 older->setAttribute("schemaVersion",21);
 for(int i=older->getNumChildElements()-1;i>=0;--i){
  auto* entry=older->getChildElement(i);
  const auto id=entry->getStringAttribute("id");
  if(id.endsWith("unisonSize")||(id.endsWith("unisonDetune")||(id=="mpePitch"||(id.endsWith("mpePressureDepth")||(id.endsWith("mpeTimbreDepth")||(id.endsWith("env3Lfo1Depth")||id.endsWith("extendedLowPass")))))))older->removeChildElement(entry,true);
 }
 check(memorylab::patch::apply(*q,older->toString()).wasOk(),"schema21 migration");
 for(int part=0;part<4;++part){
  check(q->state.getRawParameterValue(MemoryLabProcessor::partId(part,"unisonSize"))->load()==0,"legacy mode");
  check(std::abs(q->state.getRawParameterValue(MemoryLabProcessor::partId(part,"unisonDetune"))->load()-12)<1e-5,"legacy detune");
 }
 check(q->state.getRawParameterValue("stereoSpread")->load()==.75f,"legacy spread was lost");

 q->editingPart=2;
 auto editor=std::unique_ptr<juce::AudioProcessorEditor>(q->createEditor());
 auto* expression=dynamic_cast<juce::Button*>(find(*editor,"expressionPage"));
 check(expression&&expression->onClick,"expression navigation");
 expression->onClick();
 auto* knob=dynamic_cast<juce::Slider*>(find(*editor,"part3__unisonSize"));
 check(knob&&knob->getTextFromValue(2)=="4","unison editor display");
 knob->setValue(2,juce::sendNotificationSync);
 check(q->state.getRawParameterValue("part3__unisonSize")->load()==2&&
       q->state.getRawParameterValue("unisonSize")->load()==0,"unison edit crossed parts");

 q->prepareToPlay(48000,128);
 juce::AudioBuffer<float> audio(2,128);
 for(int mode=0;mode<3;++mode){
  set(*q,"unisonSize",float(mode));
  juce::MidiBuffer midi;
  midi.addEvent(juce::MidiMessage::noteOn(1,60,.8f),0);
  q->processBlock(audio,midi);
  check(q->voiceCount.load()==(1<<mode),"processor stack size");
 }
 std::cout<<"PASS unison parameter order, migration, editor and live MIDI processing\n";
}
