#pragma once
#include "Randomizer.hpp"

void randomizerChecks(){
 auto p=std::make_unique<MemoryLabProcessor>();
 set(*p,"volume",.17f);set(*p,"drive",7);set(*p,"depth",.3f);set(*p,"recovery",180);set(*p,"voiceDrive",6);set(*p,"arpOn",1);set(*p,"arpBpm",99);set(*p,"lfoBpm",111);set(*p,"phaseReset",0);
 set(*p,"keyTrack",.5f);set(*p,"ampVelocity",.4f);set(*p,"filterVelocity",.6f);
 set(*p,"lfo2Pitch",7);set(*p,"lfo2Cutoff",.3f);set(*p,"lfo2Sync",1);set(*p,"lfo2Bpm",99);
 set(*p,"lfo3Pitch",9);set(*p,"lfo3Cutoff",.25f);set(*p,"lfo3Bpm",103);set(*p,"lfo4Pwm",.07f);set(*p,"lfo4Sync",1);set(*p,"lfo4Bpm",109);
 set(*p,"env3Pitch",123);set(*p,"env4Cutoff",.75f);set(*p,"env3Attack",1200);
 set(*p,"lfoAmp",.4f);set(*p,"env3Pwm",.1f);set(*p,"ampPitch",123);
 const auto original=memorylab::patch::encode(*p);
 // Every lock combination: no writes to locked groups or protected controls.
 const std::array<std::vector<juce::String>,4>groups{{
  {"sawLevel","pulseLevel","noiseLevel","width","detune","osc2Interval"},
  {"filterFamily","profile","resonance","cutoff"},
  {"ampDelay","ampAttack","ampHold","ampDecay","ampSustain","ampRelease","fltDelay","fltAttack","fltHold","fltDecay","fltSustain","fltRelease","filterEnv"},
  {"lfoShape","lfoRate","vibrato","pwm","filterLfo","pressure","lfoSync","lfoDivision","lfoRetrigger"}
 }};
 struct Monitor:juce::AudioProcessorParameter::Listener{
  int writes=0;void parameterValueChanged(int,float)override{++writes;}void parameterGestureChanged(int,bool)override{}
 }monitor;
 for(bool bypass:{false,true})for(int mask=0;mask<16;++mask){
  check(memorylab::patch::apply(*p,original).wasOk(),"randomizer fixture restore");
  p->randomizerSession.bypassDrives=bypass;
  std::vector<juce::AudioProcessorParameter*>protectedParams;std::vector<float>protectedValues;
  for(int group=0;group<4;++group)p->randomizerSession.locked[std::size_t(group)]=(mask&(1<<group))!=0;
  for(auto*base:p->getParameters()){
   auto*parameter=dynamic_cast<juce::RangedAudioParameter*>(base);check(parameter,"randomizer parameter type");bool protectedValue=true;
   for(int group=0;group<4;++group)if(std::find(groups[std::size_t(group)].begin(),groups[std::size_t(group)].end(),parameter->paramID)!=groups[std::size_t(group)].end())protectedValue=p->randomizerSession.locked[std::size_t(group)];
   if(bypass&&(parameter->paramID=="drive"||parameter->paramID=="voiceDrive"))protectedValue=false;
   if(protectedValue){protectedParams.push_back(base);protectedValues.push_back(base->getValue());base->addListener(&monitor);}
  }
  monitor.writes=0;const bool rolled=memorylab::randomizer::roll(*p,100+unsigned(mask));
  for(std::size_t i=0;i<protectedParams.size();++i){protectedParams[i]->removeListener(&monitor);check(protectedParams[i]->getValue()==protectedValues[i],"roll changed locked/protected parameter");}
  check(monitor.writes==0,"roll notified a locked/protected parameter");check(rolled==(mask!=15||bypass),"all-locked roll handling");
  if(bypass)check(p->state.getRawParameterValue("drive")->load()==0&&p->state.getRawParameterValue("voiceDrive")->load()==0,"bypass roll did not zero both drives");
  if(rolled){check(memorylab::patch::encode(*p)!=original,"roll did not change sound");check(memorylab::randomizer::undo(*p),"roll undo unavailable");check(memorylab::patch::encode(*p)==original,"undo did not restore full previous patch");check(!memorylab::randomizer::undo(*p),"undo repeated");}
 }
 p->randomizerSession.bypassDrives=false;p->randomizerSession.locked.fill(false);auto second=std::make_unique<MemoryLabProcessor>();
 check(memorylab::patch::apply(*second,original).wasOk(),"seeded second fixture restore");
 memorylab::randomizer::roll(*p,12345);memorylab::randomizer::roll(*second,12345);check(memorylab::patch::encode(*p)==memorylab::patch::encode(*second),"seeded roll not reproducible");
 auto saved=memorylab::patch::encode(*p);check(memorylab::patch::apply(*second,saved).wasOk(),"rolled patch roundtrip rejected");
 memorylab::randomizer::roll(*p,54321);check(memorylab::randomizer::undo(*p)&&memorylab::patch::encode(*p)==saved,"undo did not restore immediately preceding roll");
 memorylab::randomizer::roll(*p,13579);
 set(*p,"cutoff",777);check(!memorylab::randomizer::undo(*p),"undo overwrote subsequent manual edit");check(p->state.getRawParameterValue("cutoff")->load()==777,"failed undo changed edits");
 // UI controls, per-instance locks/history across editor reopen, and no editor writes.
 auto editor=std::unique_ptr<juce::AudioProcessorEditor>(p->createEditor());
 auto*dice=dynamic_cast<juce::Button*>(find(*editor,"dicePage"));check(dice&&bool(dice->onClick),"dice page missing");dice->onClick();
 auto*lock=dynamic_cast<juce::ToggleButton*>(find(*editor,"lockOscillators"));check(lock&&bool(lock->onClick),"dice lock missing");lock->setToggleState(true,juce::dontSendNotification);lock->onClick();
 check(p->randomizerSession.locked[0],"lock did not reach session");
 auto*roll=dynamic_cast<juce::Button*>(find(*editor,"rollPatch"));check(roll&&bool(roll->onClick),"roll button missing");roll->onClick();const auto rolled=memorylab::patch::encode(*p);
 editor.reset();editor.reset(p->createEditor());check(memorylab::patch::encode(*p)==rolled,"editor reopen changed patch");
 lock=dynamic_cast<juce::ToggleButton*>(find(*editor,"lockOscillators"));check(lock&&lock->getToggleState(),"lock lost across editor reopen");check(memorylab::randomizer::canUndo(*p),"undo lost across editor reopen");
 for(const char*id:{"lockFilter","lockEnvelopes","lockMotion"}){auto*b=dynamic_cast<juce::ToggleButton*>(find(*editor,id));check(b&&bool(b->onClick),"section lock missing");b->setToggleState(true,juce::dontSendNotification);b->onClick();}
 roll=dynamic_cast<juce::Button*>(find(*editor,"rollPatch"));check(roll&&!roll->isEnabled(),"all-locked roll button active");
 check(memorylab::randomizer::canUndo(*p)&&!memorylab::randomizer::roll(*p,99)&&memorylab::randomizer::canUndo(*p),"all-locked no-op discarded undo history");
 // Checking is passive. With all sections locked, a bypass roll changes only drives.
 auto*bypass=dynamic_cast<juce::ToggleButton*>(find(*editor,"diceBypassDrives"));check(bypass&&bool(bypass->onClick)&&!bypass->getToggleState(),"bypass checkbox default");
 const auto beforeBypass=memorylab::patch::encode(*p);std::vector<float>beforeValues;for(auto*base:p->getParameters())beforeValues.push_back(base->getValue());
 bypass->setToggleState(true,juce::dontSendNotification);bypass->onClick();check(memorylab::patch::encode(*p)==beforeBypass,"checkbox changed audio before a roll");check(roll->isEnabled(),"all-locked bypass roll not enabled");roll->onClick();
 for(int i=0;i<p->getParameters().size();++i){auto*parameter=dynamic_cast<juce::RangedAudioParameter*>(p->getParameters()[i]);check(parameter,"bypass parameter type");if(parameter->paramID=="drive"||parameter->paramID=="voiceDrive")check(parameter->getValue()==0,"bypass button did not zero drive");else check(parameter->getValue()==beforeValues[std::size_t(i)],"all-locked bypass changed another control");}
 check(!roll->isEnabled()&&!memorylab::randomizer::roll(*p,99)&&memorylab::randomizer::canUndo(*p),"already bypassed all-locked roll should be a no-op");
 check(memorylab::randomizer::undo(*p)&&memorylab::patch::encode(*p)==beforeBypass,"undo did not restore drives and prior patch");
 editor.reset();editor.reset(p->createEditor());bypass=dynamic_cast<juce::ToggleButton*>(find(*editor,"diceBypassDrives"));check(bypass&&bypass->getToggleState(),"bypass choice lost across editor reopen");
 // Render an ensemble of seeded patches with drive bypassed: finite, audible fixtures.
 auto audioProcessor=std::make_unique<MemoryLabProcessor>();audioProcessor->prepareToPlay(48000,512);set(*audioProcessor,"drive",0);set(*audioProcessor,"depth",0);
 double maxPeak=0;
 for(unsigned seed=0;seed<32;++seed){
  memorylab::randomizer::roll(*audioProcessor,seed);juce::AudioBuffer<float>a(2,512);juce::MidiBuffer midi;
  for(int channel=1;channel<=16;++channel)midi.addEvent(juce::MidiMessage::controllerEvent(channel,120,0),0);
  double energy=0;
  for(int block=0;block<64;++block){if(block==0)for(int note:{48,50,52,53,55,57,59,60})midi.addEvent(juce::MidiMessage::noteOn(1,note,(juce::uint8)100),0);audioProcessor->processBlock(a,midi);
   for(int i=0;i<512;++i){const float v=a.getSample(0,i);check(std::isfinite(v),"random patch nonfinite audio");energy+=v*v;maxPeak=std::max(maxPeak,double(std::abs(v)));}
  }check(energy>1e-6,"unlocked random patch silent");
 }
 std::cout<<"PASS randomizer: 32 lock/bypass combinations, protected no-writes, passive checkbox and drive-only roll, drive undo/no-op/history, seeded repeatability, schema roundtrip, editor session persistence, 32 eight-voice renders; fixture peak="<<maxPeak<<" (not a universal loudness guarantee)\n";
}
