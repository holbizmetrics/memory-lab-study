#pragma once
#include <array>
#include <cmath>
#include <vector>

// Use the existing check() supplied by each test executable.
inline void mpePitchAudioChecks(juce::AudioProcessor& processor){
 auto parameter=[&](const char* name,float value){
  for(auto* p:processor.getParameters())if(p->getName(128)==name){p->setValueNotifyingHost(value);return;}
  check(false,name);
 };
 parameter("MPE lower-zone pitch",1);
 parameter("Part 1 unison voices",.5f);
 parameter("Part 1 unison detune (cents)",.48f);
 parameter("Drive",0);
 processor.prepareToPlay(48000,256);
 juce::AudioBuffer<float> buffer(2,256);
 juce::MidiBuffer midi;
 std::vector<float> samples;
 auto render=[&]{
  samples.clear();
  for(int block=0;block<160;++block){
   processor.processBlock(buffer,midi);
   midi.clear();
   if(block>=32)samples.insert(samples.end(),buffer.getReadPointer(0),buffer.getReadPointer(0)+256);
  }
 };
 auto magnitude=[&](double hz){
  constexpr double pi=3.14159265358979323846;
  double re=0,im=0;
  const auto n=samples.size();
  for(std::size_t i=0;i<n;++i){
   const auto frame=double(i);
   const double window=.5-.5*std::cos(2*pi*frame/double(n-1));
   const double phase=2*pi*hz*frame/48000;
   re+=samples[i]*window*std::cos(phase);
   im+=samples[i]*window*std::sin(phase);
  }
  return std::hypot(re,im)/double(n);
 };
 auto stack=[&](double centre){
  return std::min(magnitude(centre*std::exp2(-24./1200)),magnitude(centre*std::exp2(24./1200)));
 };
 constexpr double low=130.81278265,high=440.;
 // Initial bend on the member channel immediately precedes its note.
 midi.addEvent(juce::MidiMessage::pitchWheel(2,10240),1);
 midi.addEvent(juce::MidiMessage::noteOn(2,48,.8f),2);
 midi.addEvent(juce::MidiMessage::noteOn(3,69,.8f),2);
 render();
 check(stack(2*low)>1e-4&&stack(high)>1e-4&&stack(low)<stack(2*low)*.05,"MPE initial bend and neighbor isolation");
 midi.addEvent(juce::MidiMessage::pitchWheel(2,8192),5);
 render();
 check(stack(low)>1e-4&&stack(high)>1e-4,"MPE live bend center");
 midi.addEvent(juce::MidiMessage::pitchWheel(1,16383),7);
 render();
 const double global=std::exp2(2./12);
 check(stack(low*global)>1e-4&&stack(high*global)>1e-4,"MPE manager wheel");
 midi.addEvent(juce::MidiMessage::pitchWheel(1,8192),0);
 midi.addEvent(juce::MidiMessage::pitchWheel(2,10240),1);
 midi.addEvent(juce::MidiMessage::controllerEvent(1,64,127),2);
 render();
 midi.addEvent(juce::MidiMessage::noteOff(2,48),0);
 midi.addEvent(juce::MidiMessage::noteOn(2,55,.8f),1);
 render();
 const double reuse=196.;
 check(stack(2*low)>1e-4&&stack(reuse)>1e-4,"MPE held tail and neutral channel reuse");
 midi.addEvent(juce::MidiMessage::controllerEvent(1,64,0),0);
 render();
 render(); // Measure after the default 500 ms release has ended.
 check(stack(2*low)<1e-5&&stack(reuse)>1e-4&&stack(high)>1e-4,"MPE manager pedal release");
 midi.addEvent(juce::MidiMessage::controllerEvent(1,120,0),0);
 render();
 for(float sample:samples)check(sample==0,"MPE manager panic");
 parameter("Part 1 unison voices",1);
 midi.addEvent(juce::MidiMessage::pitchWheel(2,10240),0);
 midi.addEvent(juce::MidiMessage::noteOn(2,48,.8f),1);
 midi.addEvent(juce::MidiMessage::noteOn(3,57,.8f),2);
 midi.addEvent(juce::MidiMessage::noteOn(4,69,.8f),3);
 render();
 check(stack(220)>1e-4&&stack(high)>1e-4&&stack(2*low)<1e-5,"MPE whole-stack steal");
 midi.addEvent(juce::MidiMessage::noteOff(2,48),0);
 midi.addEvent(juce::MidiMessage::noteOn(2,55,.8f),1);
 render();
 check(stack(reuse)>1e-4&&stack(high)>1e-4,"MPE neutral channel after stealing");
 processor.releaseResources();
}
