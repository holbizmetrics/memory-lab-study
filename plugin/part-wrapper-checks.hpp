#pragma once
void partWrapperChecks(){
 using namespace memorylab;
 auto p=std::make_unique<MemoryLabProcessor>();
 check(p->getParameters().size()==425,"part parameter count");
 check(p->state.getRawParameterValue("partMode")->load()==0,"default not single");
 set(*p,"cutoff",300);set(*p,"part2__cutoff",4800);set(*p,"part2__osc2Interval",12);set(*p,"partMode",2);set(*p,"splitNote",60);set(*p,"part2Channel",2);
 const auto encoded=patch::encode(*p);auto old=juce::parseXML(encoded);old->setAttribute("schemaVersion",14);stripParts(*old);
 check(patch::apply(*p,old->toString()).wasOk(),"schema14 part migration");
 check(p->state.getRawParameterValue("partMode")->load()==0&&p->state.getRawParameterValue("part2__cutoff")->load()==1450,"old patch retained parts");
 check(patch::apply(*p,encoded).wasOk(),"multipart patch reload");
 juce::MemoryBlock host;p->getStateInformation(host);auto restored=std::make_unique<MemoryLabProcessor>();restored->setStateInformation(host.getData(),int(host.getSize()));check(patch::encode(*restored)==encoded,"multipart host roundtrip");
 auto legacy=p->state.copyState();for(int i=legacy.getNumChildren()-1;i>=0;--i){const auto id=legacy.getChild(i).getProperty("id").toString();if(id.startsWith("part")||id=="splitNote")legacy.removeChild(i,nullptr);}
 legacy.setProperty("version",16,nullptr);auto xml=legacy.createXml();juce::AudioProcessor::copyXmlToBinary(*xml,host);p->setStateInformation(host.getData(),int(host.getSize()));check(p->state.getRawParameterValue("partMode")->load()==0&&p->state.getRawParameterValue("part4__cutoff")->load()==1450,"old host did not default parts");
 p->copyPart(0,1);check(p->state.getRawParameterValue("part2__cutoff")->load()==p->state.getRawParameterValue("cutoff")->load(),"copy part sound");
 p->editingPart=1;set(*p,"partMode",3);set(*p,"part3__cutoff",820);set(*p,"arpBpm",111);
 const float first=p->state.getRawParameterValue("cutoff")->load(),third=p->state.getRawParameterValue("part3__cutoff")->load();
 check(presets::load(*p,0),"part2 starting sound");
 check(p->state.getRawParameterValue("cutoff")->load()==first&&p->state.getRawParameterValue("part3__cutoff")->load()==third&&p->state.getRawParameterValue("partMode")->load()==3&&p->state.getRawParameterValue("arpBpm")->load()==111,"starting sound changed other part/routing/arp");
 check(p->state.getRawParameterValue("part2__cutoff")->load()==1450&&p->state.getRawParameterValue("part2__drive")->load()==0,"part2 starting sound missing");
 const auto beforeDice=patch::encode(*p);check(randomizer::roll(*p,19),"part2 dice");
 check(p->state.getRawParameterValue("cutoff")->load()==first&&p->state.getRawParameterValue("part3__cutoff")->load()==third,"dice changed unselected part");
 check(randomizer::undo(*p)&&patch::encode(*p)==beforeDice,"part2 dice undo");
 auto editor=std::unique_ptr<juce::AudioProcessorEditor>(p->createEditor());
 auto click=[&](const char*id){auto*b=dynamic_cast<juce::Button*>(find(*editor,id));check(b&&bool(b->onClick),"part button missing");b->onClick();};
 const auto beforeBrowse=patch::encode(*p);click("partsPage");click("editPart3");check(p->editingPart.load()==2,"edit part selection");
 auto*cutoff=dynamic_cast<juce::Slider*>(find(*editor,"part3__cutoff"));check(cutoff,"part3 knob binding missing");cutoff->setValue(2000,juce::sendNotificationSync);check(std::abs(p->state.getRawParameterValue("part3__cutoff")->load()-2000)<1&&p->state.getRawParameterValue("cutoff")->load()==first,"part knob wrote wrong part");
 click("partsPage");click("editPart1");check(find(*editor,"cutoff"),"original knobs did not rebind");
 const auto selection=patch::encode(*p);for(int i=0;i<8;++i){click("partsPage");click(i%2?"editPart4":"editPart2");}
 check(patch::encode(*p)==selection,"part navigation changed sound");
 click("initPatch");check(p->editingPart.load()==0&&p->state.getRawParameterValue("partMode")->load()==0,"INIT did not reset parts/selection");
 auto factory=std::make_unique<MemoryLabProcessor>();check(patch::encode(*p)==patch::encode(*factory),"INIT not complete");
 double peak=0;
 for(double sr:{44100.,48000.,96000.,192000.}){
  auto play=std::make_unique<MemoryLabProcessor>();play->prepareToPlay(sr,257);
  set(*play,"partMode",3);
  for(int part=0;part<4;++part){set(*play,MemoryLabProcessor::partId(part,"drive").toRawUTF8(),0);set(*play,MemoryLabProcessor::partId(part,"volume").toRawUTF8(),.025f);set(*play,MemoryLabProcessor::partId(part,"cutoff").toRawUTF8(),float(300+part*800));}
  juce::AudioBuffer<float>a(2,257);juce::MidiBuffer m;for(int note:{48,52,55})m.addEvent(juce::MidiMessage::noteOn(1,note,(juce::uint8)100),13);
  play->processBlock(a,m);check(play->voiceCount.load()==8,"wrapper global eight-voice cap");
  double energy=0;for(int b=0;b<100;++b){play->processBlock(a,m);energy+=power(a);peak=std::max(peak,double(a.getMagnitude(0,0,a.getNumSamples())));}
  check(energy>1e-6,"multipart wrapper silent");
  set(*play,"partMode",2);set(*play,"splitNote",60);set(*play,"part1Channel",1);set(*play,"part2Channel",2);
  play->processBlock(a,m);check(play->voiceCount.load()==0,"routing change stranded held notes");
  m.addEvent(juce::MidiMessage::noteOn(1,48,(juce::uint8)100),0);m.addEvent(juce::MidiMessage::noteOn(2,72,(juce::uint8)100),0);play->processBlock(a,m);check(play->voiceCount.load()==2,"split/channel wrapper routing");
  m.addEvent(juce::MidiMessage::allSoundOff(1),0);play->processBlock(a,m);check(play->voiceCount.load()==1,"part channel panic isolation");
  m.addEvent(juce::MidiMessage::allSoundOff(2),0);play->processBlock(a,m);check(play->voiceCount.load()==0,"part panic did not clear");
  set(*play,"partMode",0);play->releaseResources();play->prepareToPlay(sr,257);m.addEvent(juce::MidiMessage::noteOn(1,48,(juce::uint8)100),0);play->processBlock(a,m);check(play->voiceCount.load()==1,"prepare retained old allocation");
 }
 std::cout<<"PASS part wrapper: 425 controls, old/new patch and host migration, selected-part preset/dice/undo/copy/edit isolation, passive navigation, complete INIT, four-rate layers/splits/channel panic/prepare; peak="<<peak<<"\n";
}

