#pragma once
#include <numeric>
#include <vector>

inline void mpeCutoffAudioChecks(juce::AudioProcessor& processor,bool timbre){
 auto parameter=[&](const char* name){
  for(auto* value:processor.getParameters())if(value->getName(128)==name)return value;
  throw std::runtime_error(name);
 };
 parameter("Drive")->setValueNotifyingHost(0);
 auto* cutoff=parameter("Filter cutoff (Hz)");
 cutoff->setValueNotifyingHost(cutoff->getValueForText("180"));
 parameter("Part 1 unison voices")->setValueNotifyingHost(.5f);
 auto* q=parameter("Filter Q");
 q->setValueNotifyingHost(q->getValueForText("0.7"));
 auto* mode=parameter("MPE lower-zone pitch");
 auto* depth=parameter(timbre?"Part 1 MPE timbre to cutoff (octaves)":"Part 1 MPE pressure to cutoff (octaves)");
 const int neutralValue=timbre?64:0;
 juce::AudioBuffer<float> buffer(2,256);
 juce::MidiBuffer midi;
 constexpr int note=60;
 auto process=[&](int blocks){
  std::vector<float> last(256);
  for(int i=0;i<blocks;++i){processor.processBlock(buffer,midi);midi.clear();}
  std::copy_n(buffer.getReadPointer(0),256,last.begin());
  return last;
 };
 auto prepare=[&](bool mpe,float amount){
  mode->setValueNotifyingHost(mpe?1.f:0.f);
  depth->setValueNotifyingHost(amount);
  processor.prepareToPlay(48000,256);
  process(64);
 };
 auto expression=[&](int channel,int value){
  const auto message=timbre?juce::MidiMessage::controllerEvent(channel,74,value)
                           :juce::MidiMessage::channelPressureChange(channel,value);
  midi.addEvent(message,0);
 };
 auto noteOn=[&](int channel){midi.addEvent(juce::MidiMessage::noteOn(channel,note,.8f),1);};
 auto held=[&](int noteChannel,int pressureChannel,int pressure,bool mpe,float amount,bool initial=false){
  prepare(mpe,amount);
  if(initial)expression(pressureChannel,pressure);
  noteOn(noteChannel);
  process(64);
  if(!initial)expression(pressureChannel,pressure);
  auto out=process(64);
  processor.releaseResources();
  return out;
 };
 auto energy=[](const auto& samples){return std::inner_product(samples.begin(),samples.end(),samples.begin(),0.);};
 const auto neutral=held(2,2,neutralValue,true,1);
 const auto raised=held(2,2,127,true,1);
 if(timbre){
  check(held(2,2,64,true,1,true)==neutral,"CC 74 center is neutral");
  check(energy(held(2,2,0,true,1))<energy(neutral)*.8,"CC 74 below center closes cutoff");
 }
 check(energy(raised)>energy(neutral)*1.3,"MPE cutoff expression opens cutoff on the held stack");
 check(energy(held(2,2,127,true,0))<energy(neutral)*.8,"negative MPE cutoff expression closes cutoff");
 check(held(2,2,127,true,.5f)==neutral,"zero MPE cutoff expression depth");
 check(held(2,2,127,false,1)==neutral,"MPE cutoff expression is opt-in");
 const auto other=held(3,2,neutralValue,true,1);
 check(held(3,2,127,true,1)==other,"pressure reached a different member channel");
 check(held(3,1,127,true,1)==other,"manager pressure reached the dedicated member route");
 check(energy(held(2,2,127,true,1,true))>energy(neutral)*1.3,"pre-note member pressure");

 auto sustained=[&](bool clear){
  prepare(true,1);
  expression(2,127);noteOn(2);process(64);
  midi.addEvent(juce::MidiMessage::controllerEvent(1,64,127),0);process(1);
  midi.addEvent(juce::MidiMessage::noteOff(2,note),0);process(1);
  if(clear)expression(2,neutralValue);
  auto tail=process(64);
  processor.releaseResources();
  return tail;
 };
 check(sustained(true)==sustained(false),"member pressure must stay latched after key-up");

 auto reuse=[&](int pressure){
  prepare(true,1);
  expression(2,pressure);noteOn(2);process(64);
  midi.addEvent(juce::MidiMessage::noteOff(2,note),0);process(160);
  noteOn(2);auto out=process(64);
  processor.releaseResources();
  return out;
 };
 check(reuse(127)==reuse(neutralValue),"new note inherited pressure from a previous note");
}
