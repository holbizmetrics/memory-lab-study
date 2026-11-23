#pragma once
#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <vector>

inline void lfoFadeAudioChecks(juce::AudioProcessor& processor){
 auto parameter=[&](const char* name){
  for(auto* p:processor.getParameters())if(p->getName(128)==name)return p;
  throw std::runtime_error(name);
 };
 parameter("Drive")->setValueNotifyingHost(0);
 for(const auto& item:{std::pair{"Envelope 3 delay (ms)","300"},
                      std::pair{"Envelope 3 attack (ms)","700"},
                      std::pair{"Envelope 3 sustain","1"},
                      std::pair{"Envelope 3 hold (ms)","0"},
                      std::pair{"Envelope 3 decay (ms)","0"},
                      std::pair{"LFO rate (Hz)","4"}}){
  auto* p=parameter(item.first);
  p->setValueNotifyingHost(p->getValueForText(item.second));
 }
 constexpr int blockSize=256;
 auto* fade=parameter("Part 1 ENV 3 to LFO 1 depth");
 auto* amp=parameter("LFO 1 amplitude depth");
 auto* pitch=parameter("Pitch LFO (cents)");
 auto* other=parameter("LFO 2 amplitude depth");
 auto capture=[&](float amount,float ampValue,float pitchValue,float otherAmp){
  fade->setValueNotifyingHost(amount);
  amp->setValueNotifyingHost(ampValue);
  pitch->setValueNotifyingHost(pitchValue);
  other->setValueNotifyingHost(otherAmp);
  processor.prepareToPlay(48000,blockSize);
  juce::AudioBuffer<float> buffer(2,blockSize);
  juce::MidiBuffer notes;
  for(int i=0;i<160;++i)processor.processBlock(buffer,notes);
  notes.addEvent(juce::MidiMessage::noteOn(1,57,.8f),0);
  std::vector<float> output;
  for(int i=0;i<300;++i){
   processor.processBlock(buffer,notes);
   notes.clear();
   output.insert(output.end(),buffer.getReadPointer(0),buffer.getReadPointer(0)+blockSize);
  }
  processor.releaseResources();
  return output;
 };
 auto distance=[](const auto& a,const auto& b,int begin,int end){
  float largest=0;
  for(int i=begin;i<end;++i)largest=std::max(largest,std::abs(a[i]-b[i]));
  return largest;
 };
 const auto dry=capture(0,.5f,.5f,.5f);
 const auto direct=capture(0,.9f,.5f,.5f);
 const auto faded=capture(1,.9f,.5f,.5f);
 check(distance(dry,direct,2000,10000)>.004f,"ordinary LFO 1 tremolo");
 check(distance(dry,faded,2000,10000)<1e-6f,"ENV 3 delay suppresses tremolo");
 check(distance(direct,faded,62000,74000)<1e-6f,"ENV 3 sustain reaches full tremolo");
 const auto half=capture(.5f,.9f,.5f,.5f);
 const auto fullDistance=distance(dry,direct,2000,10000);
 check(std::abs(distance(dry,half,2000,10000)-fullDistance*.5f)<1e-6f,"half fade retains half tremolo at onset");
 const auto vibrato=capture(1,.5f,1,.5f);
 check(distance(dry,vibrato,2000,10000)<1e-6f&&distance(dry,vibrato,55000,74000)>.004f,"ENV 3 fades in vibrato");
 check(capture(0,.5f,.5f,.9f)==capture(1,.5f,.5f,.9f),"ENV 3 fade affects only LFO 1");
}
