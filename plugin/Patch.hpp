#pragma once
#include "Processor.hpp"
#include <cstdlib>
#include <limits>
#include <set>

namespace memorylab::patch {
// Explicit user action on the message thread; never reset on editor creation.
inline void initialise(MemoryLabProcessor& processor){
 for(auto* p:processor.getParameters()){p->beginChangeGesture();p->setValueNotifyingHost(p->getDefaultValue());p->endChangeGesture();}
 processor.updateHostDisplay();
}
inline juce::String encode(MemoryLabProcessor& processor){
 juce::XmlElement root("MemoryLabPatch");root.setAttribute("schemaVersion",16);
 for(auto* base:processor.getParameters())if(auto* p=dynamic_cast<juce::RangedAudioParameter*>(base)){
  auto* entry=root.createNewChildElement("PARAM");entry->setAttribute("id",p->paramID);
  entry->setAttribute("value",double(p->convertFrom0to1(p->getValue())));
 }
 return root.toString();
}

// Validate the complete patch before applying any value. File loading is a
// message-thread action; it is deliberately separate from host state migration.
inline juce::Result apply(MemoryLabProcessor& processor,const juce::String& text){
 auto root=juce::parseXML(text);
 if(!root||!root->hasTagName("MemoryLabPatch")||(root->getStringAttribute("schemaVersion")!="1"&&root->getStringAttribute("schemaVersion")!="2"&&root->getStringAttribute("schemaVersion")!="3"&&root->getStringAttribute("schemaVersion")!="4"&&root->getStringAttribute("schemaVersion")!="5"&&root->getStringAttribute("schemaVersion")!="6"&&root->getStringAttribute("schemaVersion")!="7"&&root->getStringAttribute("schemaVersion")!="8"&&root->getStringAttribute("schemaVersion")!="9"&&root->getStringAttribute("schemaVersion")!="10"&&root->getStringAttribute("schemaVersion")!="11"&&root->getStringAttribute("schemaVersion")!="12"&&root->getStringAttribute("schemaVersion")!="13"&&root->getStringAttribute("schemaVersion")!="14"&&root->getStringAttribute("schemaVersion")!="15"&&root->getStringAttribute("schemaVersion")!="16"))
  return juce::Result::fail("This is not a supported Memory Lab patch.");
 const bool legacy=root->getStringAttribute("schemaVersion")=="1";
 const bool oldOscillators=legacy||root->getStringAttribute("schemaVersion")=="2";
 const bool oldNoise=oldOscillators||root->getStringAttribute("schemaVersion")=="3";
 const bool oldPolarity=root->getStringAttribute("schemaVersion")!="16";
 const bool oldParts=oldPolarity&&root->getStringAttribute("schemaVersion")!="15";
 const bool oldModern=oldParts&&root->getStringAttribute("schemaVersion")!="14";
 const bool oldVcos=oldModern&&root->getStringAttribute("schemaVersion")!="13";
 const bool oldRoutes=oldVcos&&root->getStringAttribute("schemaVersion")!="12";
 const bool oldEnvelopes=oldRoutes&&root->getStringAttribute("schemaVersion")!="11";
 const bool oldMore=oldEnvelopes&&root->getStringAttribute("schemaVersion")!="10";
 const bool oldSecond=oldMore&&root->getStringAttribute("schemaVersion")!="9";
 const bool oldExpression=oldSecond&&root->getStringAttribute("schemaVersion")!="8";
 const bool oldClock=oldExpression&&root->getStringAttribute("schemaVersion")!="7";
 const bool oldShape=oldClock&&root->getStringAttribute("schemaVersion")!="6";
 const bool oldFamily=oldShape&&root->getStringAttribute("schemaVersion")!="5";
 std::set<juce::String> seen;
 std::vector<std::pair<juce::RangedAudioParameter*,float>> values;
 for(auto* entry:root->getChildIterator()){
  const auto id=entry->getStringAttribute("id");auto* p=processor.state.getParameter(id);
  if(!entry->hasTagName("PARAM")||!p||(oldPolarity&&id.endsWith("Unipolar"))||(oldParts&&(id.startsWith("part")||id=="splitNote"))||(oldModern&&(id=="modernEnabled"||id=="modernProfile"||id=="modernBandwidth"))||(oldVcos&&(id=="osc1PulseLevel"||id=="osc2SawLevel"||id=="osc1Width"))||(oldRoutes&&(id=="lfoAmp"||id=="lfo2Amp"||id=="lfo3Amp"||id=="lfo4Amp"||id=="ampPitch"||id=="fltPitch"||id=="env3Pwm"||id=="env4Pwm"))||(oldEnvelopes&&(id.startsWith("env3")||id.startsWith("env4")))||(oldMore&&(id.startsWith("lfo3")||id.startsWith("lfo4")))||(oldSecond&&id.startsWith("lfo2"))||(legacy&&id=="voiceDrive")||(oldOscillators&&(id=="sawLevel"||id=="pulseLevel"||id=="osc2Interval"))||(oldNoise&&id=="noiseLevel")||(oldExpression&&(id=="keyTrack"||id=="ampVelocity"||id=="filterVelocity"))||(oldClock&&(id=="lfoSync"||id=="lfoDivision"||id=="lfoRetrigger"||id=="lfoBpm"))||(oldShape&&id=="lfoShape")||(oldFamily&&id=="filterFamily")||!seen.insert(id).second)
   return juce::Result::fail("The patch contains an unknown or repeated control.");
  const auto raw=entry->getStringAttribute("value").trim();const auto* start=raw.toRawUTF8();char* end=nullptr;
  const double value=std::strtod(start,&end);
  const double minimum=p->convertFrom0to1(0),maximum=p->convertFrom0to1(1);
  // XML's decimal representation can round a float endpoint just outside its
  // exact double value. Admit only double-formatting error, not a float ULP
  // or parameter-step tolerance. Zero remains exact; all checks precede edits.
  const auto rounding=[](double bound){return 8*std::numeric_limits<double>::epsilon()*std::abs(bound);};
  if(end==start||*end!='\0'||!std::isfinite(value)||value<minimum-rounding(minimum)||value>maximum+rounding(maximum))
   return juce::Result::fail("The patch contains an invalid value for "+id+".");
  values.emplace_back(p,p->convertTo0to1(float(juce::jlimit(minimum,maximum,value))));
 }
 if(oldPolarity)for(auto*base:processor.getParameters())if(auto*p=dynamic_cast<juce::RangedAudioParameter*>(base))if(p->paramID.endsWith("Unipolar")&&!(oldParts&&p->paramID.startsWith("part")))values.emplace_back(p,p->getDefaultValue());
 if(oldParts)for(auto*base:processor.getParameters())if(auto*p=dynamic_cast<juce::RangedAudioParameter*>(base))if(p->paramID.startsWith("part")||p->paramID=="splitNote")values.emplace_back(p,p->getDefaultValue());
 if(oldModern)for(const char*id:{"modernEnabled","modernProfile","modernBandwidth"}){auto*p=processor.state.getParameter(id);values.emplace_back(p,p->getDefaultValue());}
 if(oldVcos)for(const char*id:{"osc1PulseLevel","osc2SawLevel","osc1Width"}){auto*p=processor.state.getParameter(id);values.emplace_back(p,p->getDefaultValue());}
 if(oldRoutes)for(const char*id:{"lfoAmp","lfo2Amp","lfo3Amp","lfo4Amp","ampPitch","fltPitch","env3Pwm","env4Pwm"}){auto*p=processor.state.getParameter(id);values.emplace_back(p,p->getDefaultValue());}
 if(oldEnvelopes)for(const char*id:{"env3Delay","env3Attack","env3Hold","env3Decay","env3Sustain","env3Release","env3Pitch","env3Cutoff","env4Delay","env4Attack","env4Hold","env4Decay","env4Sustain","env4Release","env4Pitch","env4Cutoff"}){auto*p=processor.state.getParameter(id);values.emplace_back(p,p->getDefaultValue());}
 if(oldMore)for(const char*id:{"lfo3Rate","lfo3Pitch","lfo3Pwm","lfo3Cutoff","lfo3Shape","lfo3Sync","lfo3Division","lfo3Retrigger","lfo3Bpm","lfo4Rate","lfo4Pitch","lfo4Pwm","lfo4Cutoff","lfo4Shape","lfo4Sync","lfo4Division","lfo4Retrigger","lfo4Bpm"}){auto*p=processor.state.getParameter(id);values.emplace_back(p,p->getDefaultValue());}
 if(oldSecond)for(const char*id:{"lfo2Rate","lfo2Pitch","lfo2Pwm","lfo2Cutoff","lfo2Shape","lfo2Sync","lfo2Division","lfo2Retrigger","lfo2Bpm"}){auto*p=processor.state.getParameter(id);values.emplace_back(p,p->getDefaultValue());}
 if(oldExpression)for(const char*id:{"keyTrack","ampVelocity","filterVelocity"}){auto*p=processor.state.getParameter(id);values.emplace_back(p,p->getDefaultValue());}
 if(oldClock)for(const char*id:{"lfoSync","lfoDivision","lfoRetrigger","lfoBpm"}){auto*p=processor.state.getParameter(id);values.emplace_back(p,p->getDefaultValue());}
 if(oldShape)values.emplace_back(processor.state.getParameter("lfoShape"),0.f);
 if(oldFamily)values.emplace_back(processor.state.getParameter("filterFamily"),0.f);
 if(legacy)values.emplace_back(processor.state.getParameter("voiceDrive"),0.f);
 if(oldNoise)values.emplace_back(processor.state.getParameter("noiseLevel"),0.f);
 if(oldOscillators)for(const char*id:{"sawLevel","pulseLevel","osc2Interval"}){auto*p=processor.state.getParameter(id);values.emplace_back(p,p->getDefaultValue());}
 if(values.size()!=std::size_t(processor.getParameters().size()))
  return juce::Result::fail("The patch does not contain every control.");
 for(const auto& value:values){auto* p=value.first;p->beginChangeGesture();p->setValueNotifyingHost(value.second);p->endChangeGesture();}
 processor.updateHostDisplay();
 return juce::Result::ok();
}

inline juce::Result save(MemoryLabProcessor& processor,const juce::File& file){
 juce::TemporaryFile temporary(file);
 if(!temporary.getFile().replaceWithText(encode(processor))||!temporary.overwriteTargetFileWithTemporary())
  return juce::Result::fail("Could not save the patch. Check the folder and file permissions.");
 return juce::Result::ok();
}
inline juce::Result load(MemoryLabProcessor& processor,const juce::File& file){
 if(!file.existsAsFile()||file.getSize()<=0||file.getSize()>1024*1024)
  return juce::Result::fail("Choose a Memory Lab patch file smaller than 1 MB.");
 return apply(processor,file.loadFileAsString());
}
}
