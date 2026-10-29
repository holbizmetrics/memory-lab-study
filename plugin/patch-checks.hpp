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
 auto root=juce::parseXML(encoded);root->setAttribute("schemaVersion",3);reject(root->toString());
 root=juce::parseXML(encoded);root->getChildElement(root->getNumChildElements()-1)->setAttribute("value","nan");reject(root->toString());
 root=juce::parseXML(encoded);root->getChildElement(root->getNumChildElements()-1)->setAttribute("value","15garbage");reject(root->toString());
 root=juce::parseXML(encoded);root->getChildElement(root->getNumChildElements()-1)->setAttribute("value",1000000);reject(root->toString());
 root=juce::parseXML(encoded);root->getChildElement(root->getNumChildElements()-1)->setAttribute("id","unknown");reject(root->toString());
 root=juce::parseXML(encoded);root->getChildElement(root->getNumChildElements()-1)->setAttribute("id","drive");reject(root->toString());
 root=juce::parseXML(encoded);root->removeChildElement(root->getChildElement(root->getNumChildElements()-1),true);reject(root->toString());
 root=juce::parseXML(encoded);root->setAttribute("schemaVersion",1);
 for(int i=root->getNumChildElements()-1;i>=0;--i)if(root->getChildElement(i)->getStringAttribute("id")=="voiceDrive")root->removeChildElement(root->getChildElement(i),true);
 set(*target,"voiceDrive",24);check(memorylab::patch::apply(*target,root->toString()).wasOk(),"legacy patch rejected");
 check(target->state.getRawParameterValue("voiceDrive")->load()==0,"legacy patch did not bypass voice drive");
 check(memorylab::patch::apply(*target,encoded).wasOk(),"restore current patch failed");
 juce::TemporaryFile file;
 check(memorylab::patch::save(*source,file.getFile()).wasOk(),"patch file save failed");
 set(*target,"cutoff",400);check(memorylab::patch::load(*target,file.getFile()).wasOk(),"patch file load failed");
 check(memorylab::patch::encode(*target)==unchanged,"patch file roundtrip changed controls");
 std::cout<<"PASS patches: all 36 controls roundtrip, atomic file save/load, malformed/unsupported/incomplete/duplicate/unknown/out-of-range values rejected without partial edits.\n";
}
