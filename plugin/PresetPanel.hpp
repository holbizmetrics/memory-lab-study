#pragma once
#include "Presets.hpp"
#include "PatchLocation.hpp"

class PresetPanel final:public juce::Component {
 MemoryLabProcessor&processor;
 juce::ComboBox category,sound;
 juce::TextButton load{"LOAD SOUND"},undo{"UNDO LOAD"};
 juce::ToggleButton autoLoad{"Auto-load on selection"};
 std::unique_ptr<memorylab::PatchLocation> preferences;
 juce::String status{"Browsing is silent. LOAD replaces sound controls; output stays as set."};
 int selected=0;
 void loadSelected(){if(memorylab::presets::load(processor,selected))status="Loaded: "+juce::String(memorylab::presets::bank()[std::size_t(selected)].name)+". Play new notes; SAVE to keep edits.";refresh();}
 void populate(){
  sound.clear(juce::dontSendNotification);
  const auto filter=category.getText();
  for(int i=0;i<int(memorylab::presets::bank().size());++i){const auto&e=memorylab::presets::bank()[std::size_t(i)];if(filter=="All"||filter==e.category)sound.addItem(e.name,i+1);}
  if(sound.indexOfItemId(selected+1)<0)selected=sound.getItemId(0)-1;
  sound.setSelectedId(selected+1,juce::dontSendNotification);repaint();
 }
public:
 explicit PresetPanel(MemoryLabProcessor&p,const juce::File& preferenceFile={}):processor(p),preferences(preferenceFile==juce::File()?std::make_unique<memorylab::PatchLocation>():std::make_unique<memorylab::PatchLocation>(preferenceFile)),selected(p.presetSession.selected){
  setComponentID("presetPanel");
  for(auto*c:{static_cast<juce::Component*>(&category),static_cast<juce::Component*>(&sound),static_cast<juce::Component*>(&load),static_cast<juce::Component*>(&undo)})addAndMakeVisible(c);
  category.setComponentID("presetCategory");sound.setComponentID("presetSound");load.setComponentID("loadFactoryPreset");undo.setComponentID("undoFactoryPreset");
  category.setName("Preset category");sound.setName("Starting sound");
  addAndMakeVisible(autoLoad);autoLoad.setComponentID("presetAutoLoad");autoLoad.setToggleState(preferences->autoLoadPresets(),juce::dontSendNotification);
  autoLoad.setTooltip("When enabled, choosing a different sound loads it immediately. Category changes and enabling this option never load a sound. Undo restores the previous sound.");
  autoLoad.onClick=[this]{const bool enabled=autoLoad.getToggleState();status=preferences->rememberAutoLoadPresets(enabled)?(enabled?"Auto-load enabled. The next sound selection replaces current edits.":"Auto-load off. Browse, then LOAD SOUND."):"Preference could not be saved; this setting applies for this editor session.";repaint();};
  const char*categories[]{"All","Basic","Bass","Lead","Pad","Pluck","Percussion","Texture","Rhythm"};int id=1;for(auto*c:categories)category.addItem(c,id++);
  category.setSelectedId(1,juce::dontSendNotification);populate();
  category.onChange=[this]{populate();};sound.onChange=[this]{const int next=sound.getSelectedId()-1;if(next<0||next==selected)return;selected=next;if(autoLoad.getToggleState())loadSelected();else repaint();};
  load.setTooltip("Replace sound controls, including expression and arp. Output stays unchanged. Both drives load at zero. SAVE edits first; play fresh notes after loading.");
  undo.setTooltip("Restore the full sound before the last preset load, until another sound edit, INIT, file load or automation invalidates it.");
  load.onClick=[this]{loadSelected();};
  undo.onClick=[this]{if(memorylab::presets::undo(processor))status="Previous sound restored.";refresh();};refresh();
 }
 void refresh(){undo.setEnabled(memorylab::presets::canUndo(processor));repaint();}
 void resized()override{category.setBounds(40,128,175,36);sound.setBounds(235,128,570,36);autoLoad.setBounds(40,258,600,26);load.setBounds(40,292,170,42);undo.setBounds(230,292,150,42);}
 void paint(juce::Graphics&g)override{
  g.setColour(panel);g.fillRoundedRectangle(getLocalBounds().toFloat(),12);
  g.setColour(white);g.setFont(juce::Font(juce::FontOptions(26.f,juce::Font::bold)));g.drawText("STARTING SOUNDS",40,22,740,36,juce::Justification::centredLeft);
  g.setColour(muted);g.setFont(juce::Font(juce::FontOptions(14.f)));g.drawFittedText("Eight original starting points. Browse then LOAD, or enable auto-load to audition each selection. Categories never load sounds. Both drives load bypassed; output stays as set.",40,68,750,50,juce::Justification::topLeft,3);
  if(selected>=0&&selected<int(memorylab::presets::bank().size())){g.setColour(mint);g.drawFittedText(memorylab::presets::bank()[std::size_t(selected)].description,40,190,750,56,juce::Justification::topLeft,3);}
  g.setColour(white);g.drawFittedText(status,40,351,750,40,juce::Justification::topLeft,2);
  g.setColour(muted);g.setFont(juce::Font(juce::FontOptions(11.f)));g.drawText("LOAD replaces edits. SAVE keeps them as .mlpatch files. Selection and undo are session-only; DAW state stores the sound.",40,395,790,18,juce::Justification::centredLeft);
 }
};
