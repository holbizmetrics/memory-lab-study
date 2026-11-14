#pragma once
// Cross-feature coverage, distinct from isolated feature tests and live DAW QA.
struct IntegrationPlayHead:juce::AudioPlayHead{
 double bpm=137.3;bool playing=true,tempo=true;
 juce::Optional<PositionInfo>getPosition()const override{PositionInfo p;p.setIsPlaying(playing);if(tempo)p.setBpm(bpm);return p;}
};
inline void integrationSetup(MemoryLabProcessor&p,int scenario){
 set(p,"partMode",scenario==1?2:3);set(p,"splitNote",60);set(p,"arpOn",scenario==2?1:0);set(p,"arpOctaves",2);set(p,"arpGate",72);
 for(int part=0;part<4;++part){
  auto v=[&](const char*id,float value){const auto full=MemoryLabProcessor::partId(part,id);set(p,full.toRawUTF8(),value);};
  set(p,("part"+juce::String(part+1)+"Channel").toRawUTF8(),scenario==0?float(part+1):0.f);
  v("volume",.025f);v("drive",scenario==0?4.f:0.f);v("voiceDrive",scenario==0?2.f:0.f);v("noiseLevel",.02f);
  v("cutoff",float(650+part*450));v("resonance",1.2f+part*.2f);v("modernEnabled",part%2?1.f:0.f);v("modernProfile",.15f+part*.2f);v("modernBandwidth",.5f+part*.6f);
  v("osc1PulseLevel",.1f);v("osc2SawLevel",.08f);v("osc2Interval",float(part*3));v("osc1Width",.2f+part*.15f);
  v("filterEnv",.8f);v("ampAttack",10+part*30.f);v("ampRelease",350);v("fltAttack",150);v("fltRelease",300);v("keyTrack",.5f);v("pressure",.4f);
  v("env3Attack",250);v("env3Release",200);v("env3Pitch",30);v("env3Cutoff",.3f);v("env3Pwm",.05f);
  v("env4Attack",400);v("env4Release",300);v("env4Pitch",-20);v("env4Cutoff",-.25f);v("env4Pwm",-.04f);v("ampPitch",10);v("fltPitch",30);
  v("vibrato",5);v("pwm",.03f);v("filterLfo",.2f);v("lfoAmp",.2f);v("lfoSync",1);v("lfoDivision",float(part%6));
  for(int source=2;source<=4;++source){const auto prefix="lfo"+juce::String(source);
   v((prefix+"Pitch").toRawUTF8(),float(source*3));v((prefix+"Pwm").toRawUTF8(),.02f);
   v((prefix+"Cutoff").toRawUTF8(),.15f);v((prefix+"Amp").toRawUTF8(),source%2?.1f:-.1f);
   v((prefix+"Shape").toRawUTF8(),float(source-1));v((prefix+"Sync").toRawUTF8(),1);v((prefix+"Division").toRawUTF8(),float((part+source)%6));
  }
 }
}
inline std::vector<float> integrationRender(MemoryLabProcessor&p,int sr,int block,int scenario){
 IntegrationPlayHead host;p.setPlayHead(&host);p.prepareToPlay(sr,block);
 struct Midi{int frame;juce::MidiMessage message;};std::vector<Midi>events;
 auto add=[&](double time,juce::MidiMessage m){events.push_back({int(time*sr),std::move(m)});};
 for(int ch=1;ch<=4;++ch){
  const int note=48+(ch-1)*8;
  add(.1,juce::MidiMessage::noteOn(ch,note,(juce::uint8)(70+ch*10)));
  add(.2,juce::MidiMessage::noteOn(ch,note+4,(juce::uint8)100));
  add(.5,juce::MidiMessage::pitchWheel(ch,10000));add(.6,juce::MidiMessage::channelPressureChange(ch,75));
  add(.7,juce::MidiMessage::controllerEvent(ch,64,127));
  add(.8,juce::MidiMessage::noteOff(ch,note));add(1.,juce::MidiMessage::noteOff(ch,note+4));
  add(1.2,juce::MidiMessage::controllerEvent(ch,64,0));
  add(1.5,juce::MidiMessage::allSoundOff(ch));
 }
 std::stable_sort(events.begin(),events.end(),[](const Midi&a,const Midi&b){return a.frame<b.frame;});
 const int total=sr*2,edit1=int(sr*.45),edit2=int(sr*.95),stop=sr*3/2;
 std::vector<float>out(total);juce::AudioBuffer<float>audio(2,block);juce::MidiBuffer midi;midi.ensureSize(4096);
 std::size_t next=0;int maxVoices=0;
 for(int pos=0;pos<total;){
  if(pos==edit1){set(p,"part2__modernProfile",.9f);set(p,"part3__lfo4Pitch",24);set(p,"part4__cutoff",3200);}
  if(pos==edit2){set(p,"part2__modernBandwidth",3);set(p,"part4__voiceDrive",0);}
  int n=std::min(block,total-pos);if(pos<edit1)n=std::min(n,edit1-pos);if(pos<edit2)n=std::min(n,edit2-pos);if(pos<stop)n=std::min(n,stop-pos);
  audio.setSize(2,n,false,false,true);midi.clear();
  while(next<events.size()&&events[next].frame<pos+n){midi.addEvent(events[next].message,events[next].frame-pos);++next;}
  // Exercise host-tempo loss/fallback and transport stop at common boundaries.
  host.tempo=pos<edit2;host.playing=pos<int(sr*1.5);
  p.processBlock(audio,midi);maxVoices=std::max(maxVoices,p.voiceCount.load());
  check(p.voiceCount.load()<=8,"combined feature voice budget");
  for(int i=0;i<n;++i){const float x=audio.getSample(0,i);check(std::isfinite(x)&&std::abs(x)<1,"combined feature finite/headroom");check(x==audio.getSample(1,i),"combined feature stereo mismatch");out[pos+i]=x;}
  pos+=n;
 }
 check(maxVoices>0,"combined feature never played");
 check(p.voiceCount.load()==0,"combined controller/arp panic stranded voice");
 check(p.lfoHasHostTempo.load()==false,"tempo fallback flag stuck");
 p.setPlayHead(nullptr);
 double energy=0,tail=0;for(float x:out)energy+=x*x;for(int i=total-100;i<total;++i)tail+=out[i]*out[i];
 check(energy>1e-5&&tail<1e-9,"combined output silent or panic tail");
 return out;
}
void integrationChecks(){
 double peak=0;
 for(int sr:{44100,48000,96000,192000})for(int scenario=0;scenario<3;++scenario){
  auto original=std::make_unique<MemoryLabProcessor>();integrationSetup(*original,scenario);
  const auto patchText=memorylab::patch::encode(*original);juce::MemoryBlock saved;original->getStateInformation(saved);
  auto fromHost=std::make_unique<MemoryLabProcessor>(),fromPatch=std::make_unique<MemoryLabProcessor>();
  fromHost->setStateInformation(saved.getData(),int(saved.getSize()));
  check(memorylab::patch::apply(*fromPatch,patchText).wasOk(),"combined patch decode");
  check(memorylab::patch::encode(*fromHost)==patchText&&memorylab::patch::encode(*fromPatch)==patchText,"combined setting recall mismatch");
  const auto a=integrationRender(*original,sr,127,scenario),b=integrationRender(*fromHost,sr,512,scenario),c=integrationRender(*fromPatch,sr,257,scenario);
  check(a==b&&a==c,"combined state/block partition mismatch");
  for(float x:a)peak=std::max(peak,double(std::abs(x)));
 }
 std::cout<<"PASS integration: 12 four-rate contexts with parts, oscillators/noise, filters, all LFOs/envelopes, drive/expression, arp/transport, tempo fallback and parameter edits; patch/host/block exact parity, eight-voice cap, finite audio and panic; fixture peak="<<peak<<"\n";
}

