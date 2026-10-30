#include <juce_audio_processors_headless/juce_audio_processors_headless.h>
#include <iostream>
#include <stdexcept>
#include <fstream>
void check(bool ok,const char*why){if(!ok)throw std::runtime_error(why);}
double energy(const juce::AudioBuffer<float>&a){double e=0;for(int ch=0;ch<a.getNumChannels();++ch)for(int i=0;i<a.getNumSamples();++i){const float v=a.getSample(ch,i);check(std::isfinite(v)&&std::abs(v)<1,"nonfinite/clipped audio");e+=v*v;}return e;}
int capture(const char*bundle,const char*stem,const char*setting){
 juce::ScopedJuceInitialiser_GUI gui;juce::VST3PluginFormatHeadless format;juce::OwnedArray<juce::PluginDescription>types;format.findAllTypesForFile(types,juce::File(juce::String::fromUTF8(bundle)).getFullPathName());check(types.size()==1,"capture scan");
 juce::String error;auto p=format.createInstanceFromDescription(*types[0],48000,512,error);if(!p)throw std::runtime_error(error.toStdString());p->setPlayConfigDetails(0,2,48000,512);p->prepareToPlay(48000,512);
 auto setNamed=[&](const char*name,float normalized){for(auto*parameter:p->getParameters())if(parameter->getName(128)==name){parameter->setValueNotifyingHost(normalized);return true;}return false;};
 const std::string mode(setting);check(mode=="default"||mode=="clean"||mode=="stress","unknown capture setting");
 if(mode=="clean"){check(setNamed("Drive",0),"capture drive missing");check(setNamed("Memory depth",0),"capture depth missing");setNamed("Voice drive (dB): zero bypasses",0);setNamed("White noise level",0);setNamed("Filter family: Classic / LP4 / BP4 / Blend",0);}
 juce::AudioBuffer<float>a(2,512);juce::MidiBuffer midi;for(int i=0;i<100;++i){midi.clear();p->processBlock(a,midi);}
 std::ofstream raw(std::string(stem)+".f32",std::ios::binary),settings(std::string(stem)+".txt");check(bool(raw)&&bool(settings),"capture open");settings<<"plugin="<<types[0]->version.toStdString()<<"\nsetting="<<mode<<'\n';for(auto*parameter:p->getParameters())settings<<parameter->getName(128).toStdString()<<'='<<parameter->getText(parameter->getValue(),128).toStdString()<<" normalized="<<parameter->getValue()<<'\n';
 const auto notes=mode=="stress"?std::vector<int>{48,50,52,53,55,57,59,60}:std::vector<int>{52,55,59};
 double sum=0,peak=0,processingMs=0,worstRatio=0;int overload=0;
 for(int pos=0;pos<144000;pos+=512){const int count=std::min(512,144000-pos);a.setSize(2,count,false,false,true);midi.clear();for(int n:notes){if(pos<=4800&&4800<pos+count)midi.addEvent(juce::MidiMessage::noteOn(1,n,(juce::uint8)100),4800-pos);if(pos<=76800&&76800<pos+count)midi.addEvent(juce::MidiMessage::noteOff(1,n),76800-pos);}const double started=juce::Time::getMillisecondCounterHiRes();p->processBlock(a,midi);const double elapsed=juce::Time::getMillisecondCounterHiRes()-started;processingMs+=elapsed;worstRatio=std::max(worstRatio,elapsed/(count*1000./48000));
  for(int i=0;i<count;++i){const float v=a.getSample(0,i);check(std::isfinite(v),"capture nonfinite");sum+=double(v)*v;peak=std::max(peak,std::abs(double(v)));overload+=std::abs(v)>=1;raw.write(reinterpret_cast<const char*>(&v),4);}
 }
 check(bool(raw)&&bool(settings),"capture write");std::cout<<"CAPTURE version="<<types[0]->version<<" mode="<<mode<<" peak="<<peak<<" rms="<<std::sqrt(sum/144000)<<" samples_above_full_scale="<<overload<<" processing_ms="<<processingMs<<" worst_block_budget_ratio="<<worstRatio<<'\n';p->releaseResources();return 0;
}
int main(int argc,char**argv){try{
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
