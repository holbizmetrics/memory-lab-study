#include "Processor.hpp"
#include "Editor.hpp"

namespace {
struct Spec{const char* id;const char* name;float low,high,step;double initial;float skew;memorylab::Param parameter;};
const std::array<Spec,32> specs{{
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
 {"phaseReset","Oscillator phase reset",0,1,1,1,1,memorylab::Param::Retrigger},
 {"ampDelay","Amplitude delay (ms)",0,5000,1,0,.3f,memorylab::Param::AmpDelay},
 {"ampAttack","Amplitude attack (ms)",0,10000,1,12,.3f,memorylab::Param::AmpAttack},
 {"ampHold","Amplitude hold (ms)",0,5000,1,0,.3f,memorylab::Param::AmpHold},
 {"ampDecay","Amplitude decay (ms)",0,10000,1,180,.3f,memorylab::Param::AmpDecay},
 {"ampSustain","Amplitude sustain",0,1,.001f,.65,1,memorylab::Param::AmpSustain},
 {"ampRelease","Amplitude release (ms)",0,10000,1,500,.3f,memorylab::Param::AmpRelease},
 {"fltDelay","Filter delay (ms)",0,5000,1,0,.3f,memorylab::Param::FilterDelay},
 {"fltAttack","Filter attack (ms)",0,10000,1,20,.3f,memorylab::Param::FilterAttack},
 {"fltHold","Filter hold (ms)",0,5000,1,0,.3f,memorylab::Param::FilterHold},
 {"fltDecay","Filter decay (ms)",0,10000,1,350,.3f,memorylab::Param::FilterDecay},
 {"fltSustain","Filter sustain",0,1,.001f,.15,1,memorylab::Param::FilterSustain},
 {"fltRelease","Filter release (ms)",0,10000,1,400,.3f,memorylab::Param::FilterRelease},
 {"voiceDrive","Voice drive (dB): zero bypasses",0,24,.1f,0,1,memorylab::Param::VoiceDrive},
 {"sawLevel","Oscillator 1 saw level",0,1,.001f,.42,1,memorylab::Param::SawLevel},
 {"pulseLevel","Oscillator 2 pulse level",0,1,.001f,.28,1,memorylab::Param::PulseLevel},
 {"osc2Interval","Oscillator 2 interval (semitones)",-24,24,1,0,1,memorylab::Param::Osc2Interval}
}};
struct ArpSpec{const char*id;const char*name;float low,high,step,initial;};
const std::array<ArpSpec,7> arpSpecs{{
 {"arpOn","Arpeggiator on",0,1,1,0},
 {"arpSync","Arpeggiator host tempo",0,1,1,1},
 {"arpBpm","Arpeggiator internal BPM",20,400,.1f,130},
 {"arpRate","Arpeggiator rate: quarter/eighth/sixteenth/32nd/eighth triplet/sixteenth triplet",0,5,1,2},
 {"arpOrder","Arpeggiator order: up/down/up-down",0,2,1,0},
 {"arpOctaves","Arpeggiator octave range",1,3,1,1},
 {"arpGate","Arpeggiator gate (%)",5,95,1,50}
}};
}

juce::AudioProcessorValueTreeState::ParameterLayout MemoryLabProcessor::layout(){
 juce::AudioProcessorValueTreeState::ParameterLayout result;
 auto addSpec=[&](const Spec&s){
  result.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{s.id,1},s.name,
   juce::NormalisableRange<float>{s.low,s.high,s.step,s.skew},float(s.initial)));
 };
 // Keep the original 35 host indices stable. New controls append after the arp.
 for(std::size_t i=0;i<28;++i)addSpec(specs[i]);
 for(std::size_t i=0;i<arpSpecs.size();++i){const auto&s=arpSpecs[i];
  auto attributes=juce::AudioParameterFloatAttributes()
   .withStringFromValueFunction([i](float value,int){
    if(i<2)return juce::String(value>=.5f?"ON":"OFF");
    if(i==3){const char*names[]{"1/4","1/8","1/16","1/32","1/8 T","1/16 T"};return juce::String(names[juce::jlimit(0,5,int(value))]);}
    if(i==4){const char*names[]{"UP","DOWN","UP/DOWN"};return juce::String(names[juce::jlimit(0,2,int(value))]);}
    return juce::String(value,i==2?1:0);
   }).withValueFromStringFunction([i](const juce::String&text){
    const auto clean=text.trim().toUpperCase();
    if(i<2){if(clean=="ON")return 1.f;if(clean=="OFF")return 0.f;}
    if(i==3){const char*names[]{"1/4","1/8","1/16","1/32","1/8 T","1/16 T"};for(int n=0;n<6;++n)if(clean==names[n])return float(n);}
    if(i==4){const char*names[]{"UP","DOWN","UP/DOWN"};for(int n=0;n<3;++n)if(clean==names[n])return float(n);}
    return text.getFloatValue();
   });
  result.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{s.id,1},s.name,juce::NormalisableRange<float>{s.low,s.high,s.step},s.initial,attributes));
 }
 for(std::size_t i=28;i<specs.size();++i)addSpec(specs[i]);
 return result;
}
MemoryLabProcessor::MemoryLabProcessor():AudioProcessor(BusesProperties().withOutput("Output",juce::AudioChannelSet::stereo(),true)),state(*this,nullptr,"MemoryLabState",layout()){
 previous.fill(std::numeric_limits<float>::quiet_NaN());
 for(std::size_t i=0;i<specs.size();++i)values[i]=state.getRawParameterValue(specs[i].id);
 for(std::size_t i=0;i<arpSpecs.size();++i)arpValues[i]=state.getRawParameterValue(arpSpecs[i].id);
}
bool MemoryLabProcessor::isBusesLayoutSupported(const BusesLayout& b)const{
 return b.getMainInputChannelSet().isDisabled()&&(b.getMainOutputChannelSet()==juce::AudioChannelSet::mono()||b.getMainOutputChannelSet()==juce::AudioChannelSet::stereo());
}
void MemoryLabProcessor::prepareToPlay(double rate,int){ready=synth.prepare(rate);if(ready)renderSampleRate=rate;arp.reset();arpEnabled=false;wasPlaying=false;keyboard.reset();outputPeak=0;voiceCount=0;arpStep=0;previous.fill(std::numeric_limits<float>::quiet_NaN());setLatencySamples(0);}
void MemoryLabProcessor::arpEmit(memorylab::EventKind kind,int note,double velocity,int channel){synth.dispatch({0,kind,note,velocity,channel});}
void MemoryLabProcessor::render(float*audio,int count){
 if(!arpEnabled){synth.render(audio,std::size_t(count));return;}
 arp.render(count,renderSampleRate,clockBpm,int(arpValues[3]->load()),int(arpValues[4]->load()),int(arpValues[5]->load()),arpValues[6]->load()*.01,
  [this](auto kind,int note,double value,int channel){arpEmit(kind,note,value,channel);},
  [this,audio](int offset,int length){synth.render(audio+offset,std::size_t(length));});
}
juce::AudioProcessorEditor* MemoryLabProcessor::createEditor(){return new MemoryLabEditor(*this);}
void MemoryLabProcessor::midi(const juce::uint8*data,int length){
 if(length<1)return;const int kind=data[0]&0xf0,channel=data[0]&0xf;using namespace memorylab;
 auto dispatch=[&](EventKind k,int note,double value){synth.dispatch({0,k,note,value,channel});};
 auto emit=[this](auto k,int note,double value,int c){arpEmit(k,note,value,c);};
 if(arpEnabled){
  if((kind==0x80||kind==0x90)&&length>=3&&data[1]<128&&data[2]<128){arp.note(channel,data[1],kind==0x80?0.:data[2]/127.,emit);return;}
  if(kind==0xb0&&length>=3&&data[1]<128&&data[2]<128){
   if(data[1]==64){arp.sustain(channel,data[2]>=64,emit);return;}
   if(data[1]==120||data[1]==123)arp.clearChannel(channel,emit);
   if(data[1]==121)arp.sustain(channel,false,emit);
  }
 }
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
 const bool enabled=arpValues[0]->load()>=.5f;
 if(enabled!=arpEnabled){
  for(int c=0;c<16;++c){synth.dispatch({0,memorylab::EventKind::ChannelAllSoundOff,0,0,c});synth.dispatch({0,memorylab::EventKind::Sustain,0,0,c});}
  arp.reset();arpEnabled=enabled;
 }
 clockBpm=arpValues[2]->load();bool hostTempo=false,playing=false;
 if(auto*hostPlayHead=getPlayHead())if(auto position=hostPlayHead->getPosition()){
  playing=position->getIsPlaying();
  if(arpValues[1]->load()>=.5f)if(auto bpm=position->getBpm())if(std::isfinite(*bpm)&&*bpm>=20&&*bpm<=400){clockBpm=*bpm;hostTempo=true;}
 }
 if(arpEnabled&&playing!=wasPlaying){
  auto emit=[this](auto k,int note,double v,int c){arpEmit(k,note,v,c);};
  if(playing)arp.restart(emit);else{arp.stop(emit);arp.reset();}
 }
 wasPlaying=playing;arpTempo=float(clockBpm);arpHasHostTempo=hostTempo;
 keyboard.processNextMidiBuffer(events,0,count,true);
 for(const auto metadata:events){const int position=juce::jlimit(cursor,count,metadata.samplePosition);
  if(position>cursor)render(mono+cursor,position-cursor);
  midi(metadata.data,metadata.numBytes);cursor=position;
 }
 if(cursor<count)render(mono+cursor,count-cursor);
 arpStep=arp.position();
 for(int channel=1;channel<audio.getNumChannels();++channel)audio.copyFrom(channel,0,audio,0,0,count);
 outputPeak.store(count>0?audio.getMagnitude(0,0,count):0);voiceCount.store(int(synth.activeVoices()));
 events.clear();
}
void MemoryLabProcessor::getStateInformation(juce::MemoryBlock& dest){auto copy=state.copyState();copy.setProperty("version",5,nullptr);if(auto xml=copy.createXml())copyXmlToBinary(*xml,dest);}
void MemoryLabProcessor::setStateInformation(const void*data,int size){if(size<=0||size>1024*1024)return;if(auto xml=getXmlFromBinary(data,size))if(xml->hasTagName(state.state.getType())){
 auto incoming=juce::ValueTree::fromXml(*xml);
 // A missing new control in a legacy preset must take its original default,
 // rather than retaining a value edited in the current instance.
 for(std::size_t i=16;i<specs.size();++i)if(!incoming.getChildWithProperty("id",specs[i].id).isValid()){
  juce::ValueTree child("PARAM");child.setProperty("id",specs[i].id,nullptr);child.setProperty("value",float(specs[i].initial),nullptr);incoming.appendChild(child,nullptr);
 }
 for(const auto&s:arpSpecs)if(!incoming.getChildWithProperty("id",s.id).isValid()){
  juce::ValueTree child("PARAM");child.setProperty("id",s.id,nullptr);child.setProperty("value",s.initial,nullptr);incoming.appendChild(child,nullptr);
 }state.replaceState(incoming);
}}
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter(){return new MemoryLabProcessor();}
