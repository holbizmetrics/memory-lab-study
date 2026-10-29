#pragma once
void filterFamilyChecks(){
 for(double sr:{44100.,48000.,96000.,192000.}){
  auto p=std::make_unique<MemoryLabProcessor>();p->prepareToPlay(sr,512);set(*p,"volume",.005f);set(*p,"drive",0);set(*p,"depth",0);set(*p,"noiseLevel",.08f);
  juce::AudioBuffer<float>a(2,512);juce::MidiBuffer midi;for(int n=48;n<56;++n)midi.addEvent(juce::MidiMessage::noteOn(1,n,(juce::uint8)100),0);p->processBlock(a,midi);
  for(float mode:{1.f,2.f,3.f,0.f,3.f,1.f,0.f}){set(*p,"filterFamily",mode);for(int i=0;i<100;++i){midi.clear();p->processBlock(a,midi);check(power(a)>0,"family held notes silent");}}
  for(float mode:{0.f,1.f,2.f,3.f})for(float hz:{30.f,18000.f}){set(*p,"filterFamily",mode);set(*p,"cutoff",hz);set(*p,"resonance",8);for(int i=0;i<20;++i){midi.clear();p->processBlock(a,midi);power(a);}}
  midi.addEvent(juce::MidiMessage::controllerEvent(1,120,0),0);p->processBlock(a,midi);check(power(a)==0,"family all-off failed");
 }
 auto p=std::make_unique<MemoryLabProcessor>();set(*p,"filterFamily",3);set(*p,"noiseLevel",.35f);juce::MemoryBlock data;p->getStateInformation(data);
 auto restored=std::make_unique<MemoryLabProcessor>();restored->setStateInformation(data.getData(),int(data.getSize()));check(restored->state.getRawParameterValue("filterFamily")->load()==3,"family state recall");
 auto legacy=p->state.copyState();for(int i=legacy.getNumChildren()-1;i>=0;--i)if(legacy.getChild(i).getProperty("id").toString()=="filterFamily")legacy.removeChild(i,nullptr);
 auto xml=legacy.createXml();juce::AudioProcessor::copyXmlToBinary(*xml,data);restored->setStateInformation(data.getData(),int(data.getSize()));check(restored->state.getRawParameterValue("filterFamily")->load()==0,"legacy host state did not restore classic");check(std::abs(restored->state.getRawParameterValue("noiseLevel")->load()-.35f)<1e-6,"legacy noise changed");
 auto full=std::make_unique<memorylab::Synth>(),split=std::make_unique<memorylab::Synth>();for(auto*s:{full.get(),split.get()}){s->prepare(48000);s->dispatch({0,memorylab::EventKind::NoteOn,52,.8});}
 const memorylab::Event events[]={{300,memorylab::EventKind::Parameter,0,1,0,memorylab::Param::FilterFamily},{1900,memorylab::EventKind::Parameter,0,3,0,memorylab::Param::FilterFamily},{4000,memorylab::EventKind::Parameter,0,0,0,memorylab::Param::FilterFamily}};
 std::vector<float>a(8192),b(8192);full->render(a.data(),a.size(),events,3);
 for(int pos=0;pos<8192;pos+=127){int count=std::min(127,8192-pos);std::array<memorylab::Event,3>local{};int used=0;for(auto e:events)if(e.offset>=std::size_t(pos)&&e.offset<std::size_t(pos+count)){e.offset-=pos;local[used++]=e;}split->render(b.data()+pos,count,local.data(),used);}check(a==b,"family automation block partition mismatch");
 std::cout<<"PASS filter integration: held eight-note switches and high-Q edges at four rates, all-off, state/legacy defaults, exact block parity\n";
}
