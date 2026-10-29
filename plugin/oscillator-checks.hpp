#pragma once
void oscillatorChecks(){
 for(double sr:{44100.,48000.,96000.,192000.}){
  auto p=std::make_unique<MemoryLabProcessor>();p->prepareToPlay(sr,512);set(*p,"volume",.01f);set(*p,"drive",0);set(*p,"depth",0);
  juce::AudioBuffer<float>a(2,512);juce::MidiBuffer midi;
  for(int n=48;n<56;++n)midi.addEvent(juce::MidiMessage::noteOn(1,n,(juce::uint8)100),0);
  p->processBlock(a,midi);
  for(float interval:{-24.f,-12.f,7.f,12.f,24.f,0.f}){
   set(*p,"osc2Interval",interval);set(*p,"sawLevel",1);set(*p,"pulseLevel",1);
   for(int i=0;i<100;++i){midi.clear();p->processBlock(a,midi);check(power(a)>0,"oscillator automation silent");}
  }
  set(*p,"sawLevel",0);set(*p,"pulseLevel",0);
  for(int i=0;i<600;++i){midi.clear();p->processBlock(a,midi);power(a);}check(power(a)<1e-12,"muted mix did not settle to silence");
  set(*p,"sawLevel",.42f);for(int i=0;i<100;++i)p->processBlock(a,midi);check(power(a)>0,"unmuted held oscillator silent");
  midi.addEvent(juce::MidiMessage::controllerEvent(1,120,0),0);p->processBlock(a,midi);check(power(a)==0,"oscillator all-sound-off failed");
 }
 auto p=std::make_unique<MemoryLabProcessor>();set(*p,"sawLevel",.8f);set(*p,"pulseLevel",.9f);set(*p,"osc2Interval",-12);
 juce::MemoryBlock data;p->getStateInformation(data);auto restored=std::make_unique<MemoryLabProcessor>();restored->setStateInformation(data.getData(),int(data.getSize()));
 check(restored->state.getRawParameterValue("osc2Interval")->load()==-12,"oscillator state recall");
 auto legacy=p->state.copyState();for(int i=legacy.getNumChildren()-1;i>=0;--i){auto id=legacy.getChild(i).getProperty("id").toString();if(id=="sawLevel"||id=="pulseLevel"||id=="osc2Interval")legacy.removeChild(i,nullptr);}
 auto xml=legacy.createXml();juce::AudioProcessor::copyXmlToBinary(*xml,data);restored->setStateInformation(data.getData(),int(data.getSize()));
 check(std::abs(restored->state.getRawParameterValue("sawLevel")->load()-.42f)<1e-6&&std::abs(restored->state.getRawParameterValue("pulseLevel")->load()-.28f)<1e-6&&restored->state.getRawParameterValue("osc2Interval")->load()==0,"legacy oscillator defaults lost");
 auto full=std::make_unique<memorylab::Synth>(),split=std::make_unique<memorylab::Synth>();for(auto*s:{full.get(),split.get()}){s->prepare(48000);s->dispatch({0,memorylab::EventKind::NoteOn,52,.8});}
 const memorylab::Event events[]={{300,memorylab::EventKind::Parameter,0,7,0,memorylab::Param::Osc2Interval},{900,memorylab::EventKind::Parameter,0,0,0,memorylab::Param::SawLevel},{1600,memorylab::EventKind::Parameter,0,1,0,memorylab::Param::PulseLevel}};
 std::vector<float>a(8192),b(8192);full->render(a.data(),a.size(),events,3);
 for(int pos=0;pos<8192;pos+=127){int count=std::min(127,8192-pos);std::array<memorylab::Event,3>local{};int used=0;for(auto e:events)if(e.offset>=std::size_t(pos)&&e.offset<std::size_t(pos+count)){e.offset-=pos;local[used++]=e;}split->render(b.data()+pos,count,local.data(),used);}
 check(a==b,"oscillator automation block partition mismatch");
 std::cout<<"PASS oscillator integration: held eight-note edits at four rates, mute/unmute/all-off, state/legacy defaults, exact automated block partition parity\n";
}
