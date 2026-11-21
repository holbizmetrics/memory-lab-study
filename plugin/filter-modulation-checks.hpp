#pragma once
void filterModulationChecks(){
 using namespace memorylab;
 const double base=std::nextafter(.3,1.);
 check(filterRouteValue(base,0,0,.8,-.6,0,1)==base,"zero route arithmetic changed");
 check(std::abs(filterRouteValue(.25,.4,-.2,.5,-.75,0,1)-.6)<1e-14,"route sum/sign reference");
 check(filterRouteValue(.8,1,1,1,1,0,1)==1&&filterRouteValue(.2,-1,1,1,-1,0,1)==0,"profile clamps");
 check(filterRouteValue(2,7.5,7.5,1,1,.5,8)==8&&filterRouteValue(2,-7.5,7.5,1,-1,.5,8)==.5,"resonance clamps");
 const char*ids[]{"filterModEnvProfile","filterModLfoProfile","filterModEnvRes","filterModLfoRes"};
 auto p=std::make_unique<MemoryLabProcessor>();check(p->getParameters().size()==514,"filter route parameter count");
 for(int part=0;part<4;++part)for(int i=0;i<4;++i){auto*param=p->state.getParameter(MemoryLabProcessor::partId(part,ids[i]));check(param&&param==p->getParameters()[457+part*4+i]&&param->isAutomatable()&&std::abs(param->convertFrom0to1(param->getDefaultValue()))<1e-6,"filter append/default/order");set(*p,MemoryLabProcessor::partId(part,ids[i]).toRawUTF8(),i<2?.4f:2.f);}
 set(*p,"ampCurve",1);set(*p,"lfoUnipolar",1);const auto full=patch::encode(*p);
 juce::MemoryBlock host;p->getStateInformation(host);auto q=std::make_unique<MemoryLabProcessor>();q->setStateInformation(host.getData(),int(host.getSize()));check(patch::encode(*q)==full,"filter routes host recall");
 auto old=juce::parseXML(full);old->setAttribute("schemaVersion",17);for(int i=old->getNumChildElements()-1;i>=0;--i)if((old->getChildElement(i)->getStringAttribute("id").contains("filterMod")||(old->getChildElement(i)->getStringAttribute("id").contains("osc1Pwm")||(old->getChildElement(i)->getStringAttribute("id").endsWith("postFilterAmp")||(old->getChildElement(i)->getStringAttribute("id").endsWith("stereoSpread")||old->getChildElement(i)->getStringAttribute("id").endsWith("unisonSize")||(old->getChildElement(i)->getStringAttribute("id").endsWith("unisonDetune")||old->getChildElement(i)->getStringAttribute("id")=="mpePitch"))))))old->removeChildElement(old->getChildElement(i),true);
 check(patch::apply(*p,old->toString()).wasOk(),"schema17 migration");
 for(int part=0;part<4;++part)for(const char*id:ids)check(std::abs(p->state.getRawParameterValue(MemoryLabProcessor::partId(part,id))->load())<1e-6,"old patch retained route");
 check(p->state.getRawParameterValue("ampCurve")->load()==1&&p->state.getRawParameterValue("lfoUnipolar")->load()==1,"old patch lost existing options");
 check(patch::apply(*p,full).wasOk(),"new route patch recall");
 auto oldHost=q->state.copyState();for(int i=oldHost.getNumChildren()-1;i>=0;--i)if((oldHost.getChild(i).getProperty("id").toString().contains("filterMod")||(oldHost.getChild(i).getProperty("id").toString().contains("osc1Pwm")||(oldHost.getChild(i).getProperty("id").toString().endsWith("postFilterAmp")||(oldHost.getChild(i).getProperty("id").toString().endsWith("stereoSpread")||oldHost.getChild(i).getProperty("id").toString().endsWith("unisonSize")||(oldHost.getChild(i).getProperty("id").toString().endsWith("unisonDetune")||oldHost.getChild(i).getProperty("id").toString()=="mpePitch"))))))oldHost.removeChild(i,nullptr);
 oldHost.setProperty("version",19,nullptr);auto xml=oldHost.createXml();juce::AudioProcessor::copyXmlToBinary(*xml,host);q->setStateInformation(host.getData(),int(host.getSize()));check(std::abs(q->state.getRawParameterValue(ids[0])->load())<1e-6&&q->state.getRawParameterValue("ampCurve")->load()==1,"old host route migration");
 p->copyPart(0,2);check(std::abs(p->state.getRawParameterValue("part3__filterModEnvProfile")->load()-.4f)<1e-6,"copy routes");
 p->editingPart=2;check(presets::load(*p,0),"route starter reset");check(std::abs(p->state.getRawParameterValue("part3__filterModEnvProfile")->load())<1e-6&&std::abs(p->state.getRawParameterValue(ids[0])->load()-.4f)<1e-6,"starter part isolation");
 p->editingPart=0;check(randomizer::roll(*p,23),"dice failed");check(std::abs(p->state.getRawParameterValue(ids[0])->load()-.4f)<1e-6,"dice changed route scope");
 auto render=[&](int family,int route,float depth){auto a=std::make_unique<MemoryLabProcessor>();set(*a,"drive",0);set(*a,"volume",.005f);set(*a,"cutoff",1400);set(*a,"resonance",1);set(*a,"profile",.25f);set(*a,"modernProfile",.2f);set(*a,"filterFamily",float(std::min(family,3)));set(*a,"modernEnabled",family==4?1.f:0.f);set(*a,"env3Attack",120);set(*a,"env3Decay",250);set(*a,"env3Sustain",.7f);set(*a,"lfoRate",1);if(route>=0)set(*a,ids[route],depth);
  a->prepareToPlay(48000,256);juce::AudioBuffer<float>audio(2,256);juce::MidiBuffer midi;std::vector<float>out;
  for(int block=0;block<220;++block){if(block==24)midi.addEvent(juce::MidiMessage::noteOn(1,60,(juce::uint8)100),0);a->processBlock(audio,midi);power(audio);out.insert(out.end(),audio.getReadPointer(0),audio.getReadPointer(0)+256);}return out;};
 for(int family:{0,4}){const auto baseline=render(family,-1,0);for(int route=0;route<4;++route)check(render(family,route,route<2?.5f:3.f)!=baseline,"isolated filter route had no effect");}
 for(int family:{1,2,3}){const auto baseline=render(family,-1,0);for(int route:{0,1})check(render(family,route,.5f)==baseline,"profile affected unsupported legacy family");check(render(family,2,3)!=baseline,"Q route missing in legacy family");}
 for(int sr:{44100,48000,96000,192000}){auto a=std::make_unique<MemoryLabProcessor>(),b=std::make_unique<MemoryLabProcessor>();integrationSetup(*a,0);integrationSetup(*b,0);
  for(auto*processor:{a.get(),b.get()})for(int part=0;part<4;++part){set(*processor,MemoryLabProcessor::partId(part,"volume").toRawUTF8(),.0005f);set(*processor,MemoryLabProcessor::partId(part,"lfoShape").toRawUTF8(),3);set(*processor,MemoryLabProcessor::partId(part,"env3Curve").toRawUTF8(),1);for(int i=0;i<4;++i)set(*processor,MemoryLabProcessor::partId(part,ids[i]).toRawUTF8(),i<2?.5f:3.f);}
  check(integrationRender(*a,sr,127,0,false,true)==integrationRender(*b,sr,257,0,false,true),"filter route held-edit multipart block parity");
 }
 p->editingPart=0;auto editor=std::unique_ptr<juce::AudioProcessorEditor>(p->createEditor());auto*page=dynamic_cast<juce::Button*>(find(*editor,"filterModRoutesPage"));check(page&&bool(page->onClick),"filter routes page missing");
 set(*p,"modernEnabled",0);set(*p,"filterFamily",1);const auto before=patch::encode(*p);page->onClick();check(patch::encode(*p)==before,"route navigation edited sound");
 auto*profile=dynamic_cast<juce::Slider*>(find(*editor,ids[0]));auto*res=dynamic_cast<juce::Slider*>(find(*editor,ids[2]));check(profile&&!profile->isEnabled()&&res&&res->isEnabled(),"legacy route enabled-state");
 set(*p,"modernEnabled",1);page->onClick();check(profile->isEnabled(),"modern profile route disabled");
 p->editingPart=2;page->onClick();auto*partKnob=dynamic_cast<juce::Slider*>(find(*editor,"part3__filterModLfoProfile"));check(partKnob,"selected-part filter route knob");partKnob->setValue(.2,juce::sendNotificationSync);check(std::abs(p->state.getRawParameterValue("part3__filterModLfoProfile")->load()-.2f)<1e-6&&std::abs(p->state.getRawParameterValue("filterModLfoProfile")->load()-.4f)<1e-6,"selected-part route edit isolation");
 std::cout<<"PASS filter modulation: signed/clamped math, append-only IDs, schema17/state19 migration, recall/copy/starter/dice isolation, Classic/Modern source routes, family bypass, four-rate held edits and UI states\n";
}
