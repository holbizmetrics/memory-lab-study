#pragma once
#include <juce_audio_utils/juce_audio_utils.h>
#include "../native/voice.hpp"
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
 double getTailLengthSeconds()const override{return 1.5;}
 int getNumPrograms()override{return 1;}
 int getCurrentProgram()override{return 0;}
 void setCurrentProgram(int)override{}
 const juce::String getProgramName(int)override{return "Study baseline";}
 void changeProgramName(int,const juce::String&)override{}
 bool hasEditor()const override{return true;}
 juce::AudioProcessorEditor* createEditor()override{return new juce::GenericAudioProcessorEditor(*this);}
 void getStateInformation(juce::MemoryBlock&)override;
 void setStateInformation(const void*,int)override;
 juce::AudioProcessorValueTreeState state;
private:
 static juce::AudioProcessorValueTreeState::ParameterLayout layout();
 void midi(const juce::uint8*,int);
 memorylab::Synth synth;
 bool ready=false;
 static constexpr std::size_t parameterCount=16;
 std::array<std::atomic<float>*,parameterCount> values{};
 std::array<float,parameterCount> previous{};
 JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MemoryLabProcessor)
};
