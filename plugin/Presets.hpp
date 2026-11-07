#pragma once
#include "Patch.hpp"

namespace memorylab::presets {
struct Entry {
 const char*name;const char*category;const char*description;
 std::vector<std::pair<const char*,float>> settings;
};
inline const std::vector<Entry>& bank(){
 static const std::vector<Entry> entries{
  {"Clean Foundation","Basic","A clean starting voice. Both drives bypassed; no modulation.",{}},
  {"Round Bass","Bass","Short, low-pass bass. Try C2-C3; firmer strikes open the filter.",{{"sawLevel",.32f},{"pulseLevel",.12f},{"detune",2},{"cutoff",420},{"resonance",.8f},{"ampAttack",3},{"ampDecay",180},{"ampSustain",.65f},{"ampRelease",100},{"filterEnv",1.5f},{"fltAttack",0},{"fltDecay",220},{"fltSustain",0},{"fltRelease",100},{"keyTrack",.4f}}},
  {"Soft Fifth","Lead","A saw/pulse fifth with gentle vibrato. Polyphonic, not a mono-legato mode.",{{"sawLevel",.26f},{"pulseLevel",.16f},{"osc2Interval",7},{"detune",3},{"cutoff",2400},{"resonance",.7f},{"ampAttack",20},{"ampSustain",.8f},{"ampRelease",220},{"vibrato",3},{"keyTrack",.5f}}},
  {"Slow Air","Pad","Hold a chord: slow amplitude and filter swells, light noise and pulse-width motion.",{{"sawLevel",.2f},{"pulseLevel",.18f},{"noiseLevel",.012f},{"cutoff",800},{"resonance",.7f},{"ampAttack",1200},{"ampDecay",1000},{"ampSustain",.8f},{"ampRelease",1800},{"filterEnv",1.2f},{"fltAttack",1800},{"fltDecay",1200},{"fltSustain",.6f},{"fltRelease",1500},{"lfoRate",.3f},{"pwm",.08f},{"keyTrack",.4f}}},
  {"Glass Pluck","Pluck","A short bright envelope over a quiet low-pass body. Try an arpeggiated chord.",{{"sawLevel",.22f},{"pulseLevel",.14f},{"cutoff",650},{"resonance",1.1f},{"ampAttack",1},{"ampDecay",550},{"ampSustain",0},{"ampRelease",180},{"filterEnv",2.5f},{"fltAttack",0},{"fltDecay",350},{"fltSustain",0},{"fltRelease",150},{"keyTrack",.6f}}},
  {"Noise Tap","Percussion","Noise alone through a band-pass filter and short envelope. No pitched oscillator.",{{"sawLevel",0},{"pulseLevel",0},{"noiseLevel",.18f},{"filterFamily",2},{"cutoff",2400},{"resonance",.8f},{"ampAttack",0},{"ampDecay",170},{"ampSustain",0},{"ampRelease",70},{"filterEnv",-.7f},{"fltAttack",0},{"fltDecay",140},{"fltSustain",0},{"fltRelease",70}}},
  {"Breath Swell","Texture","Noise alone with a slow opening filter. Hold notes for several seconds.",{{"sawLevel",0},{"pulseLevel",0},{"noiseLevel",.15f},{"cutoff",350},{"resonance",.65f},{"ampAttack",2200},{"ampSustain",.8f},{"ampRelease",2000},{"filterEnv",3},{"fltAttack",3500},{"fltDecay",1200},{"fltSustain",1},{"fltRelease",2000},{"filterVelocity",0}}},
  {"Clockwork Pulse","Rhythm","Hold a chord for a sixteenth-note arp and synced triangle filter movement. Uses host BPM or 130 fallback.",{{"sawLevel",.2f},{"pulseLevel",.15f},{"cutoff",900},{"resonance",.8f},{"ampAttack",2},{"ampDecay",180},{"ampSustain",.4f},{"ampRelease",100},{"filterEnv",1},{"fltAttack",0},{"fltDecay",130},{"fltSustain",0},{"lfoShape",1},{"lfoSync",1},{"lfoDivision",1},{"lfoRetrigger",0},{"filterLfo",.4f},{"arpOn",1},{"arpOctaves",2},{"arpGate",45}}}
 };
 return entries;
}
// Build a complete schema-11 patch without changing the live processor. Every
// preset starts from factory defaults, not the previously selected sound.
inline juce::String encode(MemoryLabProcessor&p,int index){
 if(index<0||index>=int(bank().size()))return {};
 juce::XmlElement root("MemoryLabPatch");root.setAttribute("schemaVersion",11);
 for(auto*base:p.getParameters())if(auto*parameter=dynamic_cast<juce::RangedAudioParameter*>(base)){
  float value=parameter->convertFrom0to1(parameter->getDefaultValue());
  const auto&id=parameter->paramID;
  if(id=="drive"||id=="voiceDrive")value=0;
  if(id=="resonance")value=.9f;
  for(const auto&setting:bank()[std::size_t(index)].settings)if(id==setting.first)value=setting.second;
  // Keep the user's output level; presets never turn a muted instrument up.
  if(id=="volume")value=parameter->convertFrom0to1(parameter->getValue());
  auto*entry=root.createNewChildElement("PARAM");entry->setAttribute("id",id);entry->setAttribute("value",double(value));
 }
 return root.toString();
}
inline bool canUndo(MemoryLabProcessor&p){
 auto&s=p.presetSession;if(s.beforeLoad.isEmpty())return false;
 if(patch::encode(p)==s.afterLoad)return true;
 s.beforeLoad.clear();s.afterLoad.clear();return false;
}
inline bool load(MemoryLabProcessor&p,int index){
 const auto text=encode(p,index);if(text.isEmpty())return false;
 const auto before=patch::encode(p);
 if(patch::apply(p,text).failed())return false;
 p.presetSession.selected=index;p.presetSession.beforeLoad=before;p.presetSession.afterLoad=patch::encode(p);
 p.randomizerSession.beforeRoll.clear();p.randomizerSession.afterRoll.clear();return true;
}
inline bool undo(MemoryLabProcessor&p){
 if(!canUndo(p))return false;
 const auto before=p.presetSession.beforeLoad;
 if(patch::apply(p,before).failed())return false;
 p.presetSession.beforeLoad.clear();p.presetSession.afterLoad.clear();return true;
}
}
