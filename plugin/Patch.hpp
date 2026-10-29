#pragma once
#include "Processor.hpp"
#include <cstdlib>
#include <set>

namespace memorylab::patch {
inline juce::String encode(MemoryLabProcessor& processor){
 juce::XmlElement root("MemoryLabPatch");root.setAttribute("schemaVersion",3);
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
 if(!root||!root->hasTagName("MemoryLabPatch")||(root->getStringAttribute("schemaVersion")!="1"&&root->getStringAttribute("schemaVersion")!="2"&&root->getStringAttribute("schemaVersion")!="3"))
  return juce::Result::fail("This is not a supported Memory Lab patch.");
 const bool legacy=root->getStringAttribute("schemaVersion")=="1";
 const bool oldOscillators=root->getStringAttribute("schemaVersion")!="3";
 std::set<juce::String> seen;
 std::vector<std::pair<juce::RangedAudioParameter*,float>> values;
 for(auto* entry:root->getChildIterator()){
  const auto id=entry->getStringAttribute("id");auto* p=processor.state.getParameter(id);
  if(!entry->hasTagName("PARAM")||!p||(legacy&&id=="voiceDrive")||(oldOscillators&&(id=="sawLevel"||id=="pulseLevel"||id=="osc2Interval"))||!seen.insert(id).second)
   return juce::Result::fail("The patch contains an unknown or repeated control.");
  const auto raw=entry->getStringAttribute("value").trim();const auto* start=raw.toRawUTF8();char* end=nullptr;
  const double value=std::strtod(start,&end);
  if(end==start||*end!='\0'||!std::isfinite(value)||value<p->convertFrom0to1(0)||value>p->convertFrom0to1(1))
   return juce::Result::fail("The patch contains an invalid value for "+id+".");
  values.emplace_back(p,p->convertTo0to1(float(value)));
 }
 if(legacy)values.emplace_back(processor.state.getParameter("voiceDrive"),0.f);
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
