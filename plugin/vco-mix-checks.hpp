#pragma once
void vcoMixChecks(){
 using namespace memorylab;
 auto render=[](double sr,int block,int source,double width=.5,double interval=0){
  auto synth=std::make_unique<Synth>();synth->prepare(sr);
  auto param=[&](Param p,double value){synth->dispatch({0,EventKind::Parameter,0,value,0,p});};
  param(Param::Drive,0);param(Param::Volume,.1);param(Param::Cutoff,3000);param(Param::Q,.8);
  param(Param::SawLevel,0);param(Param::PulseLevel,0);param(Param::Osc1Width,width);param(Param::Osc2Interval,interval);
  if(source==1||source==3)param(Param::Osc1PulseLevel,.4);
  if(source==2||source==3)param(Param::Osc2SawLevel,.4);
  // Like existing source levels, these controls smooth. Settle before starting
  // a fresh note so isolation does not include the old saw/pulse fade-out.
  std::vector<float>settling(static_cast<std::size_t>(sr));synth->render(settling.data(),settling.size());
  synth->dispatch({0,EventKind::NoteOn,60,.8});std::vector<float>out(int(sr*1.5));
  for(int pos=0;pos<int(out.size());){if(pos==int(sr*.7))synth->dispatch({0,EventKind::NoteOff,60});int count=std::min(block,int(out.size())-pos);if(pos<int(sr*.7))count=std::min(count,int(sr*.7)-pos);check(synth->render(out.data()+pos,count),"VCO mix render failed");pos+=count;}
  for(float x:out)check(std::isfinite(x)&&std::abs(x)<1,"VCO mix finite/headroom");check(synth->activeVoices()==0,"VCO mix stuck release");return out;
 };
 for(double sr:{44100.,48000.,96000.,192000.}){
  auto silence=render(sr,127,0);check(std::all_of(silence.begin(),silence.end(),[](float x){return x==0;}),"muted new mixer not silent");
  for(int source:{1,2,3}){check(render(sr,127,source)!=silence,"added waveform silent");check(render(sr,127,source)==render(sr,512,source),"VCO mix block parity");}
  check(render(sr,127,1,.2)!=render(sr,127,1,.5),"OSC 1 width had no effect");
  check(render(sr,127,2,.2)==render(sr,127,2,.5),"OSC 1 width affected OSC 2 saw");
  check(render(sr,127,1,.5,12)==render(sr,127,1,.5,0),"OSC 2 tuning affected OSC 1 pulse");
  check(render(sr,127,2,.5,12)!=render(sr,127,2,.5,0),"OSC 2 tuning missed OSC 2 saw");
 }
 auto p=std::make_unique<MemoryLabProcessor>();set(*p,"osc1PulseLevel",.3f);set(*p,"osc2SawLevel",.2f);set(*p,"osc1Width",.25f);
 const auto encoded=patch::encode(*p);auto old=juce::parseXML(encoded);old->setAttribute("schemaVersion",12);stripModern(*old);
 for(int i=old->getNumChildElements()-1;i>=0;--i){const auto id=old->getChildElement(i)->getStringAttribute("id");if(id=="osc1PulseLevel"||id=="osc2SawLevel"||id=="osc1Width")old->removeChildElement(old->getChildElement(i),true);}
 check(patch::apply(*p,old->toString()).wasOk(),"schema 12 migration failed");check(p->state.getRawParameterValue("osc1PulseLevel")->load()==0&&p->state.getRawParameterValue("osc2SawLevel")->load()==0,"old patch retained added waveforms");
 check(patch::apply(*p,encoded).wasOk(),"VCO mixer patch recall");juce::MemoryBlock state;p->getStateInformation(state);auto q=std::make_unique<MemoryLabProcessor>();q->setStateInformation(state.getData(),int(state.getSize()));check(patch::encode(*q)==patch::encode(*p),"VCO mixer host recall");
 for(double sr:{44100.,48000.,96000.}){
  p->prepareToPlay(sr,127);juce::AudioBuffer<float>audio(2,127);juce::MidiBuffer midi;set(*p,"volume",.01f);set(*p,"drive",0);
  for(int note=48;note<56;++note)midi.addEvent(juce::MidiMessage::noteOn(1,note,(juce::uint8)100),0);p->processBlock(audio,midi);midi.clear();
  for(int turn=0;turn<100;++turn){set(*p,"osc1Width",turn%2?.1f:.9f);set(*p,"osc1PulseLevel",turn%2?0.f:1.f);set(*p,"osc2SawLevel",turn%2?1.f:0.f);p->processBlock(audio,midi);power(audio);}
  midi.addEvent(juce::MidiMessage::controllerEvent(1,120,0),0);p->processBlock(audio,midi);check(power(audio)==0,"new waveforms ignored all sound off");
 }
 auto editor=std::unique_ptr<juce::AudioProcessorEditor>(p->createEditor());auto*b=dynamic_cast<juce::Button*>(find(*editor,"oscMixPage"));check(b&&bool(b->onClick),"OSC MIX page missing");b->onClick();for(auto*id:{"sawLevel","pulseLevel","osc1PulseLevel","osc2SawLevel","osc1Width","width","noiseLevel","volume"})check(find(*editor,id),"OSC MIX binding missing");
 std::cout<<"PASS two VCO mixer: solo/combined sources, separate width/tuning, four sample rates, exact block parity, mute/release, held eight-note edits/all-off, schema 12 and host recall, editor bindings\n";
}
