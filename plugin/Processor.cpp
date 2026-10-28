#include "Processor.hpp"

namespace {
struct Spec{const char* id;const char* name;float low,high,step;double initial;float skew;memorylab::Param parameter;};
const std::array<Spec,16> specs{{
 {"drive","Drive",0,12,.1f,5,1,memorylab::Param::Drive},
 {"depth","Memory depth",0,1.5f,.01f,.8,1,memorylab::Param::Depth},
 {"recovery","Memory recovery (ms)",50,400,1,250,.5f,memorylab::Param::Recovery},
 {"detune","Oscillator detune (cents)",-30,30,.1f,7,1,memorylab::Param::Detune},
 {"width","Pulse width",.1f,.9f,.001f,.43,1,memorylab::Param::PulseWidth},
 {"cutoff","Filter cutoff (Hz)",30,18000,1,1450,.3f,memorylab::Param::Cutoff},
 {"resonance","Filter Q",.5f,8,.01f,2.4,.5f,memorylab::Param::Q},
 {"volume","Output gain",0,.8f,.001f,.25f,1,memorylab::Param::Volume},
 {"profile","Profile: 0 HP / 0.5 notch / 1 LP",0,1,.001f,1,1,memorylab::Param::Profile},
 {"lfoRate","LFO rate (Hz)",.05f,20,.01f,4.5f,.4f,memorylab::Param::LfoRate},
 {"vibrato","Pitch LFO (cents)",-50,50,.1f,0,1,memorylab::Param::PitchLfo},
 {"pwm","Pulse width LFO depth",-.35f,.35f,.001f,0,1,memorylab::Param::PwmLfo},
 {"filterLfo","Cutoff LFO (octaves)",-2,2,.01f,0,1,memorylab::Param::CutoffLfo},
 {"filterEnv","Filter envelope (octaves)",-4,4,.01f,0,1,memorylab::Param::FilterEnvelope},
 {"pressure","Pressure to cutoff (octaves)",-2,2,.01f,0,1,memorylab::Param::PressureDepth},
 {"phaseReset","Oscillator phase reset",0,1,1,1,1,memorylab::Param::Retrigger}
}};
}

juce::AudioProcessorValueTreeState::ParameterLayout MemoryLabProcessor::layout(){
 juce::AudioProcessorValueTreeState::ParameterLayout result;
 for(std::size_t i=0;i<specs.size();++i){const auto&s=specs[i];
  result.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{s.id,1},s.name,
   juce::NormalisableRange<float>{s.low,s.high,s.step,s.skew},float(s.initial)));
 }return result;
}
MemoryLabProcessor::MemoryLabProcessor():AudioProcessor(BusesProperties().withOutput("Output",juce::AudioChannelSet::stereo(),true)),state(*this,nullptr,"MemoryLabState",layout()){
 previous.fill(std::numeric_limits<float>::quiet_NaN());
 for(std::size_t i=0;i<specs.size();++i)values[i]=state.getRawParameterValue(specs[i].id);
}
bool MemoryLabProcessor::isBusesLayoutSupported(const BusesLayout& b)const{
 return b.getMainInputChannelSet().isDisabled()&&(b.getMainOutputChannelSet()==juce::AudioChannelSet::mono()||b.getMainOutputChannelSet()==juce::AudioChannelSet::stereo());
}
void MemoryLabProcessor::prepareToPlay(double rate,int){ready=synth.prepare(rate);previous.fill(std::numeric_limits<float>::quiet_NaN());setLatencySamples(0);}
void MemoryLabProcessor::midi(const juce::uint8*data,int length){
 if(length<1)return;const int kind=data[0]&0xf0,channel=data[0]&0xf;using namespace memorylab;
 auto dispatch=[&](EventKind k,int note,double value){synth.dispatch({0,k,note,value,channel});};
 if((kind==0x80||kind==0x90)&&length>=3&&data[1]<128&&data[2]<128)dispatch(kind==0x80||data[2]==0?EventKind::NoteOff:EventKind::NoteOn,data[1],data[2]/127.);
 else if(kind==0xe0&&length>=3&&data[1]<128&&data[2]<128){const int bend=(data[1]|(data[2]<<7))-8192;dispatch(EventKind::PitchBend,0,bend/double(bend>=0?8191:8192));}
 else if(kind==0xd0&&length>=2&&data[1]<128)dispatch(EventKind::ChannelPressure,0,data[1]/127.);
 else if(kind==0xb0&&length>=3&&data[1]<128&&data[2]<128){
  if(data[1]==64)dispatch(EventKind::Sustain,0,data[2]>=64?1:0);
  else if(data[1]==123)dispatch(EventKind::ChannelAllOff,0,0);
  else if(data[1]==120)dispatch(EventKind::ChannelAllSoundOff,0,0);
  else if(data[1]==121){dispatch(EventKind::Sustain,0,0);dispatch(EventKind::PitchBend,0,0);dispatch(EventKind::ChannelPressure,0,0);}
 }
}
void MemoryLabProcessor::processBlock(juce::AudioBuffer<float>& audio,juce::MidiBuffer& events){
 juce::ScopedNoDenormals noDenormals;audio.clear();if(!ready||audio.getNumChannels()==0){events.clear();return;}
 for(std::size_t i=0;i<specs.size();++i){const float value=values[i]->load();if(value!=previous[i]){
  // APVTS normalised float roundtrips may shift a default by a few ulps.
  // Canonicalise only within a fraction of its displayed control increment.
  const double tolerance=std::max(1e-6,double(specs[i].step)*.001);
  const double nativeValue=std::abs(double(value)-specs[i].initial)<tolerance?specs[i].initial:double(value);
  synth.dispatch({0,memorylab::EventKind::Parameter,0,nativeValue,0,specs[i].parameter});previous[i]=value;
 }}
 const int count=audio.getNumSamples();float* mono=audio.getWritePointer(0);int cursor=0;
 for(const auto metadata:events){const int position=juce::jlimit(cursor,count,metadata.samplePosition);
  if(position>cursor)synth.render(mono+cursor,std::size_t(position-cursor));
  midi(metadata.data,metadata.numBytes);cursor=position;
 }
 if(cursor<count)synth.render(mono+cursor,std::size_t(count-cursor));
 for(int channel=1;channel<audio.getNumChannels();++channel)audio.copyFrom(channel,0,audio,0,0,count);
 events.clear();
}
void MemoryLabProcessor::getStateInformation(juce::MemoryBlock& dest){auto copy=state.copyState();copy.setProperty("version",1,nullptr);if(auto xml=copy.createXml())copyXmlToBinary(*xml,dest);}
void MemoryLabProcessor::setStateInformation(const void*data,int size){if(size<=0||size>1024*1024)return;if(auto xml=getXmlFromBinary(data,size))if(xml->hasTagName(state.state.getType()))state.replaceState(juce::ValueTree::fromXml(*xml));}
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter(){return new MemoryLabProcessor();}
