#pragma once
namespace preset_ui_checks {
 const juce::Colour panel{0xff1a2729},white{0xffe5eee8},muted{0xff91a9a1},mint{0xffb9e7cb};
 #include "PresetPanel.hpp"
}
void routingChecks(){
 using namespace memorylab;
 const std::array<Param,8> routes{Param::LfoAmp,Param::Lfo2Amp,Param::Lfo3Amp,Param::Lfo4Amp,Param::AmpPitch,Param::FltPitch,Param::Env3Pwm,Param::Env4Pwm};
 auto render=[&](double sr,int block,int route,double sign,bool combined=false){
  auto s=std::make_unique<Synth>();s->prepare(sr);
  auto setp=[&](Param p,double x){s->dispatch({0,EventKind::Parameter,0,x,0,p});};
  setp(Param::Drive,0);setp(Param::Cutoff,1500);setp(Param::Q,.8);
  for(int i=0;i<8;++i)if(combined||i==route)setp(routes[std::size_t(i)],sign*(i<4?.8:i<6?200:.2));
  s->dispatch({0,EventKind::NoteOn,60,.8});std::vector<float>out(int(sr*1.3));
  for(int pos=0;pos<int(out.size());){if(pos==int(sr*.7))s->dispatch({0,EventKind::NoteOff,60});int count=std::min(block,int(out.size())-pos);if(pos<int(sr*.7))count=std::min(count,int(sr*.7)-pos);check(s->render(out.data()+pos,count),"routing render failed");pos+=count;}
  for(float x:out)check(std::isfinite(x)&&std::abs(x)<1,"routing finite/headroom failed");check(s->activeVoices()==0,"routing retained voice after amp release");return out;
 };
 for(double sr:{44100.,48000.,96000.,192000.}){
  auto base=render(sr,127,-1,1);
  for(int i=0;i<8;++i){auto plus=render(sr,127,i,1),minus=render(sr,127,i,-1);check(plus!=base&&minus!=base,"new route had no effect");check(plus!=minus,"signed route polarity ignored");check(plus==render(sr,512,i,1),"route block parity failed");}
  check(render(sr,127,-1,1,true)==render(sr,512,-1,1,true),"combined routing block parity");
 }
 auto p=std::make_unique<MemoryLabProcessor>();const char*ids[]{"lfoAmp","lfo2Amp","lfo3Amp","lfo4Amp","ampPitch","fltPitch","env3Pwm","env4Pwm"};
 for(auto*id:ids)set(*p,id,juce::String(id).contains("Pitch")?123:.2f);
 const auto encoded=patch::encode(*p);auto old=juce::parseXML(encoded);old->setAttribute("schemaVersion",11);stripModern(*old);
 for(int i=old->getNumChildElements()-1;i>=0;--i){const auto id=old->getChildElement(i)->getStringAttribute("id");if(id=="osc1PulseLevel"||id=="osc2SawLevel"||id=="osc1Width")old->removeChildElement(old->getChildElement(i),true);}
 for(int i=old->getNumChildElements()-1;i>=0;--i)for(auto*id:ids)if(old->getChildElement(i)->getStringAttribute("id")==id){old->removeChildElement(old->getChildElement(i),true);break;}
 // Normalised float roundtrips can leave sub-micro-unit PWM residuals; the
 // wrapper canonicalises defaults before DSP dispatch. Audio parity is exact.
 check(patch::apply(*p,old->toString()).wasOk(),"schema 11 migration failed");for(auto*id:ids)check(std::abs(p->state.getRawParameterValue(id)->load())<1e-6,"old patch retained new route");
 check(patch::apply(*p,encoded).wasOk(),"routing patch recall failed");juce::MemoryBlock state;p->getStateInformation(state);auto q=std::make_unique<MemoryLabProcessor>();q->setStateInformation(state.getData(),int(state.getSize()));check(patch::encode(*q)==patch::encode(*p),"routing host recall failed");
 // A private preference file prevents test clicks from changing Holger's preference.
 juce::TemporaryFile settings;PatchLocation first(settings.getFile());check(!first.autoLoadPresets(),"auto-load not off by default");
 auto panel=std::make_unique<preset_ui_checks::PresetPanel>(*p,settings.getFile());
 auto*toggle=dynamic_cast<juce::Button*>(find(*panel,"presetAutoLoad"));auto*sound=dynamic_cast<juce::ComboBox*>(find(*panel,"presetSound"));auto*category=dynamic_cast<juce::ComboBox*>(find(*panel,"presetCategory"));auto*undo=dynamic_cast<juce::Button*>(find(*panel,"undoFactoryPreset"));check(toggle&&sound&&category&&undo,"preset auto-load controls missing");
 const auto before=patch::encode(*p);toggle->setToggleState(true,juce::dontSendNotification);toggle->onClick();check(patch::encode(*p)==before,"enabling auto-load loaded a sound");
 for(int id:{2,4,1}){category->setSelectedId(id,juce::dontSendNotification);category->onChange();check(patch::encode(*p)==before,"category loaded a sound with auto-load on");}
 set(*p,"volume",.137f);const auto beforeLoad=patch::encode(*p);
 sound->setSelectedId(2,juce::dontSendNotification);sound->onChange();check(patch::encode(*p)!=beforeLoad,"auto-load selection did not load");check(std::abs(p->state.getRawParameterValue("volume")->load()-.137f)<1e-6,"auto-load changed output");check(p->state.getRawParameterValue("drive")->load()==0&&p->state.getRawParameterValue("voiceDrive")->load()==0,"auto-load did not bypass drives");
 const auto loaded=patch::encode(*p);sound->setSelectedId(3,juce::dontSendNotification);sound->onChange();undo->onClick();check(patch::encode(*p)==loaded,"auto-load undo did not restore latest previous sound");
 toggle->setToggleState(false,juce::dontSendNotification);toggle->onClick();const auto passive=patch::encode(*p);sound->setSelectedId(4,juce::dontSendNotification);sound->onChange();check(patch::encode(*p)==passive,"auto-load off changed sound");
 toggle->setToggleState(true,juce::dontSendNotification);toggle->onClick();panel.reset();
 auto reopened=std::make_unique<preset_ui_checks::PresetPanel>(*p,settings.getFile());auto*restored=dynamic_cast<juce::Button*>(find(*reopened,"presetAutoLoad"));check(restored&&restored->getToggleState(),"auto-load preference did not persist");check(patch::encode(*p)==passive,"opening panel with saved auto-load changed sound");
 // Reloading preference objects must not erase the independent patch-folder key.
 check(first.rememberSuccess(juce::File::getCurrentWorkingDirectory().getChildFile("test.mlpatch")),"folder preference write");check(first.autoLoadPresets(),"folder preference erased auto-load");
 std::cout<<"PASS routing: eight signed destinations, four sample rates, block/release parity, old/new patch and host recall; auto-load defaults, explicit selection, passive categories/toggle, output/drives, latest Undo and private preference persistence\n";
}
