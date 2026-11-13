#pragma once
#include <juce_audio_utils/juce_audio_utils.h>
#include "../native/part-synth.hpp"
#include "../native/arpeggiator.hpp"
#include "RandomizerSession.hpp"
#include "PresetSession.hpp"
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
 memorylab::RandomizerSession randomizerSession;
 memorylab::PresetSession presetSession;
 std::atomic<int> editingPart{0}; // editor selection, not a sound/host parameter
 static juce::String partId(int part,const juce::String&base){return part==0||base.startsWith("arp")?base:"part"+juce::String(part+1)+"__"+base;}
 juce::String editId(const juce::String&base)const{return partId(editingPart.load(),base);}
 void copyPart(int from,int to);
 juce::MidiKeyboardState keyboard;
 std::atomic<float> outputPeak{0};
 std::atomic<int> voiceCount{0};
 std::atomic<float> arpTempo{130};
 std::atomic<float> lfoTempo{130},lfo2Tempo{130},lfo3Tempo{130},lfo4Tempo{130};
 std::atomic<bool> lfo2HasHostTempo{false},lfo3HasHostTempo{false},lfo4HasHostTempo{false};
 std::atomic<bool> lfoHasHostTempo{false};
 std::atomic<int> arpStep{0};
 std::atomic<bool> arpHasHostTempo{false};
private:
 static juce::AudioProcessorValueTreeState::ParameterLayout layout();
 void midi(const juce::uint8*,int);
 memorylab::PartSynth synth;
 memorylab::Arpeggiator arp;
 std::array<std::atomic<float>*,7> arpValues{};
 bool arpEnabled=false,wasPlaying=false;
 void arpEmit(memorylab::EventKind,int,double,int);
 void render(float*,int);
 double clockBpm=130;
 double renderSampleRate=48000;
 bool ready=false;
 static constexpr std::size_t parameterCount=99;
 std::array<std::atomic<float>*,parameterCount> values{};
 std::array<float,parameterCount> previous{};
 std::array<std::array<std::atomic<float>*,parameterCount>,3> partValues{};
 std::array<std::array<float,parameterCount>,3> partPrevious{};
 std::array<std::array<double,parameterCount>,3> partInitial{};
 std::array<std::atomic<float>*,6> routingValues{};
 std::array<int,6> previousRouting{{0,60,0,0,0,0}};
 void updateRouting();
 JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MemoryLabProcessor)
};
