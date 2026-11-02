#pragma once
#include "Randomizer.hpp"

// Included after the editor palette; this is UI-only session state and actions.
class DicePanel final:public juce::Component {
 MemoryLabProcessor&processor;
 std::array<juce::ToggleButton,4>locks;
 juce::TextButton roll{"ROLL NEW SOUND"},undo{"UNDO ROLL"};
 juce::Random rng;
 juce::String status{"Release held notes after a roll. SAVE a sound you want to keep."};
public:
 explicit DicePanel(MemoryLabProcessor&p):processor(p){
  setComponentID("dicePanel");
  const char*names[]{"LOCK OSC","LOCK FILTER","LOCK ENVELOPES","LOCK MOTION"};
  const char*ids[]{"lockOscillators","lockFilter","lockEnvelopes","lockMotion"};
  for(int i=0;i<4;++i){auto&b=locks[std::size_t(i)];addAndMakeVisible(b);b.setButtonText(names[i]);b.setComponentID(ids[i]);b.setToggleState(p.randomizerSession.locked[std::size_t(i)],juce::dontSendNotification);b.onClick=[this,i]{processor.randomizerSession.locked[std::size_t(i)]=locks[std::size_t(i)].getToggleState();refresh();};}
  addAndMakeVisible(roll);addAndMakeVisible(undo);roll.setComponentID("rollPatch");undo.setComponentID("undoRoll");
  roll.setTooltip("Replace unlocked sound sections. Output, drive, arp and fallback BPM stay unchanged. SAVE first to keep current edits.");
  undo.setTooltip("Restore the entire patch from before the last roll. Available until other sound edits, INIT, load or automation change it.");
  roll.onClick=[this]{if(memorylab::randomizer::roll(processor,std::uint32_t(rng.nextInt())))status="New sound. Play new notes; SAVE to keep it, or UNDO ROLL to go back.";refresh();};
  undo.onClick=[this]{if(memorylab::randomizer::undo(processor))status="Previous sound restored.";refresh();};refresh();
 }
 void refresh(){undo.setEnabled(memorylab::randomizer::canUndo(processor));roll.setEnabled(!std::all_of(processor.randomizerSession.locked.begin(),processor.randomizerSession.locked.end(),[](bool v){return v;}));repaint();}
 void resized()override{
  for(int i=0;i<4;++i)locks[std::size_t(i)].setBounds(40+196*i,140,190,34);
  roll.setBounds(40,288,170,42);undo.setBounds(226,288,150,42);
 }
 void paint(juce::Graphics&g)override{
  g.setColour(panel);g.fillRoundedRectangle(getLocalBounds().toFloat(),12);
  g.setColour(white);g.setFont(juce::Font(juce::FontOptions(26.f,juce::Font::bold)));g.drawText("PATCH DICE",40,22,650,36,juce::Justification::centredLeft);
  g.setColour(muted);g.setFont(juce::Font(juce::FontOptions(14.f)));g.drawFittedText("Vary unlocked sound sections. Output, both drives, arpeggiator, oscillator phase reset and fallback BPM stay as you set them.",40,68,680,55,juce::Justification::topLeft,3);
  const char*descriptions[]{"Source mix, noise, pulse width, detune and interval.","Family, cutoff, moderate resonance and Classic profile.","Amplitude and filter stages, plus filter-envelope amount.","LFO shape, rate, clock mode and modulation depths."};
  g.setFont(juce::Font(juce::FontOptions(13.f)));for(int i=0;i<4;++i)g.drawFittedText(descriptions[i],40+196*i,188,170,72,juce::Justification::topLeft,4);
  g.setColour(amber);g.drawRoundedRectangle(762,26,62,62,10,2);
  for(auto point:{juce::Point<float>{778,42},{808,42},{793,57},{778,72},{808,72}})g.fillEllipse(point.x-3,point.y-3,6,6);
  g.setColour(white);g.drawFittedText(status,40,347,785,35,juce::Justification::topLeft,2);
  g.setColour(muted);g.setFont(juce::Font(juce::FontOptions(11.f)));g.drawText("Locks / one-roll undo last for this instance. Other sound edits invalidate undo. Rolls can differ in loudness.",40,391,790,20,juce::Justification::centredLeft);
 }
};
