#pragma once
#include "Presets.hpp"
void presetChecks(){
 auto p=std::make_unique<MemoryLabProcessor>();
 set(*p,"volume",.137f);set(*p,"voiceDrive",8);set(*p,"drive",3);set(*p,"keyTrack",.7f);set(*p,"arpOn",1);
 const auto original=memorylab::patch::encode(*p);
 check(memorylab::presets::encode(*p,-1).isEmpty()&&!memorylab::presets::load(*p,99),"invalid preset accepted");
 check(memorylab::patch::encode(*p)==original,"invalid preset changed controls");
 for(int i=0;i<int(memorylab::presets::bank().size());++i){
  check(!memorylab::presets::encode(*p,i).isEmpty(),"preset encoding empty");
  check(memorylab::patch::encode(*p)==original,"building preset changed controls");
  check(memorylab::presets::load(*p,i),"preset failed full patch validation");
  check(p->getParameters().size()==103,"preset changed parameter count");
  check(std::abs(p->state.getRawParameterValue("volume")->load()-.137f)<1e-6,"preset changed output");
  check(p->state.getRawParameterValue("drive")->load()==0&&p->state.getRawParameterValue("voiceDrive")->load()==0,"preset drive not bypassed");
  auto fresh=std::make_unique<MemoryLabProcessor>();set(*fresh,"volume",.137f);check(memorylab::presets::load(*fresh,i),"fresh preset load");check(memorylab::patch::encode(*fresh)==memorylab::patch::encode(*p),"preset depends on previous sound");
  auto other=std::make_unique<MemoryLabProcessor>();check(memorylab::patch::apply(*other,memorylab::patch::encode(*p)).wasOk(),"preset patch roundtrip");
  check(memorylab::patch::encode(*other)==memorylab::patch::encode(*p),"preset patch values changed");
  juce::MemoryBlock state;p->getStateInformation(state);other->setStateInformation(state.getData(),int(state.getSize()));
  check(memorylab::patch::encode(*other)==memorylab::patch::encode(*p),"preset host recall changed values");
  check(memorylab::presets::undo(*p)&&memorylab::patch::encode(*p)==original,"preset undo did not restore complete sound");
  check(!memorylab::presets::undo(*p),"preset undo repeated");
 }
 check(memorylab::presets::load(*p,1),"preset load");set(*p,"cutoff",1234);check(!memorylab::presets::canUndo(*p),"edited preset retained undo");
 check(memorylab::presets::load(*p,2),"preset load");memorylab::patch::initialise(*p);check(!memorylab::presets::canUndo(*p),"INIT retained preset undo");
 set(*p,"volume",0);check(memorylab::presets::load(*p,3)&&p->state.getRawParameterValue("volume")->load()==0,"preset unmuted output");memorylab::patch::initialise(*p);
 // Browsing/category changes and page switches must not write sound parameters.
 auto editor=std::unique_ptr<juce::AudioProcessorEditor>(p->createEditor());
 auto*page=dynamic_cast<juce::Button*>(find(*editor,"presetPage"));check(page&&bool(page->onClick),"SOUNDS button missing");page->onClick();
 auto*category=dynamic_cast<juce::ComboBox*>(find(*editor,"presetCategory"));auto*sound=dynamic_cast<juce::ComboBox*>(find(*editor,"presetSound"));
 check(category&&sound,"preset browser controls missing");auto*autoLoad=dynamic_cast<juce::Button*>(find(*editor,"presetAutoLoad"));check(autoLoad,"auto-load missing");autoLoad->setToggleState(false,juce::dontSendNotification);const auto beforeBrowse=memorylab::patch::encode(*p);
 for(int id=1;id<=9;++id){category->setSelectedId(id,juce::dontSendNotification);category->onChange();check(sound->getNumItems()>0,"empty category");sound->onChange();check(memorylab::patch::encode(*p)==beforeBrowse,"browsing changed sound");}
 auto*load=dynamic_cast<juce::Button*>(find(*editor,"loadFactoryPreset"));auto*undo=dynamic_cast<juce::Button*>(find(*editor,"undoFactoryPreset"));check(load&&undo,"preset actions missing");
 load->onClick();check(p->state.getRawParameterValue("arpOn")->load()==1,"selected rhythm preset not loaded");undo->onClick();check(memorylab::patch::encode(*p)==beforeBrowse,"UI undo changed sound");editor.reset();
 // Eight simultaneous notes, including long swell attack, at common rates.
 double maxPeak=0;
 for(double sr:{44100.,48000.,96000.})for(int i=0;i<int(memorylab::presets::bank().size());++i){
  auto synth=std::make_unique<MemoryLabProcessor>();check(memorylab::presets::load(*synth,i),"render preset load");synth->prepareToPlay(sr,512);
  double sum=0;juce::AudioBuffer<float>audio(2,512);int block=0;
  for(int pos=0;pos<int(sr*5);pos+=512,++block){juce::MidiBuffer midi;
   if(block==0)for(int note:{48,55,60,63,67,72,75,79})midi.addEvent(juce::MidiMessage::noteOn(1,note,(juce::uint8)100),0);
   if(pos<=int(sr*4)&&pos+512>int(sr*4))for(int note:{48,55,60,63,67,72,75,79})midi.addEvent(juce::MidiMessage::noteOff(1,note),int(sr*4)-pos);
   synth->processBlock(audio,midi);sum+=power(audio);for(int n=0;n<audio.getNumSamples();++n)maxPeak=std::max(maxPeak,double(std::abs(audio.getSample(0,n))));
  }
  check(sum>1e-6,"preset render silent");synth->releaseResources();
 }
 std::cout<<"PASS starting presets: eight complete sounds, output preservation, muted/drive defaults, patch/host recall, invalid selection, one-step undo/edit invalidation, passive category browsing; 24 eight-note renders peak="<<maxPeak<<"\n";
}
