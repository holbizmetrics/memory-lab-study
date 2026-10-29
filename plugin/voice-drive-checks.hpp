#pragma once

void voiceDriveChecks(){
 // Enabled drive must change the signal, with identical results across block sizes.
 auto full=std::make_unique<memorylab::Synth>(),split=std::make_unique<memorylab::Synth>(),bypass=std::make_unique<memorylab::Synth>();
 for(auto*s:{full.get(),split.get(),bypass.get()}){s->prepare(48000);s->dispatch({0,memorylab::EventKind::Parameter,0,0,0,memorylab::Param::Drive});s->dispatch({0,memorylab::EventKind::NoteOn,60,.8});}
 full->dispatch({0,memorylab::EventKind::Parameter,0,24,0,memorylab::Param::VoiceDrive});split->dispatch({0,memorylab::EventKind::Parameter,0,24,0,memorylab::Param::VoiceDrive});
 std::vector<float>a(8192),b(8192),c(8192);full->render(a.data(),a.size());bypass->render(c.data(),c.size());
 for(int pos=0;pos<8192;pos+=127)split->render(b.data()+pos,std::min(127,8192-pos));
 check(a==b,"voice drive block partition mismatch");check(a!=c,"voice drive has no effect");
 for(double sr:{44100.,48000.,96000.,192000.}){
  auto p=std::make_unique<MemoryLabProcessor>();p->prepareToPlay(sr,512);
  set(*p,"volume",.05f);set(*p,"drive",0);set(*p,"depth",0);
  juce::AudioBuffer<float> audio(2,512);juce::MidiBuffer midi;
  for(int note=48;note<56;++note)midi.addEvent(juce::MidiMessage::noteOn(1,note,(juce::uint8)100),0);
  p->processBlock(audio,midi);
  for(float db:{0.f,8.f,24.f,0.f}){
   set(*p,"voiceDrive",db);
   for(int block=0;block<100;++block){midi.clear();p->processBlock(audio,midi);check(power(audio)>0,"drive held notes silent");}
  }
  midi.addEvent(juce::MidiMessage::controllerEvent(1,120,0),0);p->processBlock(audio,midi);check(power(audio)==0,"voice drive all-off failed");
 }
 auto p=std::make_unique<MemoryLabProcessor>();set(*p,"voiceDrive",24);
 juce::MemoryBlock data;p->getStateInformation(data);
 auto restored=std::make_unique<MemoryLabProcessor>();restored->setStateInformation(data.getData(),int(data.getSize()));
 check(restored->state.getRawParameterValue("voiceDrive")->load()==24,"voice drive state recall failed");
 auto legacy=p->state.copyState();for(int i=legacy.getNumChildren()-1;i>=0;--i)if(legacy.getChild(i).getProperty("id").toString()=="voiceDrive")legacy.removeChild(i,nullptr);
 auto xml=legacy.createXml();juce::AudioProcessor::copyXmlToBinary(*xml,data);restored->setStateInformation(data.getData(),int(data.getSize()));
 check(restored->state.getRawParameterValue("voiceDrive")->load()==0,"legacy host state enabled voice drive");
 std::cout<<"PASS voice drive: held eight-note automation at four sample rates, finite audio with fixture headroom, all-off, state recall and legacy bypass\n";
}
