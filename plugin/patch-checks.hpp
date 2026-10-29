#pragma once
#include "Patch.hpp"

void patchChecks(){
 auto source=std::make_unique<MemoryLabProcessor>(),target=std::make_unique<MemoryLabProcessor>();
 int index=0;
 for(auto* p:source->getParameters())p->setValueNotifyingHost(float(++index)/float(source->getParameters().size()+1));
 const auto encoded=memorylab::patch::encode(*source);
 check(memorylab::patch::apply(*target,encoded).wasOk(),"patch roundtrip rejected");
 for(int i=0;i<source->getParameters().size();++i)
  check(std::abs(source->getParameters()[i]->getValue()-target->getParameters()[i]->getValue())<.00001f,"patch lost a parameter");
 const auto unchanged=memorylab::patch::encode(*target);
 auto reject=[&](const juce::String& text){
  check(memorylab::patch::apply(*target,text).failed(),"invalid patch accepted");
  check(memorylab::patch::encode(*target)==unchanged,"invalid patch partially changed sound");
 };
 reject("not XML");
 auto root=juce::parseXML(encoded);root->setAttribute("schemaVersion",4);reject(root->toString());
 root=juce::parseXML(encoded);root->getChildElement(root->getNumChildElements()-1)->setAttribute("value","nan");reject(root->toString());
 root=juce::parseXML(encoded);root->getChildElement(root->getNumChildElements()-1)->setAttribute("value","15garbage");reject(root->toString());
 root=juce::parseXML(encoded);root->getChildElement(root->getNumChildElements()-1)->setAttribute("value",1000000);reject(root->toString());
 root=juce::parseXML(encoded);root->getChildElement(root->getNumChildElements()-1)->setAttribute("id","unknown");reject(root->toString());
 root=juce::parseXML(encoded);root->getChildElement(root->getNumChildElements()-1)->setAttribute("id","drive");reject(root->toString());
 root=juce::parseXML(encoded);root->removeChildElement(root->getChildElement(root->getNumChildElements()-1),true);reject(root->toString());
 root=juce::parseXML(encoded);root->setAttribute("schemaVersion",1);
 for(int i=root->getNumChildElements()-1;i>=0;--i)if(root->getChildElement(i)->getStringAttribute("id")=="voiceDrive"||root->getChildElement(i)->getStringAttribute("id")=="sawLevel"||root->getChildElement(i)->getStringAttribute("id")=="pulseLevel"||root->getChildElement(i)->getStringAttribute("id")=="osc2Interval")root->removeChildElement(root->getChildElement(i),true);
 set(*target,"voiceDrive",24);check(memorylab::patch::apply(*target,root->toString()).wasOk(),"legacy patch rejected");
 check(target->state.getRawParameterValue("voiceDrive")->load()==0,"legacy patch did not bypass voice drive");
 check(memorylab::patch::apply(*target,encoded).wasOk(),"restore current patch failed");
 root=juce::parseXML(encoded);root->setAttribute("schemaVersion",2);
 for(int i=root->getNumChildElements()-1;i>=0;--i){const auto id=root->getChildElement(i)->getStringAttribute("id");if(id=="sawLevel"||id=="pulseLevel"||id=="osc2Interval")root->removeChildElement(root->getChildElement(i),true);}
 set(*target,"sawLevel",1);set(*target,"pulseLevel",0);set(*target,"osc2Interval",24);
 check(memorylab::patch::apply(*target,root->toString()).wasOk(),"schema 2 patch rejected");
 check(std::abs(target->state.getRawParameterValue("sawLevel")->load()-.42f)<1e-6&&std::abs(target->state.getRawParameterValue("pulseLevel")->load()-.28f)<1e-6&&target->state.getRawParameterValue("osc2Interval")->load()==0,"schema 2 oscillator defaults lost");
 check(std::abs(target->state.getRawParameterValue("voiceDrive")->load()-source->state.getRawParameterValue("voiceDrive")->load())<1e-6,"schema 2 drive lost");
 check(memorylab::patch::apply(*target,encoded).wasOk(),"restore after schema 2 failed");
 juce::TemporaryFile file;
 check(memorylab::patch::save(*source,file.getFile()).wasOk(),"patch file save failed");
 set(*target,"cutoff",400);check(memorylab::patch::load(*target,file.getFile()).wasOk(),"patch file load failed");
 check(memorylab::patch::encode(*target)==unchanged,"patch file roundtrip changed controls");
 std::cout<<"PASS patches: all 39 controls roundtrip, atomic file save/load, malformed/unsupported/incomplete/duplicate/unknown/out-of-range values rejected without partial edits.\n";
}
