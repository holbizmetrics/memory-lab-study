#pragma once
void modernFilterChecks(){
 using namespace memorylab;
 double maxPeak=0;
 auto render=[&](double sr,int block,bool enabled,double profile,double bw,double drive,int family=0,bool moving=false){
  auto synth=std::make_unique<Synth>();check(synth->prepare(sr),"modern prepare");
  auto setp=[&](Param p,double v){synth->dispatch({0,EventKind::Parameter,0,v,0,p});};
  setp(Param::Drive,drive);setp(Param::Volume,.03);setp(Param::Q,8);setp(Param::Cutoff,1800);
  setp(Param::FilterFamily,family);setp(Param::ModernEnabled,enabled?1:0);setp(Param::ModernProfile,profile);setp(Param::ModernBandwidth,bw);
  setp(Param::NoiseLevel,.1);
  std::vector<float> settle(static_cast<std::size_t>(sr));synth->render(settle.data(),settle.size());
  for(int note:{48,52,55,59,60,64,67,71})synth->dispatch({0,EventKind::NoteOn,note,.8});
  const int edit=int(sr*.25),off=int(sr*.5),end=int(sr*.65);std::vector<float>out(end);
  for(int pos=0;pos<end;){
   if(pos==edit&&moving){setp(Param::ModernProfile,1-profile);setp(Param::ModernBandwidth,bw<1?4:.25);setp(Param::Cutoff,800);setp(Param::ModernEnabled,enabled?0:1);}
   if(pos==off)synth->dispatch({0,EventKind::AllSoundOff});
   int count=std::min(block,end-pos);if(pos<edit)count=std::min(count,edit-pos);if(pos<off)count=std::min(count,off-pos);
   synth->render(out.data()+pos,count);pos+=count;
  }
  for(float x:out){check(std::isfinite(x)&&std::abs(x)<4,"modern chain finite bound");maxPeak=std::max(maxPeak,double(std::abs(x)));}
  check(synth->activeVoices()==0,"modern all-sound-off voices");
  double tail=0;for(int i=end-100;i<end;++i)tail+=out[i]*out[i];check(tail<1e-8,"modern all-sound-off tail");
  return out;
 };
 for(double sr:{44100.,48000.,96000.,192000.}){
  for(int anchor=0;anchor<5;++anchor)for(double drive:{0.,5.})for(double bw:{.25,4.})
   render(sr,127,true,anchor*.25,bw,drive);
  check(render(sr,1,true,.1,.25,0,0,true)==render(sr,257,true,.1,.25,0,0,true),"modern automated block parity");
 }
 for(int family=0;family<4;++family)
  check(render(48000,127,false,0,.25,0,family)==render(48000,127,false,1,4,0,family),"disabled modern edits changed legacy sound");
 auto p=std::make_unique<MemoryLabProcessor>();set(*p,"modernEnabled",1);set(*p,"modernProfile",.25f);set(*p,"modernBandwidth",4);
 const auto saved=patch::encode(*p);auto old=juce::parseXML(saved);old->setAttribute("schemaVersion",13);stripParts(*old);stripModern(*old);
 check(patch::apply(*p,old->toString()).wasOk(),"schema13 modern migration");
 check(p->state.getRawParameterValue("modernEnabled")->load()==0&&p->state.getRawParameterValue("modernProfile")->load()==.5f&&p->state.getRawParameterValue("modernBandwidth")->load()==1,"old patch modern defaults");
 check(patch::apply(*p,saved).wasOk(),"modern patch recall");
 juce::MemoryBlock data;p->getStateInformation(data);auto recalled=std::make_unique<MemoryLabProcessor>();recalled->setStateInformation(data.getData(),int(data.getSize()));
 check(patch::encode(*p)==patch::encode(*recalled),"modern host recall");
 auto legacy=p->state.copyState();for(int i=legacy.getNumChildren()-1;i>=0;--i)if(legacy.getChild(i).getProperty("id").toString().startsWith("modern"))legacy.removeChild(i,nullptr);
 legacy.setProperty("version",15,nullptr);auto xml=legacy.createXml();juce::AudioProcessor::copyXmlToBinary(*xml,data);p->setStateInformation(data.getData(),int(data.getSize()));
 check(p->state.getRawParameterValue("modernEnabled")->load()==0,"legacy host retained modern enabled");
 auto editor=std::unique_ptr<juce::AudioProcessorEditor>(p->createEditor());
 auto click=[&](const char*id){auto*b=dynamic_cast<juce::Button*>(find(*editor,id));check(b&&bool(b->onClick),"modern page missing");b->onClick();};
 click("familyPage");click("modernFilterPage");
 for(auto*id:{"modernEnabled","modernProfile","modernBandwidth","cutoff","resonance","filterEnv","filterLfo","volume"})check(find(*editor,id),"modern binding missing");
 check(!find(*editor,"modernProfile")->isEnabled(),"disabled modern profile active");
 set(*p,"modernEnabled",1);click("modernFilterPage");check(find(*editor,"modernProfile")->isEnabled(),"enabled modern profile inactive");
 click("filterFamiliesPage");check(!find(*editor,"filterFamily")->isEnabled()&&!find(*editor,"profile")->isEnabled(),"overridden family controls active");
 click("modernFilterPage");set(*p,"modernProfile",.4f);const auto before=patch::encode(*p);
 for(int repeat=0;repeat<20;++repeat){click("voicePage");click("familyPage");click("modernFilterPage");}
 check(patch::encode(*p)==before,"modern navigation changed sound");
 std::cout<<"PASS modern filter integration: four rates, all anchors/BW extremes, 8 voices/Q8 drive off/on, automated block parity, legacy bypass/migration, patch/host recall, bindings/inactive controls; fixture peak="<<maxPeak<<" (not universal headroom)\n";
}

