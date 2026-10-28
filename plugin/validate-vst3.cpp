#include <juce_audio_processors_headless/juce_audio_processors_headless.h>
#include <iostream>
#include <stdexcept>
void check(bool ok,const char*why){if(!ok)throw std::runtime_error(why);}
double energy(const juce::AudioBuffer<float>&a){double e=0;for(int ch=0;ch<a.getNumChannels();++ch)for(int i=0;i<a.getNumSamples();++i){const float v=a.getSample(ch,i);check(std::isfinite(v)&&std::abs(v)<1,"nonfinite/clipped audio");e+=v*v;}return e;}
int main(int argc,char**argv){try{
 if(argc!=2)throw std::runtime_error("Expected path to one VST3 bundle");
 juce::ScopedJuceInitialiser_GUI gui;juce::VST3PluginFormatHeadless format;juce::OwnedArray<juce::PluginDescription>types;
 const auto path=juce::File(juce::String::fromUTF8(argv[1])).getFullPathName();format.findAllTypesForFile(types,path);
 check(types.size()==1,"scan did not find exactly one instrument");check(types[0]->isInstrument,"plugin not marked as instrument");
 juce::String error;auto plugin=format.createInstanceFromDescription(*types[0],48000,512,error);
 if(!plugin)throw std::runtime_error(error.toStdString());
 plugin->setPlayConfigDetails(0,2,48000,512);plugin->prepareToPlay(48000,512);
 juce::AudioBuffer<float> audio(2,512);juce::MidiBuffer midi;
 midi.addEvent(juce::MidiMessage::noteOn(1,60,(juce::uint8)100),0);plugin->processBlock(audio,midi);check(energy(audio)>1e-6,"loaded VST3 silent");
 auto state=juce::MemoryBlock{};plugin->getStateInformation(state);check(state.getSize()>0,"empty VST3 state");plugin->setStateInformation(state.getData(),int(state.getSize()));
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
