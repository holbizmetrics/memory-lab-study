#pragma once
#include "Patch.hpp"
#include <random>

namespace memorylab::randomizer {
inline bool needsDriveBypass(MemoryLabProcessor&p){
 return p.randomizerSession.bypassDrives&&(p.state.getRawParameterValue(p.editId("drive"))->load()>0||p.state.getRawParameterValue(p.editId("voiceDrive"))->load()>0);
}
inline bool canRoll(MemoryLabProcessor&p){
 return !std::all_of(p.randomizerSession.locked.begin(),p.randomizerSession.locked.end(),[](bool v){return v;})||needsDriveBypass(p);
}
inline bool canUndo(MemoryLabProcessor&p){
 auto&s=p.randomizerSession;
 if(s.beforeRoll.isEmpty())return false;
 if(patch::encode(p)==s.afterRoll)return true;
 s.beforeRoll.clear();s.afterRoll.clear();return false;
}
inline bool undo(MemoryLabProcessor&p){
 if(!canUndo(p))return false;
 const auto before=p.randomizerSession.beforeRoll;
 if(patch::apply(p,before).failed())return false;
 p.randomizerSession.beforeRoll.clear();p.randomizerSession.afterRoll.clear();return true;
}
// Only explicit UI/offline calls. No random generation or XML work on audio thread.
inline bool roll(MemoryLabProcessor&p,std::uint32_t seed){
 const auto locks=p.randomizerSession.locked;
 if(!canRoll(p))return false;
 std::mt19937 rng(seed);
 auto unit=[&]{return double(rng())/4294967296.;};
 auto range=[&](double lo,double hi){return float(lo+(hi-lo)*unit());};
 auto pick=[&](int count){return int(rng()%unsigned(count));};
 std::vector<std::pair<juce::RangedAudioParameter*,float>> values;
 auto add=[&](const char*id,float value){auto*parameter=p.state.getParameter(p.editId(id));jassert(parameter);if(parameter)values.emplace_back(parameter,parameter->convertTo0to1(value));};
 if(p.randomizerSession.bypassDrives)for(const char*id:{"drive","voiceDrive"})if(p.state.getRawParameterValue(p.editId(id))->load()!=0)add(id,0);
 if(!locks[0]){
  add("sawLevel",range(.18,.42));add("pulseLevel",range(.08,.25));
  add("noiseLevel",pick(4)==0?range(0,.06):0);add("width",range(.22,.78));add("detune",range(-12,12));
  const int intervals[]{0,0,0,7,12,-12,-5};add("osc2Interval",float(intervals[pick(7)]));
 }
 if(!locks[1]){
  add("filterFamily",float(pick(4)));add("profile",range(.5,1));add("resonance",range(.6,1.3));
  add("cutoff",float(400*std::pow(12.5,unit())));
 }
 if(!locks[2]){
  add("ampDelay",0);add("ampAttack",range(5,500));add("ampHold",range(0,60));add("ampDecay",range(100,1100));add("ampSustain",range(.4,.85));add("ampRelease",range(150,1400));
  add("fltDelay",0);add("fltAttack",range(5,700));add("fltHold",range(0,80));add("fltDecay",range(60,900));add("fltSustain",range(0,.6));add("fltRelease",range(100,1000));add("filterEnv",range(0,1.5));
 }
 if(!locks[3]){
  add("lfoShape",float(pick(4)));add("lfoRate",float(.15*std::pow(40.,unit())));
  add("vibrato",range(0,8));add("pwm",range(0,.1));add("filterLfo",range(0,.6));add("pressure",range(0,.8));
  add("lfoSync",float(pick(2)));add("lfoDivision",float(pick(6)));add("lfoRetrigger",float(pick(2)));
 }
 const auto before=patch::encode(p);
 for(const auto&v:values){v.first->beginChangeGesture();v.first->setValueNotifyingHost(v.second);v.first->endChangeGesture();}
 p.updateHostDisplay();p.randomizerSession.beforeRoll=before;p.randomizerSession.afterRoll=patch::encode(p);return true;
}
}
