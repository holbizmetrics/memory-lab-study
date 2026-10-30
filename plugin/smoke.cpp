#include "Processor.hpp"
#include <iostream>
#include <stdexcept>
void check(bool ok,const char*why){if(!ok)throw std::runtime_error(why);}
void set(MemoryLabProcessor&p,const char*id,float value){auto*parameter=p.state.getParameter(id);check(parameter,"missing parameter");parameter->setValueNotifyingHost(parameter->convertTo0to1(value));}
double power(const juce::AudioBuffer<float>&b){double sum=0;for(int i=0;i<b.getNumSamples();++i){float v=b.getSample(0,i);check(std::isfinite(v)&&std::abs(v)<1,"bad audio");sum+=v*v;}return sum;}
juce::Component* find(juce::Component&root,const juce::String&id){if(root.getComponentID()==id)return &root;for(auto*child:root.getChildren())if(auto*match=find(*child,id))return match;return nullptr;}
#include "automation-checks.hpp"
#include "uia-lifecycle-checks.hpp"
#include "patch-checks.hpp"
#include "voice-drive-checks.hpp"
#include "oscillator-checks.hpp"
#include "noise-checks.hpp"
#include "filter-family-checks.hpp"
#include "tab-switch-checks.hpp"
struct TestPlayHead:juce::AudioPlayHead{
 double bpm=137.3;bool playing=true,tempo=true;
 juce::Optional<PositionInfo> getPosition()const override{PositionInfo p;p.setIsPlaying(playing);if(tempo)p.setBpm(bpm);return p;}
};
void arpWrapperTests(){
 TestPlayHead host;
 auto full=std::make_unique<MemoryLabProcessor>(),split=std::make_unique<MemoryLabProcessor>();
 full->setPlayHead(&host);split->setPlayHead(&host);
 for(auto*p:{full.get(),split.get()}){set(*p,"arpOn",1);set(*p,"arpOctaves",2);set(*p,"arpOrder",2);set(*p,"arpGate",37);p->prepareToPlay(48000,512);}
 constexpr int length=48000;juce::AudioBuffer<float>whole(2,length),part(2,127);juce::MidiBuffer events;
 for(int n:{60,64,67})events.addEvent(juce::MidiMessage::noteOn(1,n,(juce::uint8)100),50);
 for(int n:{60,64,67})events.addEvent(juce::MidiMessage::noteOff(1,n),30000);
 full->processBlock(whole,events);check(power(whole)>1e-6,"arp wrapper silent");
 for(int pos=0;pos<length;pos+=127){const int count=std::min(127,length-pos);part.setSize(2,count,false,false,true);juce::MidiBuffer local;
  for(int n:{60,64,67}){if(pos<=50&&50<pos+count)local.addEvent(juce::MidiMessage::noteOn(1,n,(juce::uint8)100),50-pos);if(pos<=30000&&30000<pos+count)local.addEvent(juce::MidiMessage::noteOff(1,n),30000-pos);}
  split->processBlock(part,local);for(int i=0;i<count;++i)check(part.getSample(0,i)==whole.getSample(0,pos+i),"arp wrapper block mismatch");
 }
 check(std::abs(full->arpTempo.load()-137.3f)<.01&&full->arpHasHostTempo.load(),"arp host BPM");
 host.tempo=false;set(*full,"arpBpm",140);events.clear();full->processBlock(part,events);check(full->arpTempo.load()==140&&!full->arpHasHostTempo.load(),"arp tempo fallback");
 host.tempo=true;set(*full,"arpSync",0);full->processBlock(part,events);check(full->arpTempo.load()==140,"arp internal clock ignored");
 set(*full,"arpSync",1);events.addEvent(juce::MidiMessage::noteOn(1,60,(juce::uint8)100),0);full->processBlock(part,events);
 host.playing=false;full->processBlock(part,events);for(int i=0;i<500;++i)full->processBlock(part,events);check(power(part)<1e-12,"arp transport stop stuck");
 host.playing=true;events.addEvent(juce::MidiMessage::noteOn(1,60,(juce::uint8)100),0);full->processBlock(part,events);set(*full,"arpOn",0);full->processBlock(part,events);check(power(part)==0,"arp disable stuck");
 juce::MemoryBlock data;set(*split,"arpGate",31);split->getStateInformation(data);full->setStateInformation(data.getData(),int(data.getSize()));check(full->state.getRawParameterValue("arpGate")->load()==31,"arp state restore");
 auto legacy=split->state.copyState();for(int i=legacy.getNumChildren()-1;i>=0;--i)if(legacy.getChild(i).getProperty("id").toString().startsWith("arp"))legacy.removeChild(i,nullptr);
 auto xml=legacy.createXml();juce::AudioProcessor::copyXmlToBinary(*xml,data);full->setStateInformation(data.getData(),int(data.getSize()));check(full->state.getRawParameterValue("arpOn")->load()==0,"legacy preset enabled arp");
 full->setPlayHead(nullptr);split->setPlayHead(nullptr);
 std::cout<<"PASS arp wrapper: audio block parity, host BPM/internal/fallback, stop/disable cleanup, state recall and legacy bypass\n";
}
int main(int argc,char**argv){try{
 if(argc==2&&std::string(argv[1])=="--tab-test"){juce::ScopedJuceInitialiser_GUI gui;tabSwitchChecks();return 0;}
 juce::ScopedJuceInitialiser_GUI gui;
#if JUCE_WINDOWS
 if(argc==2&&std::string(argv[1])=="--uia-lifecycle-test"){uiaLifecycleChecks();return 0;}
#endif
 if(argc==2&&std::string(argv[1])=="--automation-test"){automationChecks();return 0;}
 if(argc==2&&std::string(argv[1])=="--patch-test"){patchChecks();return 0;}
 if(argc==2){
  auto processor=std::make_unique<MemoryLabProcessor>();auto editor=std::unique_ptr<juce::AudioProcessorEditor>(processor->createEditor());
  const juce::File folder{juce::String::fromUTF8(argv[1])};folder.createDirectory();check(folder.isDirectory(),"snapshot folder");
  auto snapshot=[&](const char*name){auto image=editor->createComponentSnapshot(editor->getLocalBounds(),true,1);auto stream=folder.getChildFile(name).createOutputStream();check(bool(stream),"snapshot stream");stream->setPosition(0);stream->truncate();juce::PNGImageFormat png;check(png.writeImageToStream(image,*stream),"snapshot encoding");};
  snapshot("voice.png");
  auto*cutoff=dynamic_cast<juce::Slider*>(find(*editor,"cutoff"));check(cutoff,"cutoff control missing");cutoff->setValue(2000,juce::sendNotificationSync);check(std::abs(processor->state.getRawParameterValue("cutoff")->load()-2000)<1,"cutoff attachment");
  auto*motion=dynamic_cast<juce::Button*>(find(*editor,"motionPage"));check(motion&&bool(motion->onClick),"motion page missing");motion->onClick();snapshot("motion.png");
  auto*vibrato=dynamic_cast<juce::Slider*>(find(*editor,"vibrato"));check(vibrato,"vibrato control missing");vibrato->setValue(8,juce::sendNotificationSync);check(std::abs(processor->state.getRawParameterValue("vibrato")->load()-8)<.01,"vibrato attachment");
  auto*envelopes=dynamic_cast<juce::Button*>(find(*editor,"envelopePage"));check(envelopes&&bool(envelopes->onClick),"envelope page missing");envelopes->onClick();snapshot("amplitude.png");
  auto*attack=dynamic_cast<juce::Slider*>(find(*editor,"ampAttack"));check(attack,"amplitude attack control missing");attack->setValue(500,juce::sendNotificationSync);check(std::abs(processor->state.getRawParameterValue("ampAttack")->load()-500)<1,"amplitude attack attachment");snapshot("amplitude-edited.png");
  auto*filter=dynamic_cast<juce::Button*>(find(*editor,"filterEnvelopePage"));check(filter&&bool(filter->onClick),"filter envelope page missing");filter->onClick();snapshot("filter-envelope.png");
  auto*hold=dynamic_cast<juce::Slider*>(find(*editor,"fltHold"));check(hold,"filter hold control missing");hold->setValue(250,juce::sendNotificationSync);check(std::abs(processor->state.getRawParameterValue("fltHold")->load()-250)<1,"filter hold attachment");
  editor->setSize(832,592);snapshot("compact.png");
  auto*arpPage=dynamic_cast<juce::Button*>(find(*editor,"arpPage"));check(arpPage&&bool(arpPage->onClick),"arp page missing");arpPage->onClick();snapshot("arp-compact.png");
  auto*gate=dynamic_cast<juce::Slider*>(find(*editor,"arpGate"));check(gate,"arp gate missing");gate->setValue(30,juce::sendNotificationSync);check(processor->state.getRawParameterValue("arpGate")->load()==30,"arp gate attachment");
  auto*on=dynamic_cast<juce::Slider*>(find(*editor,"arpOn"));check(on,"arp on missing");on->setValue(1,juce::sendNotificationSync);
  processor->prepareToPlay(48000,512);for(int note:{60,63,67})processor->keyboard.noteOn(1,note,.8f);
  juce::AudioBuffer<float>arpAudio(2,512);juce::MidiBuffer arpMidi;double arpEnergy=0;
  for(int block=0;block<8;++block){processor->processBlock(arpAudio,arpMidi);arpEnergy+=power(arpAudio);}
  if(arpEnergy<=1e-6)throw std::runtime_error("arp screen keyboard silent: energy="+std::to_string(arpEnergy)+" on="+std::to_string(processor->state.getRawParameterValue("arpOn")->load())+" voices="+std::to_string(processor->voiceCount.load()));
  check(processor->arpStep.load()==0,"arp did not use prepared sample rate");
  editor->setSize(1040,740);snapshot("arp.png");
  auto*drivePage=dynamic_cast<juce::Button*>(find(*editor,"drivePage"));check(drivePage&&bool(drivePage->onClick),"drive page missing");drivePage->onClick();snapshot("drive.png");
  auto*voiceDrive=dynamic_cast<juce::Slider*>(find(*editor,"voiceDrive"));check(voiceDrive,"voice drive knob missing");voiceDrive->setValue(24,juce::sendNotificationSync);check(processor->state.getRawParameterValue("voiceDrive")->load()==24,"voice drive attachment");
  editor->setSize(832,592);snapshot("drive-compact.png");auto*oscPage=dynamic_cast<juce::Button*>(find(*editor,"oscPage"));check(oscPage&&bool(oscPage->onClick),"oscillator page missing");oscPage->onClick();snapshot("osc-compact.png");editor->setSize(1040,740);snapshot("osc.png");set(*processor,"noiseLevel",.35f);set(*processor,"sawLevel",0);set(*processor,"pulseLevel",0);snapshot("noise.png");editor->setSize(832,592);snapshot("noise-compact.png");auto*familyPage=dynamic_cast<juce::Button*>(find(*editor,"familyPage"));check(familyPage&&bool(familyPage->onClick),"family page missing");familyPage->onClick();editor->setSize(1040,740);snapshot("filter-classic.png");set(*processor,"filterFamily",1);snapshot("filter-lp4.png");set(*processor,"filterFamily",2);snapshot("filter-bp4.png");set(*processor,"filterFamily",3);snapshot("filter-blend.png");editor->setSize(832,592);snapshot("filter-compact.png");
  std::cout<<"PASS: Voice/Motion/Amplitude/Filter/ARP editor pages, parameter attachments, arp keyboard audio and compact snapshots; previews "<<folder.getFullPathName()<<"\n";return 0;
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
 // New envelope values survive state recall; legacy state resets absent stages.
 set(*p,"ampAttack",750);set(*p,"fltHold",250);p->getStateInformation(state);restored->setStateInformation(state.getData(),int(state.getSize()));
 check(std::abs(restored->state.getRawParameterValue("ampAttack")->load()-750)<1,"envelope state recall");
 check(std::abs(restored->state.getRawParameterValue("fltHold")->load()-250)<1,"filter hold state recall");
 auto legacy=p->state.copyState();for(int i=legacy.getNumChildren()-1;i>=0;--i){const auto id=legacy.getChild(i).getProperty("id").toString();if(id.startsWith("amp")||id.startsWith("flt"))legacy.removeChild(i,nullptr);}
 legacy.setProperty("version",1,nullptr);auto xml=legacy.createXml();juce::AudioProcessor::copyXmlToBinary(*xml,state);restored->setStateInformation(state.getData(),int(state.getSize()));
 check(std::abs(restored->state.getRawParameterValue("ampAttack")->load()-12)<.01,"legacy amp default migration");check(restored->state.getRawParameterValue("fltHold")->load()==0,"legacy filter default migration");
 check(std::abs(restored->state.getRawParameterValue("drive")->load()-9)<.01,"legacy old parameter lost");
 // Restore defaults before testing core mapping below.
 set(*p,"ampAttack",12);set(*p,"fltHold",0);
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
 arpWrapperTests();voiceDriveChecks();oscillatorChecks();noiseChecks();filterFamilyChecks();
 std::cout<<"PASS: JUCE/native default parity, MIDI sample offsets, stereo, channel CC120 isolation, sustain, bend/pressure mapping, state recall, MIDI block partitioning and on-screen keyboard audio/release\n";return 0;
 }catch(const std::exception&e){std::cerr<<"FAIL: "<<e.what()<<'\n';return 1;}}
