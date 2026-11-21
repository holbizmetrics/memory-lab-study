#pragma once
void envelopeCharacterChecks(){
 using namespace memorylab;
 EnvelopeSettings legacy{.1,1,.2,1,.2,1},curved=legacy;curved.curve=1;
 check(envelopeLevel(.6,legacy)==.5,"legacy attack changed");
 const double attack=1.3*(1-std::exp(-std::log(1.3/.3)*.5));
 check(std::abs(envelopeLevel(.6,curved)-attack)<1e-13,"curved attack reference");
 const double decayTail=(std::exp(-2.5)-std::exp(-5.))/(1-std::exp(-5.));
 check(std::abs(envelopeLevel(1.8,curved)-(.2+.8*decayTail))<1e-13,"curved decay reference");
 check(std::abs(envelopeReleaseLevel(.7,.5,curved)-.7*decayTail)<1e-13,"curved release reference");
 check(envelopeReleaseLevel(.7,1,curved)==0,"curved release endpoint");
 for(int mode:{0,1})for(int stage=0;stage<3;++stage){double previous=stage==0?0:1;
  for(int i=0;i<=1000;++i){const double x=i/1000.;EnvelopeSettings s{0,1,0,1,0,1,mode,1};
   const double level=stage==0?envelopeLevel(x,s):stage==1?envelopeLevel(1+x,s):envelopeReleaseLevel(1,x,s);
   check(std::isfinite(level)&&level>=-1e-12&&level<=1+1e-12,"envelope curve range");
   check(stage==0?level+1e-12>=previous:level<=previous+1e-12,"envelope curve monotonicity");previous=level;
  }
 }
 auto fast=effectiveEnvelopeSettings({.001,.001,0,.001,.5,.001,1,.01});
 check(fast.delay==.0001&&fast.attack==.0001&&fast.hold==0&&fast.release==.0001,"fast timing limits/zero");
 auto slow=effectiveEnvelopeSettings({5,10,5,10,.5,10,1,20});
 check(slow.delay==100&&slow.attack==100&&slow.hold==100&&slow.decay==100&&slow.release==100,"long timing limit");
 // Sampling, note-off during attack, zero stages and immutable latched settings.
 for(int sr:{44100,48000,96000,192000}){
  for(int mode:{0,1}){EnvelopeSettings s{0,.01,0,.02,.3,.01,mode,.5};FilterEnvelope a,b;a.noteOn(s);b.noteOn(s);
   s.curve=1-mode;s.timeScale=20;for(int i=0;i<sr/500;++i)check(a.tick(sr)==b.tick(sr),"settings not latched");
   a.noteOff(sr);double last=1;for(int i=0;i<sr/50;++i){const double y=a.tick(sr);check(y>=0&&y<=last+1e-12,"release from current level");last=y;}check(last==0,"release did not end");
   FilterEnvelope immediate;immediate.noteOn({0,0,0,0,.4,0,mode,20});check(immediate.tick(sr)==.4,"zero stages");immediate.noteOff(sr);check(immediate.tick(sr)==0,"zero release");
  }
 }
 const char*ids[]{"ampCurve","ampTimeScale","fltCurve","fltTimeScale","env3Curve","env3TimeScale","env4Curve","env4TimeScale"};
 auto p=std::make_unique<MemoryLabProcessor>();check(p->getParameters().size()==501&&p->getTailLengthSeconds()>=100,"envelope count/tail");
 for(int part=0;part<4;++part)for(int i=0;i<8;++i){const auto id=MemoryLabProcessor::partId(part,ids[i]);auto*param=p->state.getParameter(id);check(param&&param==p->getParameters()[425+part*8+i]&&param->isAutomatable(),"envelope append order");
  check(std::abs(param->convertFrom0to1(param->getDefaultValue())-(i%2?1.f:0.f))<1e-6,"envelope default");set(*p,id.toRawUTF8(),i%2?2.f:1.f);
 }
 set(*p,"lfoUnipolar",1);
 const auto full=patch::encode(*p);juce::MemoryBlock host;p->getStateInformation(host);auto q=std::make_unique<MemoryLabProcessor>();q->setStateInformation(host.getData(),int(host.getSize()));check(patch::encode(*q)==full,"envelope host recall");
 auto old=juce::parseXML(full);old->setAttribute("schemaVersion",16);for(int i=old->getNumChildElements()-1;i>=0;--i){const auto id=old->getChildElement(i)->getStringAttribute("id");if((id.contains("filterMod")||(id.contains("osc1Pwm")||id.endsWith("postFilterAmp")))||id.endsWith("Curve")||id.endsWith("TimeScale"))old->removeChildElement(old->getChildElement(i),true);}
 check(patch::apply(*p,old->toString()).wasOk(),"schema16 migration");
 check(p->state.getRawParameterValue("lfoUnipolar")->load()==1,"schema16 lost polarity");
 for(int part=0;part<4;++part)for(int i=0;i<8;++i)check(std::abs(p->state.getRawParameterValue(MemoryLabProcessor::partId(part,ids[i]))->load()-(i%2?1.f:0.f))<1e-6,"legacy envelope retained option");
 check(patch::apply(*p,full).wasOk(),"envelope full patch restore");
 auto oldHost=q->state.copyState();for(int i=oldHost.getNumChildren()-1;i>=0;--i){const auto id=oldHost.getChild(i).getProperty("id").toString();if((id.contains("filterMod")||(id.contains("osc1Pwm")||id.endsWith("postFilterAmp")))||id.endsWith("Curve")||id.endsWith("TimeScale"))oldHost.removeChild(i,nullptr);}
 oldHost.setProperty("version",18,nullptr);auto oldXml=oldHost.createXml();juce::AudioProcessor::copyXmlToBinary(*oldXml,host);q->setStateInformation(host.getData(),int(host.getSize()));
 check(q->state.getRawParameterValue("ampCurve")->load()==0&&std::abs(q->state.getRawParameterValue("ampTimeScale")->load()-1)<1e-6&&q->state.getRawParameterValue("lfoUnipolar")->load()==1,"old host migration");
 p->copyPart(0,3);check(p->state.getRawParameterValue("part4__ampCurve")->load()==1,"copy envelope character");
 p->editingPart=3;check(presets::load(*p,0),"starter load");check(p->state.getRawParameterValue("part4__ampCurve")->load()==0&&p->state.getRawParameterValue("ampCurve")->load()==1,"starter reset/isolation");
 for(int sr:{44100,48000,96000,192000}){auto a=std::make_unique<MemoryLabProcessor>(),b=std::make_unique<MemoryLabProcessor>();integrationSetup(*a,0);integrationSetup(*b,0);
  for(int part=0;part<4;++part)for(int i=0;i<8;++i){const auto id=MemoryLabProcessor::partId(part,ids[i]);set(*a,id.toRawUTF8(),i%2?.5f:1.f);set(*b,id.toRawUTF8(),i%2?.5f:1.f);}
  check(integrationRender(*a,sr,127,0)==integrationRender(*b,sr,257,0),"curved multipart block parity");
 }
 auto editor=std::unique_ptr<juce::AudioProcessorEditor>(p->createEditor());p->editingPart=0;
 const char*pages[]{"ampEnvelopePage","filterEnvelopePage","env3Page","env4Page"};
 for(int source=0;source<4;++source){auto*page=dynamic_cast<juce::Button*>(find(*editor,pages[source]));check(page&&bool(page->onClick),"envelope page");page->onClick();for(int i=0;i<2;++i)check(dynamic_cast<juce::Slider*>(find(*editor,ids[source*2+i]))!=nullptr,"envelope option binding");}
 std::cout<<"PASS envelope character: stage reference curves, ranges/monotonicity, timing limits, four-rate release/zero/latching, 32 appended controls, migration/recall/copy/starter isolation and curved multipart block parity\n";
}
