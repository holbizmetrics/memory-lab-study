#pragma once
void motionClockChecks(){
 struct Host:juce::AudioPlayHead{
  double bpm=120;bool available=true;
  juce::Optional<PositionInfo>getPosition()const override{PositionInfo p;if(available)p.setBpm(bpm);return p;}
 }host;
 auto p=std::make_unique<MemoryLabProcessor>();p->setPlayHead(&host);p->prepareToPlay(48000,512);
 set(*p,"lfoSync",1);set(*p,"lfoBpm",91);set(*p,"arpSync",0);
 juce::AudioBuffer<float>a(2,512);juce::MidiBuffer m;p->processBlock(a,m);
 check(p->lfoTempo.load()==120&&p->lfoHasHostTempo.load(),"LFO host tempo depends on arp sync");
 host.available=false;p->processBlock(a,m);check(p->lfoTempo.load()==91&&!p->lfoHasHostTempo.load(),"LFO fallback tempo");
 host.available=true;host.bpm=std::numeric_limits<double>::quiet_NaN();p->processBlock(a,m);check(p->lfoTempo.load()==91,"invalid host BPM accepted");
 host.bpm=500;p->processBlock(a,m);check(p->lfoTempo.load()==91,"out of range host BPM accepted");p->setPlayHead(nullptr);
 auto render=[](double sr,int block,int division,double bpm,bool retrigger,int delay){
  auto synth=std::make_unique<memorylab::Synth>();synth->prepare(sr);
  for(const auto&setting:std::initializer_list<std::pair<memorylab::Param,double>>{{memorylab::Param::Drive,0},{memorylab::Param::LfoSync,1},{memorylab::Param::LfoDivision,double(division)},{memorylab::Param::LfoBpm,bpm},{memorylab::Param::LfoRetrigger,retrigger?1.:0.},{memorylab::Param::CutoffLfo,1}})
   synth->dispatch({0,memorylab::EventKind::Parameter,0,setting.second,0,setting.first});
  std::vector<float>silence(std::size_t(sr)+delay);synth->render(silence.data(),silence.size());
  synth->dispatch({0,memorylab::EventKind::NoteOn,60,100/127.});
  const int length=int(sr*.2);std::vector<float>out(length);
  for(int pos=0;pos<length;pos+=block)synth->render(out.data()+pos,std::min(block,length-pos));
  for(float v:out)check(std::isfinite(v)&&std::abs(v)<1,"clock mode bad audio");return out;
 };
 for(double sr:{44100.,48000.,96000.,192000.}){
  for(int division=0;division<6;++division){const auto first=render(sr,512,division,120,false,0);check(first==render(sr,127,division,120,false,0),"free clock block parity");}
  check(render(sr,512,0,120,true,0)==render(sr,512,1,60,true,0),"equivalent tempo divisions differ");
  check(render(sr,512,0,120,true,0)==render(sr,512,0,120,true,int(sr*.125)),"note retrigger depends on preceding silence");
  check(render(sr,512,0,120,false,0)!=render(sr,512,0,120,false,int(sr*.125)),"free LFO did not advance through silence");
 }
 auto legacy=p->state.copyState();for(const char*id:{"lfoSync","lfoDivision","lfoRetrigger","lfoBpm"})legacy.removeChild(legacy.getChildWithProperty("id",id),nullptr);
 set(*p,"lfoSync",1);set(*p,"lfoRetrigger",0);auto xml=legacy.createXml();juce::MemoryBlock data;juce::AudioProcessor::copyXmlToBinary(*xml,data);p->setStateInformation(data.getData(),int(data.getSize()));
 check(p->state.getRawParameterValue("lfoSync")->load()==0&&p->state.getRawParameterValue("lfoRetrigger")->load()==1,"legacy host clock migration");
 auto editor=std::unique_ptr<juce::AudioProcessorEditor>(p->createEditor());auto*clock=dynamic_cast<juce::Button*>(find(*editor,"motionClockPage"));check(clock&&bool(clock->onClick),"clock subpage missing");clock->onClick();
 auto*rate=find(*editor,"lfoRate"),*division=find(*editor,"lfoDivision");check(rate&&division&&rate->isEnabled()&&!division->isEnabled(),"free mode enabled states");
 set(*p,"lfoSync",1);const auto deadline=juce::Time::getMillisecondCounterHiRes()+1000;
 while(rate->isEnabled()&&juce::Time::getMillisecondCounterHiRes()<deadline){
#if JUCE_WINDOWS
  uia_checks::pump();
#endif
  juce::Thread::sleep(5);juce::Timer::callPendingTimersSynchronously();
 }
 check(!rate->isEnabled()&&division->isEnabled(),"sync enabled states did not follow host");
 std::cout<<"PASS motion clock: host/fallback/invalid BPM, arp independence, six divisions at four sample rates, free-run silence, retrigger timing, block parity and host migration\n";
}
