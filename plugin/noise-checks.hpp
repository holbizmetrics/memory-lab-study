#pragma once
void noiseChecks(){
 for(double sr:{44100.,48000.,96000.,192000.}){
  auto p=std::make_unique<MemoryLabProcessor>();p->prepareToPlay(sr,512);set(*p,"volume",.01f);set(*p,"drive",0);set(*p,"depth",0);set(*p,"sawLevel",0);set(*p,"pulseLevel",0);
  juce::AudioBuffer<float>a(2,512);juce::MidiBuffer midi;
  // Settle source mutes before starting the noise-only fixture.
  for(int i=0;i<200;++i)p->processBlock(a,midi);
  for(int n=48;n<56;++n)midi.addEvent(juce::MidiMessage::noteOn(1,n,(juce::uint8)100),0);p->processBlock(a,midi);check(power(a)==0,"noise enabled by default");
  for(float level:{.08f,.35f,1.f}){set(*p,"noiseLevel",level);for(int i=0;i<100;++i){midi.clear();p->processBlock(a,midi);check(power(a)>0,"noise-only held notes silent");}}
  set(*p,"noiseLevel",0);for(int i=0;i<600;++i){midi.clear();p->processBlock(a,midi);power(a);}check(power(a)<1e-12,"noise mute did not settle");
  set(*p,"noiseLevel",.35f);for(int i=0;i<100;++i)p->processBlock(a,midi);check(power(a)>0,"noise re-enable silent");
  midi.addEvent(juce::MidiMessage::controllerEvent(1,120,0),0);p->processBlock(a,midi);check(power(a)==0,"noise all-off failed");
 }
 auto p=std::make_unique<MemoryLabProcessor>();set(*p,"noiseLevel",.35f);juce::MemoryBlock data;p->getStateInformation(data);
 auto restored=std::make_unique<MemoryLabProcessor>();restored->setStateInformation(data.getData(),int(data.getSize()));check(std::abs(restored->state.getRawParameterValue("noiseLevel")->load()-.35f)<1e-6,"noise state recall");
 auto legacy=p->state.copyState();for(int i=legacy.getNumChildren()-1;i>=0;--i)if(legacy.getChild(i).getProperty("id").toString()=="noiseLevel")legacy.removeChild(i,nullptr);
 auto xml=legacy.createXml();juce::AudioProcessor::copyXmlToBinary(*xml,data);restored->setStateInformation(data.getData(),int(data.getSize()));check(restored->state.getRawParameterValue("noiseLevel")->load()==0,"legacy host state enabled noise");
 auto full=std::make_unique<memorylab::Synth>(),split=std::make_unique<memorylab::Synth>();for(auto*s:{full.get(),split.get()}){s->prepare(48000);for(int n:{52,55,59})s->dispatch({0,memorylab::EventKind::NoteOn,n,.8});}
 const memorylab::Event events[]={{300,memorylab::EventKind::Parameter,0,.35,0,memorylab::Param::NoiseLevel},{1900,memorylab::EventKind::Parameter,0,0,0,memorylab::Param::NoiseLevel},{4000,memorylab::EventKind::AllOff}};
 std::vector<float>a(8192),b(8192);full->render(a.data(),a.size(),events,3);
 for(int pos=0;pos<8192;pos+=127){int count=std::min(127,8192-pos);std::array<memorylab::Event,3>local{};int used=0;for(auto e:events)if(e.offset>=std::size_t(pos)&&e.offset<std::size_t(pos+count)){e.offset-=pos;local[used++]=e;}split->render(b.data()+pos,count,local.data(),used);}
 check(a==b,"noise automation block partition mismatch");
 std::cout<<"PASS noise integration: default silence with oscillators muted, held-note edits at four rates, mute/re-enable/all-off, state/legacy migration, exact block partition parity\n";
}
