#include "Processor.hpp"
#include <iostream>
#include <stdexcept>
void check(bool ok,const char*why){if(!ok)throw std::runtime_error(why);}
void set(MemoryLabProcessor&p,const char*id,float value){auto*parameter=p.state.getParameter(id);check(parameter,"missing parameter");parameter->setValueNotifyingHost(parameter->convertTo0to1(value));}
double power(const juce::AudioBuffer<float>&b){double sum=0;for(int i=0;i<b.getNumSamples();++i){float v=b.getSample(0,i);check(std::isfinite(v)&&std::abs(v)<1,"bad audio");sum+=v*v;}return sum;}
juce::Component* find(juce::Component&root,const juce::String&id){if(root.getComponentID()==id)return &root;for(auto*child:root.getChildren())if(auto*match=find(*child,id))return match;return nullptr;}
int main(int argc,char**argv){try{
 juce::ScopedJuceInitialiser_GUI gui;
 if(argc==2){
  auto processor=std::make_unique<MemoryLabProcessor>();auto editor=std::unique_ptr<juce::AudioProcessorEditor>(processor->createEditor());
  const juce::File folder{juce::String::fromUTF8(argv[1])};folder.createDirectory();check(folder.isDirectory(),"snapshot folder");
  auto snapshot=[&](const char*name){auto image=editor->createComponentSnapshot(editor->getLocalBounds(),true,1);auto stream=folder.getChildFile(name).createOutputStream();check(bool(stream),"snapshot stream");juce::PNGImageFormat png;check(png.writeImageToStream(image,*stream),"snapshot encoding");};
  snapshot("voice.png");
  auto*cutoff=dynamic_cast<juce::Slider*>(find(*editor,"cutoff"));check(cutoff,"cutoff control missing");cutoff->setValue(2000,juce::sendNotificationSync);check(std::abs(processor->state.getRawParameterValue("cutoff")->load()-2000)<1,"cutoff attachment");
  auto*motion=dynamic_cast<juce::Button*>(find(*editor,"motionPage"));check(motion&&bool(motion->onClick),"motion page missing");motion->onClick();snapshot("motion.png");
  auto*vibrato=dynamic_cast<juce::Slider*>(find(*editor,"vibrato"));check(vibrato,"vibrato control missing");vibrato->setValue(8,juce::sendNotificationSync);check(std::abs(processor->state.getRawParameterValue("vibrato")->load()-8)<.01,"vibrato attachment");
  editor->setSize(832,592);snapshot("compact.png");
  std::cout<<"PASS: Voice/Motion editor construction, parameter attachments and compact snapshot; previews "<<folder.getFullPathName()<<"\n";return 0;
 }
 auto p=std::make_unique<MemoryLabProcessor>();p->setPlayConfigDetails(0,2,48000,512);p->prepareToPlay(48000,512);
 juce::AudioBuffer<float>a(2,512);juce::MidiBuffer midi;
 midi.addEvent(juce::MidiMessage::noteOn(1,60,(juce::uint8)100),127);p->processBlock(a,midi);
 check(power(a)>1e-6,"note silent");for(int i=0;i<127;++i)check(a.getSample(0,i)==0,"note started early");
 for(int i=0;i<512;++i)check(a.getSample(0,i)==a.getSample(1,i),"stereo mismatch");check(midi.isEmpty(),"MIDI leaked out");
 // Exact parity with the native default voice and corresponding sample event.
 auto native=std::make_unique<memorylab::Synth>();native->prepare(48000);float reference[512]{};
 memorylab::Event note{127,memorylab::EventKind::NoteOn,60,100/127.};native->render(reference,512,&note,1);
 for(int i=0;i<512;++i)check(reference[i]==a.getSample(0,i),"wrapper default differs from native");
 // Channel 2 notes survive CC120 (all sound off) on channel 1.
 p->prepareToPlay(48000,512);midi.addEvent(juce::MidiMessage::noteOn(2,60,(juce::uint8)100),0);midi.addEvent(juce::MidiMessage::controllerEvent(1,120,0),200);p->processBlock(a,midi);check(power(a)>1e-6,"CC120 leaked across channels");
 midi.addEvent(juce::MidiMessage::controllerEvent(2,120,0),0);p->processBlock(a,midi);check(power(a)==0,"CC120 failed silence");
 // Sustain note-off mapping and pedal release.
 p->prepareToPlay(48000,512);midi.addEvent(juce::MidiMessage::noteOn(1,60,(juce::uint8)100),0);midi.addEvent(juce::MidiMessage::controllerEvent(1,64,127),100);midi.addEvent(juce::MidiMessage::noteOff(1,60),200);p->processBlock(a,midi);
 for(int i=0;i<80;++i)p->processBlock(a,midi);check(power(a)>1e-6,"sustain did not hold");
 midi.addEvent(juce::MidiMessage::controllerEvent(1,64,0),0);p->processBlock(a,midi);for(int i=0;i<120;++i)p->processBlock(a,midi);check(power(a)<1e-12,"sustain release stuck");
 // Full state serialization; no factory-preset requirement for this first build.
 set(*p,"drive",9);set(*p,"profile",.5f);set(*p,"pressure",1);juce::MemoryBlock state;p->getStateInformation(state);
 auto restored=std::make_unique<MemoryLabProcessor>();restored->setStateInformation(state.getData(),int(state.getSize()));
 check(std::abs(restored->state.getRawParameterValue("drive")->load()-9)<1e-5,"drive state restore");check(std::abs(restored->state.getRawParameterValue("profile")->load()-.5f)<1e-5,"profile state restore");
 check(restored->state.getRawParameterValue("pressure")->load()==1,"pressure state restore");
 // Raw controller mapping compared sample-for-sample with the same core events.
 for(int type=0;type<2;++type){p->prepareToPlay(48000,512);set(*p,"drive",5);set(*p,"profile",1);set(*p,"pressure",type?1.f:0.f);
  native->reset();native->dispatch({0,memorylab::EventKind::Parameter,0,type?1.:0.,0,memorylab::Param::PressureDepth});
  midi.addEvent(juce::MidiMessage::noteOn(1,60,(juce::uint8)100),0);
  if(type)midi.addEvent(juce::MidiMessage::channelPressureChange(1,127),128);else midi.addEvent(juce::MidiMessage::pitchWheel(1,16383),128);
  p->processBlock(a,midi);memorylab::Event events[]={{0,memorylab::EventKind::NoteOn,60,100/127.},{128,type?memorylab::EventKind::ChannelPressure:memorylab::EventKind::PitchBend,0,1}};
  native->render(reference,512,events,2);for(int i=0;i<512;++i)check(reference[i]==a.getSample(0,i),"controller mapping parity");
 }
 // Block-size independence through the JUCE MIDI adapter.
 auto full=std::make_unique<MemoryLabProcessor>(),split=std::make_unique<MemoryLabProcessor>();full->prepareToPlay(48000,4096);split->prepareToPlay(48000,127);
 juce::AudioBuffer<float>large(2,4096),chunk(2,127);juce::MidiBuffer events;events.addEvent(juce::MidiMessage::noteOn(1,60,(juce::uint8)100),50);events.addEvent(juce::MidiMessage::pitchWheel(1,16383),500);events.addEvent(juce::MidiMessage::noteOff(1,60),2000);full->processBlock(large,events);
 for(int pos=0;pos<4096;pos+=127){int n=std::min(127,4096-pos);chunk.setSize(2,n,false,false,true);juce::MidiBuffer local;
  if(pos<=50&&50<pos+n)local.addEvent(juce::MidiMessage::noteOn(1,60,(juce::uint8)100),50-pos);
  if(pos<=500&&500<pos+n)local.addEvent(juce::MidiMessage::pitchWheel(1,16383),500-pos);
  if(pos<=2000&&2000<pos+n)local.addEvent(juce::MidiMessage::noteOff(1,60),2000-pos);
  split->processBlock(chunk,local);for(int i=0;i<n;++i)check(chunk.getSample(0,i)==large.getSample(0,pos+i),"wrapper block mismatch");
 }
 auto screen=std::make_unique<MemoryLabProcessor>();screen->prepareToPlay(48000,512);screen->keyboard.noteOn(1,60,.8f);double screenEnergy=0;
 for(int i=0;i<4;++i){midi.clear();screen->processBlock(a,midi);screenEnergy+=power(a);}check(screenEnergy>1e-6,"on-screen keyboard silent");screen->keyboard.noteOff(1,60,0);
 for(int i=0;i<140;++i){midi.clear();screen->processBlock(a,midi);}check(power(a)<1e-12,"on-screen keyboard stuck note");
 std::cout<<"PASS: JUCE/native default parity, MIDI sample offsets, stereo, channel CC120 isolation, sustain, bend/pressure mapping, state recall, MIDI block partitioning and on-screen keyboard audio/release\n";return 0;
 }catch(const std::exception&e){std::cerr<<"FAIL: "<<e.what()<<'\n';return 1;}}
