#pragma once
#include <juce_audio_utils/juce_audio_utils.h>
#include "../native/voice.hpp"
#include "../native/arpeggiator.hpp"
#include <array>
#include <limits>

class MemoryLabProcessor final:public juce::AudioProcessor {
public:
 MemoryLabProcessor();
 void prepareToPlay(double,int)override;
 void releaseResources()override{}
 void processBlock(juce::AudioBuffer<float>&,juce::MidiBuffer&)override;
 bool isBusesLayoutSupported(const BusesLayout&)const override;
 const juce::String getName()const override{return "Memory Lab Study";}
 bool acceptsMidi()const override{return true;}
 bool producesMidi()const override{return false;}
 double getTailLengthSeconds()const override{return 11;}
 int getNumPrograms()override{return 1;}
 int getCurrentProgram()override{return 0;}
 void setCurrentProgram(int)override{}
 const juce::String getProgramName(int)override{return "Study baseline";}
 void changeProgramName(int,const juce::String&)override{}
 bool hasEditor()const override{return true;}
 juce::AudioProcessorEditor* createEditor()override;
 void getStateInformation(juce::MemoryBlock&)override;
 void setStateInformation(const void*,int)override;
 juce::AudioProcessorValueTreeState state;
 juce::MidiKeyboardState keyboard;
 std::atomic<float> outputPeak{0};
 std::atomic<int> voiceCount{0};
 std::atomic<float> arpTempo{130};
 std::atomic<int> arpStep{0};
 std::atomic<bool> arpHasHostTempo{false};
private:
 static juce::AudioProcessorValueTreeState::ParameterLayout layout();
 void midi(const juce::uint8*,int);
 memorylab::Synth synth;
 memorylab::Arpeggiator arp;
 std::array<std::atomic<float>*,7> arpValues{};
 bool arpEnabled=false,wasPlaying=false;
 void arpEmit(memorylab::EventKind,int,double,int);
 void render(float*,int);
 double clockBpm=130;
 double renderSampleRate=48000;
 bool ready=false;
 static constexpr std::size_t parameterCount=34;
 std::array<std::atomic<float>*,parameterCount> values{};
 std::array<float,parameterCount> previous{};
 JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MemoryLabProcessor)
};
