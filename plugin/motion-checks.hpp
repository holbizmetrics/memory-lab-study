#pragma once
#include <vector>
void motionChecks(){
 // Known waveform polarity and wrap points; pulse/saw jumps are intentional.
 for(int shape=0;shape<4;++shape)for(int i=0;i<1000;++i){const double v=memorylab::lfoSample(i/1000.,shape);check(std::isfinite(v)&&std::abs(v)<=1,"LFO range");check(std::abs(v-memorylab::lfoSample(1+i/1000.,shape))<1e-12,"LFO period");}
 check(memorylab::lfoSample(0,1)==-1&&memorylab::lfoSample(.5,1)==1,"triangle polarity");
 check(memorylab::lfoSample(.25,2)==-.5&&memorylab::lfoSample(.75,2)==.5,"saw polarity");
 check(memorylab::lfoSample(.25,3)==1&&memorylab::lfoSample(.75,3)==-1,"pulse polarity");
 auto render=[](double sr,int block,int shape,bool modulation){
  auto p=std::make_unique<MemoryLabProcessor>();p->prepareToPlay(sr,block);set(*p,"lfoShape",float(shape));
  set(*p,"drive",0);if(modulation){set(*p,"vibrato",25);set(*p,"pwm",.2f);set(*p,"filterLfo",1);}
  const int length=int(sr*.8);std::vector<float> result;result.reserve(length);
  for(int pos=0;pos<length;pos+=block){const int count=std::min(block,length-pos);juce::AudioBuffer<float>a(2,count);juce::MidiBuffer m;
   if(pos==0)m.addEvent(juce::MidiMessage::noteOn(1,60,(juce::uint8)100),0);
   p->processBlock(a,m);power(a);for(int i=0;i<count;++i)result.push_back(a.getSample(0,i));
  }return result;
 };
 for(double sr:{44100.,48000.,96000.,192000.}){
  const auto sine=render(sr,512,0,true),unmodulated=render(sr,512,0,false);
  for(int shape=0;shape<4;++shape){const auto a=render(sr,512,shape,true),b=render(sr,127,shape,true);
   check(a==b,"LFO block partition changed audio");
   if(shape)check(a!=sine,"selected LFO shape did not affect modulation");
   check(render(sr,512,shape,false)==unmodulated,"LFO shape changed zero-depth sound");
  }
 }
 auto p=std::make_unique<MemoryLabProcessor>();set(*p,"lfoShape",3);juce::MemoryBlock data;p->getStateInformation(data);
 auto restored=std::make_unique<MemoryLabProcessor>();restored->setStateInformation(data.getData(),int(data.getSize()));check(restored->state.getRawParameterValue("lfoShape")->load()==3,"LFO host state recall");
 auto legacy=p->state.copyState();legacy.removeChild(legacy.getChildWithProperty("id","lfoShape"),nullptr);auto xml=legacy.createXml();juce::AudioProcessor::copyXmlToBinary(*xml,data);
 restored->setStateInformation(data.getData(),int(data.getSize()));check(restored->state.getRawParameterValue("lfoShape")->load()==0,"legacy host state did not restore sine");
 std::cout<<"PASS motion: waveform polarity/range/period, four sample rates, block parity, zero-depth neutrality and host-state migration\n";
}
