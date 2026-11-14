#pragma once
// Source polarity is distinct from signed route depth and amplitude attenuation.
void polarityChecks(){
 using namespace memorylab;
 for(int shape=0;shape<4;++shape)for(int i=0;i<=1000;++i){
  const double w=lfoSample(i/1000.,shape),u=lfoPolarity(w,1);
  check(lfoPolarity(w,0)==w,"bipolar math changed");
  check(u>=-1e-14&&u<=1+1e-14&&std::abs(u-(w+1)*.5)<1e-14,"unipolar waveform range/law");
 }
 auto p=std::make_unique<MemoryLabProcessor>();
 const char*ids[]{"lfoUnipolar","lfo2Unipolar","lfo3Unipolar","lfo4Unipolar"};
 check(p->getParameters().size()==425,"polarity host count");
 for(int part=0;part<4;++part)for(int i=0;i<4;++i){const auto id=MemoryLabProcessor::partId(part,ids[i]);auto*param=p->state.getParameter(id);
  check(param&&param==p->getParameters()[409+part*4+i]&&param->getDefaultValue()==0&&param->isAutomatable(),"polarity append/default/order");
  set(*p,id.toRawUTF8(),1);
 }
 const auto full=patch::encode(*p);auto old=juce::parseXML(full);old->setAttribute("schemaVersion",15);
 for(int i=old->getNumChildElements()-1;i>=0;--i)if(old->getChildElement(i)->getStringAttribute("id").endsWith("Unipolar"))old->removeChildElement(old->getChildElement(i),true);
 check(patch::apply(*p,old->toString()).wasOk(),"schema15 polarity migration");
 for(int part=0;part<4;++part)for(const char*id:ids)check(p->state.getRawParameterValue(MemoryLabProcessor::partId(part,id))->load()==0,"old patch retained polarity");
 check(patch::apply(*p,full).wasOk(),"polarity patch restore");
 juce::MemoryBlock state;p->getStateInformation(state);auto q=std::make_unique<MemoryLabProcessor>();q->setStateInformation(state.getData(),int(state.getSize()));check(patch::encode(*q)==full,"polarity host recall");
 auto legacy=p->state.copyState();for(int i=legacy.getNumChildren()-1;i>=0;--i)if(legacy.getChild(i).getProperty("id").toString().endsWith("Unipolar"))legacy.removeChild(i,nullptr);
 legacy.setProperty("version",17,nullptr);auto xml=legacy.createXml();juce::AudioProcessor::copyXmlToBinary(*xml,state);p->setStateInformation(state.getData(),int(state.getSize()));
 for(int part=0;part<4;++part)for(const char*id:ids)check(p->state.getRawParameterValue(MemoryLabProcessor::partId(part,id))->load()==0,"old host retained polarity");
 set(*p,ids[0],1);p->copyPart(0,2);check(p->state.getRawParameterValue("part3__lfoUnipolar")->load()==1,"copy polarity");
 p->editingPart=2;check(patch::apply(*p,presets::encode(*p,0)).wasOk(),"starter polarity reset");check(p->state.getRawParameterValue("part3__lfoUnipolar")->load()==0&&p->state.getRawParameterValue(ids[0])->load()==1,"selected starter polarity isolation");
 // Four-rate combined contexts include all routes and multipart smoothing.
 for(int sr:{44100,48000,96000,192000}){
  auto a=std::make_unique<MemoryLabProcessor>(),b=std::make_unique<MemoryLabProcessor>();integrationSetup(*a,0);integrationSetup(*b,0);
  for(int part=0;part<4;++part)for(const char*id:ids){const auto fullId=MemoryLabProcessor::partId(part,id);set(*a,fullId.toRawUTF8(),1);set(*b,fullId.toRawUTF8(),1);}
  check(integrationRender(*a,sr,127,0,true)==integrationRender(*b,sr,257,0,true),"unipolar held-edit multipart block parity");
 }
 // AMP DEPTH maps the full source cycle to attenuation in both polarity modes.
 // This avoids a new half-depth/DC attenuation surprise when only polarity flips.
 auto render=[&](int source,bool uni,bool amp){auto t=std::make_unique<MemoryLabProcessor>();set(*t,"drive",0);set(*t,"volume",.03f);set(*t,ids[source],uni?1.f:0.f);
  const juce::String prefix=source==0?"lfo":"lfo"+juce::String(source+1);
  const auto route=amp?prefix+"Amp":source==0?juce::String("filterLfo"):prefix+"Cutoff";set(*t,route.toRawUTF8(),.8f);
  t->prepareToPlay(48000,256);juce::AudioBuffer<float>audio(2,256);juce::MidiBuffer midi;std::vector<float>out;
  midi.addEvent(juce::MidiMessage::noteOn(1,60,(juce::uint8)100),0);
  for(int block=0;block<240;++block){t->processBlock(audio,midi);power(audio);out.insert(out.end(),audio.getReadPointer(0),audio.getReadPointer(0)+256);}return out;};
 for(int source=0;source<4;++source){check(render(source,false,false)!=render(source,true,false),"polarity had no cutoff effect");check(render(source,false,true)==render(source,true,true),"polarity changed full-cycle tremolo");}
 p->editingPart=0;auto editor=std::unique_ptr<juce::AudioProcessorEditor>(p->createEditor());
 for(int source=0;source<4;++source){auto*button=dynamic_cast<juce::Button*>(find(*editor,source==0?"motionClockPage":"motion"+juce::String(source+1)+"ClockPage"));check(button&&bool(button->onClick),"clock page");button->onClick();check(find(*editor,ids[source])!=nullptr,"polarity knob binding missing");}
 std::cout<<"PASS polarity: wave laws/ranges, 16 appended per-part IDs, schema15/state17 migration, recall/copy/starter isolation, four-rate multipart parity, four source cutoff effects, exact tremolo invariance and clock bindings\n";
}
