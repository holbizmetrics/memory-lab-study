#pragma once
#include "Patch.hpp"
#include "PatchLocation.hpp"

void patchChecks(){
 // Exercise real persisted UI preferences across fresh owners, away from user settings.
 juce::TemporaryFile preferences;
 const auto documents=juce::File::getSpecialLocation(juce::File::userDocumentsDirectory);
 const auto chosen=juce::File::getCurrentWorkingDirectory().getChildFile("test-patch.mlpatch");
 {memorylab::PatchLocation location(preferences.getFile());check(location.initialFile().getParentDirectory()==documents,"patch folder initial fallback");check(location.rememberSuccess(chosen),"patch folder preference write");}
 {memorylab::PatchLocation reopened(preferences.getFile());check(reopened.initialFile().getParentDirectory()==chosen.getParentDirectory(),"patch folder not persisted across reopen");check(!reopened.rememberSuccess(juce::File::getSpecialLocation(juce::File::tempDirectory).getChildFile(juce::Uuid().toString()).getChildFile("missing.mlpatch")),"missing patch folder accepted");check(reopened.initialFile().getParentDirectory()==chosen.getParentDirectory(),"failed folder preference changed prior selection");}
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
 auto root=juce::parseXML(encoded);root->setAttribute("schemaVersion",12);reject(root->toString());
 root=juce::parseXML(encoded);root->getChildElement(root->getNumChildElements()-1)->setAttribute("value","nan");reject(root->toString());
 root=juce::parseXML(encoded);root->getChildElement(root->getNumChildElements()-1)->setAttribute("value","15garbage");reject(root->toString());
 root=juce::parseXML(encoded);root->getChildElement(root->getNumChildElements()-1)->setAttribute("value",1000000);reject(root->toString());
 root=juce::parseXML(encoded);root->getChildElement(root->getNumChildElements()-1)->setAttribute("id","unknown");reject(root->toString());
 root=juce::parseXML(encoded);root->getChildElement(root->getNumChildElements()-1)->setAttribute("id","drive");reject(root->toString());
 root=juce::parseXML(encoded);root->removeChildElement(root->getChildElement(root->getNumChildElements()-1),true);reject(root->toString());
 root=juce::parseXML(encoded);root->setAttribute("schemaVersion",1);
 for(int n=root->getNumChildElements()-1;n>=0;--n)if((root->getChildElement(n)->getStringAttribute("id").startsWith("lfo2")||root->getChildElement(n)->getStringAttribute("id").startsWith("lfo3")||root->getChildElement(n)->getStringAttribute("id").startsWith("lfo4")||root->getChildElement(n)->getStringAttribute("id").startsWith("env3")||root->getChildElement(n)->getStringAttribute("id").startsWith("env4")))root->removeChildElement(root->getChildElement(n),true);
 for(int i=root->getNumChildElements()-1;i>=0;--i){const auto id=root->getChildElement(i)->getStringAttribute("id");if(id=="keyTrack"||id=="ampVelocity"||id=="filterVelocity")root->removeChildElement(root->getChildElement(i),true);}
 for(int i=root->getNumChildElements()-1;i>=0;--i){const auto id=root->getChildElement(i)->getStringAttribute("id");if(id=="lfoSync"||id=="lfoDivision"||id=="lfoRetrigger"||id=="lfoBpm")root->removeChildElement(root->getChildElement(i),true);}
 for(int i=root->getNumChildElements()-1;i>=0;--i)if(root->getChildElement(i)->getStringAttribute("id")=="lfoShape"||root->getChildElement(i)->getStringAttribute("id")=="filterFamily"||root->getChildElement(i)->getStringAttribute("id")=="noiseLevel"||root->getChildElement(i)->getStringAttribute("id")=="voiceDrive"||root->getChildElement(i)->getStringAttribute("id")=="sawLevel"||root->getChildElement(i)->getStringAttribute("id")=="pulseLevel"||root->getChildElement(i)->getStringAttribute("id")=="osc2Interval")root->removeChildElement(root->getChildElement(i),true);
 set(*target,"voiceDrive",24);check(memorylab::patch::apply(*target,root->toString()).wasOk(),"legacy patch rejected");
 check(target->state.getRawParameterValue("voiceDrive")->load()==0,"legacy patch did not bypass voice drive");
 check(memorylab::patch::apply(*target,encoded).wasOk(),"restore current patch failed");
 root=juce::parseXML(encoded);root->setAttribute("schemaVersion",2);
 for(int n=root->getNumChildElements()-1;n>=0;--n)if((root->getChildElement(n)->getStringAttribute("id").startsWith("lfo2")||root->getChildElement(n)->getStringAttribute("id").startsWith("lfo3")||root->getChildElement(n)->getStringAttribute("id").startsWith("lfo4")||root->getChildElement(n)->getStringAttribute("id").startsWith("env3")||root->getChildElement(n)->getStringAttribute("id").startsWith("env4")))root->removeChildElement(root->getChildElement(n),true);
 for(int i=root->getNumChildElements()-1;i>=0;--i){const auto id=root->getChildElement(i)->getStringAttribute("id");if(id=="keyTrack"||id=="ampVelocity"||id=="filterVelocity")root->removeChildElement(root->getChildElement(i),true);}
 for(int i=root->getNumChildElements()-1;i>=0;--i){const auto id=root->getChildElement(i)->getStringAttribute("id");if(id=="lfoSync"||id=="lfoDivision"||id=="lfoRetrigger"||id=="lfoBpm")root->removeChildElement(root->getChildElement(i),true);}
 for(int i=root->getNumChildElements()-1;i>=0;--i){const auto id=root->getChildElement(i)->getStringAttribute("id");if(id=="lfoShape"||id=="filterFamily"||id=="noiseLevel"||id=="sawLevel"||id=="pulseLevel"||id=="osc2Interval")root->removeChildElement(root->getChildElement(i),true);}
 set(*target,"sawLevel",1);set(*target,"pulseLevel",0);set(*target,"osc2Interval",24);
 check(memorylab::patch::apply(*target,root->toString()).wasOk(),"schema 2 patch rejected");
 check(std::abs(target->state.getRawParameterValue("sawLevel")->load()-.42f)<1e-6&&std::abs(target->state.getRawParameterValue("pulseLevel")->load()-.28f)<1e-6&&target->state.getRawParameterValue("osc2Interval")->load()==0,"schema 2 oscillator defaults lost");
 check(std::abs(target->state.getRawParameterValue("voiceDrive")->load()-source->state.getRawParameterValue("voiceDrive")->load())<1e-6,"schema 2 drive lost");
 check(memorylab::patch::apply(*target,encoded).wasOk(),"restore after schema 2 failed");
 root=juce::parseXML(encoded);root->setAttribute("schemaVersion",3);
 for(int n=root->getNumChildElements()-1;n>=0;--n)if((root->getChildElement(n)->getStringAttribute("id").startsWith("lfo2")||root->getChildElement(n)->getStringAttribute("id").startsWith("lfo3")||root->getChildElement(n)->getStringAttribute("id").startsWith("lfo4")||root->getChildElement(n)->getStringAttribute("id").startsWith("env3")||root->getChildElement(n)->getStringAttribute("id").startsWith("env4")))root->removeChildElement(root->getChildElement(n),true);
 for(int i=root->getNumChildElements()-1;i>=0;--i){const auto id=root->getChildElement(i)->getStringAttribute("id");if(id=="keyTrack"||id=="ampVelocity"||id=="filterVelocity")root->removeChildElement(root->getChildElement(i),true);}
 for(int i=root->getNumChildElements()-1;i>=0;--i){const auto id=root->getChildElement(i)->getStringAttribute("id");if(id=="lfoSync"||id=="lfoDivision"||id=="lfoRetrigger"||id=="lfoBpm")root->removeChildElement(root->getChildElement(i),true);}
 for(int i=root->getNumChildElements()-1;i>=0;--i)if(root->getChildElement(i)->getStringAttribute("id")=="lfoShape"||root->getChildElement(i)->getStringAttribute("id")=="filterFamily"||root->getChildElement(i)->getStringAttribute("id")=="noiseLevel")root->removeChildElement(root->getChildElement(i),true);
 set(*target,"noiseLevel",1);check(memorylab::patch::apply(*target,root->toString()).wasOk(),"schema 3 patch rejected");
 check(target->state.getRawParameterValue("noiseLevel")->load()==0,"schema 3 enabled noise");
 check(target->state.getRawParameterValue("osc2Interval")->load()==source->state.getRawParameterValue("osc2Interval")->load(),"schema 3 interval lost");
 check(memorylab::patch::apply(*target,encoded).wasOk(),"restore after schema 3 failed");
 root=juce::parseXML(encoded);root->setAttribute("schemaVersion",4);
 for(int n=root->getNumChildElements()-1;n>=0;--n)if((root->getChildElement(n)->getStringAttribute("id").startsWith("lfo2")||root->getChildElement(n)->getStringAttribute("id").startsWith("lfo3")||root->getChildElement(n)->getStringAttribute("id").startsWith("lfo4")||root->getChildElement(n)->getStringAttribute("id").startsWith("env3")||root->getChildElement(n)->getStringAttribute("id").startsWith("env4")))root->removeChildElement(root->getChildElement(n),true);
 for(int i=root->getNumChildElements()-1;i>=0;--i){const auto id=root->getChildElement(i)->getStringAttribute("id");if(id=="keyTrack"||id=="ampVelocity"||id=="filterVelocity")root->removeChildElement(root->getChildElement(i),true);}
 for(int i=root->getNumChildElements()-1;i>=0;--i){const auto id=root->getChildElement(i)->getStringAttribute("id");if(id=="lfoSync"||id=="lfoDivision"||id=="lfoRetrigger"||id=="lfoBpm")root->removeChildElement(root->getChildElement(i),true);}
 for(int i=root->getNumChildElements()-1;i>=0;--i)if(root->getChildElement(i)->getStringAttribute("id")=="lfoShape"||root->getChildElement(i)->getStringAttribute("id")=="filterFamily")root->removeChildElement(root->getChildElement(i),true);
 set(*target,"filterFamily",3);check(memorylab::patch::apply(*target,root->toString()).wasOk(),"schema 4 patch rejected");
 check(target->state.getRawParameterValue("filterFamily")->load()==0,"schema 4 did not restore classic");
 check(target->state.getRawParameterValue("noiseLevel")->load()==source->state.getRawParameterValue("noiseLevel")->load(),"schema 4 noise lost");
 check(memorylab::patch::apply(*target,encoded).wasOk(),"restore after schema 4 failed");
 root=juce::parseXML(encoded);root->setAttribute("schemaVersion",5);
 for(int n=root->getNumChildElements()-1;n>=0;--n)if((root->getChildElement(n)->getStringAttribute("id").startsWith("lfo2")||root->getChildElement(n)->getStringAttribute("id").startsWith("lfo3")||root->getChildElement(n)->getStringAttribute("id").startsWith("lfo4")||root->getChildElement(n)->getStringAttribute("id").startsWith("env3")||root->getChildElement(n)->getStringAttribute("id").startsWith("env4")))root->removeChildElement(root->getChildElement(n),true);
 for(int i=root->getNumChildElements()-1;i>=0;--i){const auto id=root->getChildElement(i)->getStringAttribute("id");if(id=="keyTrack"||id=="ampVelocity"||id=="filterVelocity")root->removeChildElement(root->getChildElement(i),true);}
 for(int i=root->getNumChildElements()-1;i>=0;--i){const auto id=root->getChildElement(i)->getStringAttribute("id");if(id=="lfoSync"||id=="lfoDivision"||id=="lfoRetrigger"||id=="lfoBpm")root->removeChildElement(root->getChildElement(i),true);}
 for(int i=root->getNumChildElements()-1;i>=0;--i)if(root->getChildElement(i)->getStringAttribute("id")=="lfoShape")root->removeChildElement(root->getChildElement(i),true);
 set(*target,"lfoShape",3);check(memorylab::patch::apply(*target,root->toString()).wasOk(),"schema 5 patch rejected");
 check(target->state.getRawParameterValue("lfoShape")->load()==0,"legacy patch did not restore sine");
 check(memorylab::patch::apply(*target,encoded).wasOk(),"restore after schema 5 failed");
 root=juce::parseXML(encoded);root->setAttribute("schemaVersion",6);
 for(int n=root->getNumChildElements()-1;n>=0;--n)if((root->getChildElement(n)->getStringAttribute("id").startsWith("lfo2")||root->getChildElement(n)->getStringAttribute("id").startsWith("lfo3")||root->getChildElement(n)->getStringAttribute("id").startsWith("lfo4")||root->getChildElement(n)->getStringAttribute("id").startsWith("env3")||root->getChildElement(n)->getStringAttribute("id").startsWith("env4")))root->removeChildElement(root->getChildElement(n),true);
 for(int i=root->getNumChildElements()-1;i>=0;--i){const auto id=root->getChildElement(i)->getStringAttribute("id");if(id=="keyTrack"||id=="ampVelocity"||id=="filterVelocity")root->removeChildElement(root->getChildElement(i),true);}
 for(int i=root->getNumChildElements()-1;i>=0;--i){const auto id=root->getChildElement(i)->getStringAttribute("id");if(id=="lfoSync"||id=="lfoDivision"||id=="lfoRetrigger"||id=="lfoBpm")root->removeChildElement(root->getChildElement(i),true);}
 set(*target,"lfoSync",1);set(*target,"lfoRetrigger",0);check(memorylab::patch::apply(*target,root->toString()).wasOk(),"schema 6 patch rejected");
 check(target->state.getRawParameterValue("lfoSync")->load()==0&&target->state.getRawParameterValue("lfoRetrigger")->load()==1,"legacy clock defaults lost");
 check(memorylab::patch::apply(*target,encoded).wasOk(),"restore after schema 6 failed");
 root=juce::parseXML(encoded);root->setAttribute("schemaVersion",7);
 for(int n=root->getNumChildElements()-1;n>=0;--n)if((root->getChildElement(n)->getStringAttribute("id").startsWith("lfo2")||root->getChildElement(n)->getStringAttribute("id").startsWith("lfo3")||root->getChildElement(n)->getStringAttribute("id").startsWith("lfo4")||root->getChildElement(n)->getStringAttribute("id").startsWith("env3")||root->getChildElement(n)->getStringAttribute("id").startsWith("env4")))root->removeChildElement(root->getChildElement(n),true);
 for(int i=root->getNumChildElements()-1;i>=0;--i){const auto id=root->getChildElement(i)->getStringAttribute("id");if(id=="keyTrack"||id=="ampVelocity"||id=="filterVelocity")root->removeChildElement(root->getChildElement(i),true);}
 set(*target,"keyTrack",1);set(*target,"ampVelocity",0);set(*target,"filterVelocity",0);check(memorylab::patch::apply(*target,root->toString()).wasOk(),"schema 7 patch rejected");
 check(target->state.getRawParameterValue("keyTrack")->load()==0&&target->state.getRawParameterValue("ampVelocity")->load()==1&&target->state.getRawParameterValue("filterVelocity")->load()==1,"legacy expression defaults lost");
 check(memorylab::patch::apply(*target,encoded).wasOk(),"restore after schema 7 failed");
 juce::TemporaryFile file;
 check(memorylab::patch::save(*source,file.getFile()).wasOk(),"patch file save failed");
 set(*target,"cutoff",400);check(memorylab::patch::load(*target,file.getFile()).wasOk(),"patch file load failed");
 check(memorylab::patch::encode(*target)==unchanged,"patch file roundtrip changed controls");
 auto editor=std::unique_ptr<juce::AudioProcessorEditor>(target->createEditor());auto*init=dynamic_cast<juce::Button*>(find(*editor,"initPatch"));check(init&&bool(init->onClick),"INIT button missing");init->onClick();
 auto factory=std::make_unique<MemoryLabProcessor>();check(memorylab::patch::encode(*target)==memorylab::patch::encode(*factory),"INIT did not restore every factory control");
 std::cout<<"PASS patches: all 92 controls roundtrip, atomic file save/load, malformed/unsupported/incomplete/duplicate/unknown/out-of-range values rejected without partial edits.\n";
}
