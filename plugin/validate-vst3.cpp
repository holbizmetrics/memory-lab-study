#include <juce_audio_processors_headless/juce_audio_processors_headless.h>
#include <iostream>
#include <stdexcept>
#include <fstream>
#include <set>
void check(bool ok,const char*why){if(!ok)throw std::runtime_error(why);}
double energy(const juce::AudioBuffer<float>&a){double e=0;for(int ch=0;ch<a.getNumChannels();++ch)for(int i=0;i<a.getNumSamples();++i){const float v=a.getSample(ch,i);check(std::isfinite(v)&&std::abs(v)<1,"nonfinite/clipped audio");e+=v*v;}return e;}
int capture(const char*bundle,const char*stem,const char*setting,const char*patchFile=nullptr){
 juce::ScopedJuceInitialiser_GUI gui;juce::VST3PluginFormatHeadless format;juce::OwnedArray<juce::PluginDescription>types;format.findAllTypesForFile(types,juce::File(juce::String::fromUTF8(bundle)).getFullPathName());check(types.size()==1,"capture scan");
 juce::String error;auto p=format.createInstanceFromDescription(*types[0],48000,512,error);if(!p)throw std::runtime_error(error.toStdString());p->setPlayConfigDetails(0,2,48000,512);p->prepareToPlay(48000,512);
 auto setNamed=[&](const char*name,float normalized){for(auto*parameter:p->getParameters())if(parameter->getName(128)==name){parameter->setValueNotifyingHost(normalized);return true;}return false;};
 const std::string mode(setting);check(mode=="default"||mode=="clean"||mode=="stress"||mode=="single","unknown capture setting");
 if(patchFile){
  auto patch=juce::parseXML(juce::File(juce::String::fromUTF8(patchFile)));
  check(patch&&patch->hasTagName("MemoryLabPatch")&&(patch->getIntAttribute("schemaVersion")==4||patch->getIntAttribute("schemaVersion")==5||patch->getIntAttribute("schemaVersion")==6||patch->getIntAttribute("schemaVersion")==7||patch->getIntAttribute("schemaVersion")==8||patch->getIntAttribute("schemaVersion")==9||patch->getIntAttribute("schemaVersion")==10||patch->getIntAttribute("schemaVersion")==11||patch->getIntAttribute("schemaVersion")==12||patch->getIntAttribute("schemaVersion")==13||patch->getIntAttribute("schemaVersion")==14||patch->getIntAttribute("schemaVersion")==15),"capture expects schema 4 through 15 patch");
  juce::MemoryBlock state;p->getStateInformation(state);auto outer=juce::AudioProcessor::getXmlFromBinary(state.getData(),int(state.getSize()));check(bool(outer),"capture host state XML");
  auto* component=outer->getChildByName("IComponent");check(component!=nullptr,"capture VST3 component state");
  juce::MemoryBlock componentState;check(componentState.fromBase64Encoding(component->getAllSubText()),"capture component decode");
  auto xml=juce::AudioProcessor::getXmlFromBinary(componentState.getData(),int(componentState.getSize()));check(bool(xml),"capture processor XML");
  int expected=0;for(auto*entry:xml->getChildIterator())if(entry->hasTagName("PARAM"))++expected;
  const int patchCount=patch->getIntAttribute("schemaVersion")==4?40:patch->getIntAttribute("schemaVersion")==5?41:patch->getIntAttribute("schemaVersion")==6?42:patch->getIntAttribute("schemaVersion")==7?46:patch->getIntAttribute("schemaVersion")==8?49:patch->getIntAttribute("schemaVersion")==9?58:patch->getIntAttribute("schemaVersion")==10?76:patch->getIntAttribute("schemaVersion")==11?92:patch->getIntAttribute("schemaVersion")==12?100:patch->getIntAttribute("schemaVersion")==13?103:patch->getIntAttribute("schemaVersion")==14?106:409;
  check((expected==409&&patchCount<=409)||(expected==106&&patchCount<=106)||(expected==103&&patchCount<=103)||(expected==100&&patchCount<=100)||(expected==92&&patchCount<=92)||expected==patchCount||(expected==42&&patchCount==41)||(expected==46&&(patchCount==41||patchCount==42))||(expected==49&&(patchCount==41||patchCount==42||patchCount==46))||(expected==58&&(patchCount==40||patchCount==41||patchCount==42||patchCount==46||patchCount==49))||(expected==76&&(patchCount==40||patchCount==41||patchCount==42||patchCount==46||patchCount==49||patchCount==58)),"capture patch schema does not match module controls");
  if(expected>=42&&patchCount==41)for(auto*entry:xml->getChildIterator())if(entry->getStringAttribute("id")=="lfoShape")entry->setAttribute("value",0.);
  int applied=0;std::set<juce::String>seen;for(auto*entry:patch->getChildIterator()){
   check(entry->hasTagName("PARAM")&&entry->hasAttribute("value")&&std::isfinite(entry->getDoubleAttribute("value"))&&seen.insert(entry->getStringAttribute("id")).second,"capture invalid or repeated control");
   bool found=false;for(auto*target:xml->getChildIterator())if(target->getStringAttribute("id")==entry->getStringAttribute("id")){target->setAttribute("value",entry->getDoubleAttribute("value"));found=true;++applied;break;}
   check(found,"capture unknown patch ID");
  }
  check(applied==patchCount,"capture incomplete patch");
  if(mode=="clean")for(auto*entry:xml->getChildIterator()){const auto id=entry->getStringAttribute("id");if(id=="drive"||id=="depth"||id=="voiceDrive")entry->setAttribute("value",0.);}
  juce::AudioProcessor::copyXmlToBinary(*xml,componentState);component->deleteAllTextElements();component->addTextElement(componentState.toBase64Encoding());
  juce::AudioProcessor::copyXmlToBinary(*outer,state);p->setStateInformation(state.getData(),int(state.getSize()));
  juce::MemoryBlock restored;p->getStateInformation(restored);auto restoredOuter=juce::AudioProcessor::getXmlFromBinary(restored.getData(),int(restored.getSize()));check(bool(restoredOuter),"capture restored host state");
  auto* restoredComponent=restoredOuter->getChildByName("IComponent");check(restoredComponent!=nullptr,"capture restored component");
  check(componentState.fromBase64Encoding(restoredComponent->getAllSubText()),"capture restored decode");
  auto actual=juce::AudioProcessor::getXmlFromBinary(componentState.getData(),int(componentState.getSize()));check(bool(actual),"capture restored processor state");
  for(auto*want:xml->getChildIterator()){bool matched=false;for(auto*got:actual->getChildIterator())if(got->getStringAttribute("id")==want->getStringAttribute("id")){matched=std::abs(got->getDoubleAttribute("value")-want->getDoubleAttribute("value"))<.001;break;}check(matched,"capture patch state mismatch");}
 }
 if(mode=="clean"&&!patchFile){check(setNamed("Drive",0),"capture drive missing");check(setNamed("Memory depth",0),"capture depth missing");setNamed("Voice drive (dB): zero bypasses",0);setNamed("White noise level",0);setNamed("Filter family: Classic / LP4 / BP4 / Blend",0);}
 juce::AudioBuffer<float>a(2,512);juce::MidiBuffer midi;for(int i=0;i<100;++i){midi.clear();p->processBlock(a,midi);}
 std::ofstream raw(std::string(stem)+".f32",std::ios::binary),settings(std::string(stem)+".txt");check(bool(raw)&&bool(settings),"capture open");settings<<"plugin="<<types[0]->version.toStdString()<<"\nsetting="<<mode<<'\n';for(auto*parameter:p->getParameters())settings<<parameter->getName(128).toStdString()<<'='<<parameter->getText(parameter->getValue(),128).toStdString()<<" normalized="<<parameter->getValue()<<'\n';
 const auto notes=mode=="stress"?std::vector<int>{48,50,52,53,55,57,59,60}:mode=="single"?std::vector<int>{52}:std::vector<int>{52,55,59};
 double sum=0,peak=0,processingMs=0,worstRatio=0;int overload=0;std::vector<double>heldTimes;
 for(int pos=0;pos<144000;pos+=512){const int count=std::min(512,144000-pos);a.setSize(2,count,false,false,true);midi.clear();for(int n:notes){if(pos<=4800&&4800<pos+count)midi.addEvent(juce::MidiMessage::noteOn(1,n,(juce::uint8)100),4800-pos);if(pos<=76800&&76800<pos+count)midi.addEvent(juce::MidiMessage::noteOff(1,n),76800-pos);}const double started=juce::Time::getMillisecondCounterHiRes();p->processBlock(a,midi);const double elapsed=juce::Time::getMillisecondCounterHiRes()-started;processingMs+=elapsed;worstRatio=std::max(worstRatio,elapsed/(count*1000./48000));
  if(pos>=9600&&pos<76800)heldTimes.push_back(elapsed*1000.);
  for(int i=0;i<count;++i){const float v=a.getSample(0,i);check(std::isfinite(v),"capture nonfinite");sum+=double(v)*v;peak=std::max(peak,std::abs(double(v)));overload+=std::abs(v)>=1;raw.write(reinterpret_cast<const char*>(&v),4);}
 }
 std::sort(heldTimes.begin(),heldTimes.end());check(!heldTimes.empty(),"capture timing window empty");
 check(bool(raw)&&bool(settings),"capture write");std::cout<<"CAPTURE version="<<types[0]->version<<" mode="<<mode<<" peak="<<peak<<" rms="<<std::sqrt(sum/144000)<<" samples_above_full_scale="<<overload<<" processing_ms="<<processingMs<<" worst_block_budget_ratio="<<worstRatio<<" held_median_us="<<heldTimes[heldTimes.size()/2]<<" held_p95_us="<<heldTimes[std::size_t((heldTimes.size()-1)*.95)]<<" held_blocks="<<heldTimes.size()<<'\n';p->releaseResources();return 0;
}
int main(int argc,char**argv){try{
 if(argc==6&&std::string(argv[1])=="--capture-patch")return capture(argv[2],argv[3],argv[5],argv[4]);
 if(argc==5&&std::string(argv[1])=="--capture")return capture(argv[2],argv[3],argv[4]);
 if(argc!=2)throw std::runtime_error("Expected path to one VST3 bundle");
 juce::ScopedJuceInitialiser_GUI gui;juce::VST3PluginFormatHeadless format;juce::OwnedArray<juce::PluginDescription>types;
 const auto path=juce::File(juce::String::fromUTF8(argv[1])).getFullPathName();format.findAllTypesForFile(types,path);
 check(types.size()==1,"scan did not find exactly one instrument");check(types[0]->isInstrument,"plugin not marked as instrument");
 juce::String error;auto plugin=format.createInstanceFromDescription(*types[0],48000,512,error);
 if(!plugin)throw std::runtime_error(error.toStdString());
 plugin->setPlayConfigDetails(0,2,48000,512);plugin->prepareToPlay(48000,512);
 juce::AudioBuffer<float> audio(2,512);juce::MidiBuffer midi;
 midi.addEvent(juce::MidiMessage::noteOn(1,60,(juce::uint8)100),0);plugin->processBlock(audio,midi);check(energy(audio)>1e-6,"loaded VST3 silent");
 juce::AudioProcessorParameter*voiceDrive=nullptr;
 for(auto*parameter:plugin->getParameters())if(parameter->getName(128)=="Voice drive (dB): zero bypasses")voiceDrive=parameter;
 check(voiceDrive&&voiceDrive->getValue()==0,"VST3 voice drive missing or enabled by default");
 juce::AudioProcessorParameter*oscInterval=nullptr;
 for(auto*parameter:plugin->getParameters())if(parameter->getName(128)=="Oscillator 2 interval (semitones)")oscInterval=parameter;
 check(oscInterval!=nullptr,"VST3 oscillator interval missing");oscInterval->setValueNotifyingHost(.75f);
 juce::AudioProcessorParameter*noiseLevel=nullptr;
 for(auto*parameter:plugin->getParameters())if(parameter->getName(128)=="White noise level")noiseLevel=parameter;
 check(noiseLevel&&noiseLevel->getValue()==0,"VST3 noise missing or enabled by default");noiseLevel->setValueNotifyingHost(.35f);
 juce::AudioProcessorParameter*family=nullptr;for(auto*parameter:plugin->getParameters())if(parameter->getName(128)=="Filter family: Classic / LP4 / BP4 / Blend")family=parameter;
 check(family&&family->getValue()==0,"VST3 filter family missing or not classic");family->setValueNotifyingHost(1);
 voiceDrive->setValueNotifyingHost(1);midi.clear();for(int i=0;i<20;++i)plugin->processBlock(audio,midi);check(energy(audio)>1e-6,"VST3 enabled voice drive silent");
 auto state=juce::MemoryBlock{};plugin->getStateInformation(state);check(state.getSize()>0,"empty VST3 state");plugin->setStateInformation(state.getData(),int(state.getSize()));
 check(voiceDrive->getValue()==1,"VST3 voice drive state lost");voiceDrive->setValueNotifyingHost(0);
 check(std::abs(oscInterval->getValue()-.75f)<1e-6,"VST3 oscillator interval state lost");
 check(std::abs(noiseLevel->getValue()-.35f)<1e-5,"VST3 noise state lost");
 check(family->getValue()==1,"VST3 filter family state lost");
 midi.clear();midi.addEvent(juce::MidiMessage::noteOff(1,60),0);plugin->processBlock(audio,midi);
 for(int i=0;i<140;++i){midi.clear();plugin->processBlock(audio,midi);}check(energy(audio)<1e-12,"VST3 stuck release");
 juce::AudioProcessorParameter*arpOn=nullptr;
 for(auto*parameter:plugin->getParameters())if(parameter->getName(128)=="Arpeggiator on")arpOn=parameter;
 check(arpOn!=nullptr,"VST3 arp parameter missing");arpOn->setValueNotifyingHost(1);
 plugin->getStateInformation(state);arpOn->setValueNotifyingHost(0);plugin->setStateInformation(state.getData(),int(state.getSize()));check(arpOn->getValue()>.5f,"VST3 arp state recall");
 midi.clear();for(int n:{60,64,67})midi.addEvent(juce::MidiMessage::noteOn(1,n,(juce::uint8)100),0);
 double arpEnergy=0;for(int i=0;i<90;++i){plugin->processBlock(audio,midi);arpEnergy+=energy(audio);midi.clear();}check(arpEnergy>1e-4,"loaded VST3 arp silent");
 midi.addEvent(juce::MidiMessage::controllerEvent(1,123,0),0);plugin->processBlock(audio,midi);
 for(int i=0;i<140;++i){midi.clear();plugin->processBlock(audio,midi);}check(energy(audio)<1e-12,"VST3 arp all-off stuck");
 plugin->releaseResources();plugin.reset();
 // A second instantiation catches basic factory/unload/reload failures.
 plugin=format.createInstanceFromDescription(*types[0],44100,127,error);check(bool(plugin),"VST3 reload failed");plugin->prepareToPlay(44100,127);plugin->releaseResources();plugin.reset();
 std::cout<<"PASS: actual VST3 scan, instrument classification, instantiate, MIDI audio, arp audio/state/all-off, state roundtrip, release silence and reload at44.1kHz\n";return 0;
 }catch(const std::exception&e){std::cerr<<"FAIL: "<<e.what()<<'\n';return 1;}}
