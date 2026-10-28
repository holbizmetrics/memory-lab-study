#include "Editor.hpp"
#include <string>

namespace {
const auto ink=juce::Colour(0xff101819),panel=juce::Colour(0xff1a2729),mint=juce::Colour(0xffb9e7cb),amber=juce::Colour(0xffe8b879),white=juce::Colour(0xffe5eee8),muted=juce::Colour(0xff91a9a1);
struct Control{const char*id;const char*name;const char*unit;const char*help;};
const std::array<Control,8>voice{{
 {"detune","DETUNE","cents","Spacing between the two oscillators."},
 {"width","PULSE WIDTH","","Shape of oscillator 2. Range 0.10 to 0.90."},
 {"cutoff","CUTOFF","Hz","Sets the center frequency of the classic filter."},
 {"resonance","RESONANCE","Q","Emphasis and decay around the filter frequency."},
 {"profile","PROFILE","","Turn from high-pass through notch to low-pass."},
 {"drive","DRIVE","","Output saturation after the voice filter."},
 {"depth","MEMORY","","How strongly recent signal energy affects the drive."},
 {"recovery","RECOVERY","ms","How long the output drive takes to recover."}
}};
const std::array<Control,8>motion{{
 {"lfoRate","LFO RATE","Hz","One note-retriggered sine LFO per voice."},
 {"vibrato","VIBRATO","cents","Sine LFO pitch movement on both oscillators."},
 {"pwm","PULSE MOTION","","Sine LFO movement of pulse width."},
 {"filterLfo","FILTER MOTION","oct","Sine LFO movement of filter cutoff."},
 {"filterEnv","FILTER ENV","oct","Velocity-scaled filter envelope. Fixed A 20 ms / D 350 ms / S 0.15 / R 400 ms."},
 {"pressure","PRESSURE","oct","Channel pressure opens or closes cutoff. Zero disables this route."},
 {"depth","MEMORY","","Drive memory is shared by the summed voices."},
 {"recovery","RECOVERY","ms","Recovery time of the shared drive memory."}
}};
class PanelLook final:public juce::LookAndFeel_V4 {
public:
 PanelLook(){setColour(juce::Slider::textBoxTextColourId,white);setColour(juce::Slider::textBoxBackgroundColourId,ink);setColour(juce::Slider::textBoxOutlineColourId,juce::Colours::transparentBlack);setColour(juce::TextButton::buttonColourId,panel);setColour(juce::TextButton::buttonOnColourId,mint);setColour(juce::TextButton::textColourOffId,white);setColour(juce::TextButton::textColourOnId,ink);setColour(juce::ToggleButton::textColourId,white);setColour(juce::ToggleButton::tickColourId,mint);}
 void drawRotarySlider(juce::Graphics&g,int x,int y,int width,int height,float position,float start,float end,juce::Slider&s)override{
  const auto r=juce::Rectangle<float>(float(x),float(y),float(width),float(height)).reduced(7);const float radius=juce::jmin(r.getWidth(),r.getHeight())*.5f;
  const auto center=r.getCentre();const float angle=start+position*(end-start);const bool selected=bool(s.getProperties()["selected"]);
  juce::Path ring;ring.addCentredArc(center.x,center.y,radius,radius,0,start,end,true);g.setColour(juce::Colour(0xff3a4b4a));g.strokePath(ring,juce::PathStrokeType(3));
  juce::Path arc;arc.addCentredArc(center.x,center.y,radius,radius,0,start,angle,true);g.setColour(selected?amber:mint);g.strokePath(arc,juce::PathStrokeType(3));
  const auto body=juce::Rectangle<float>(radius*1.55f,radius*1.55f).withCentre(center);
  g.setGradientFill(juce::ColourGradient(juce::Colour(0xff435352),center.x,body.getY(),juce::Colour(0xff192120),center.x,body.getBottom(),false));g.fillEllipse(body);
  g.setColour(juce::Colour(0xff53645e));g.drawEllipse(body,1);
  const auto tip=center+juce::Point<float>(std::sin(angle),-std::cos(angle))*radius*.59f;
  const auto base=center+juce::Point<float>(std::sin(angle),-std::cos(angle))*radius*.22f;
  g.setColour(selected?amber:white);g.drawLine({base,tip},2.5f);
 }
};
}

class MemoryLabEditor::Surface final:public juce::Component,private juce::Timer {
 MemoryLabProcessor&processor;PanelLook look;
 struct Knob{juce::Label label;juce::Slider slider;std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>attachment;};
 std::array<Knob,8>knobs;
 juce::Slider output;juce::ToggleButton phase{"RESET OSC PHASE"};
 juce::TextButton voicePage{"VOICE"},motionPage{"MOTION"};
 juce::MidiKeyboardComponent keys;
 std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>outputAttachment;
 std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment>phaseAttachment;
 bool isMotion=false,binding=false;int selected=2;float meter=0;
 const auto&controls()const{return isMotion?motion:voice;}
 float value(const char*id)const{return processor.state.getRawParameterValue(id)->load();}
 void select(int index){selected=index;for(int i=0;i<8;++i){knobs[std::size_t(i)].slider.getProperties().set("selected",i==selected);knobs[std::size_t(i)].slider.repaint();}repaint();}
 void page(bool next){isMotion=next;binding=true;voicePage.setToggleState(!next,juce::dontSendNotification);motionPage.setToggleState(next,juce::dontSendNotification);
  for(int i=0;i<8;++i){auto&k=knobs[std::size_t(i)];const auto&c=controls()[std::size_t(i)];k.attachment.reset();k.label.setText(c.name,juce::dontSendNotification);k.slider.setName(c.name);k.slider.setTooltip(c.help);
   k.slider.setComponentID(c.id);k.attachment=std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processor.state,c.id,k.slider);
   k.slider.setDoubleClickReturnValue(true,processor.state.getParameter(c.id)->convertFrom0to1(processor.state.getParameter(c.id)->getDefaultValue()));
  }binding=false;select(isMotion?0:2);
 }
 void timerCallback()override{meter=juce::jmax(meter*.8f,processor.outputPeak.load());repaint();}
 void filterGraph(juce::Graphics&g,juce::Rectangle<float> area){
  const double q=value("resonance"),p=value("profile");juce::Path curve;
  g.setColour(juce::Colour(0xff263b38));for(int i=0;i<=4;++i){float x=area.getX()+area.getWidth()*i/4;g.drawVerticalLine(int(x),area.getY(),area.getBottom());}for(int i=0;i<=3;++i){float y=area.getY()+area.getHeight()*i/3;g.drawHorizontalLine(int(y),area.getX(),area.getRight());}
  const double sr=processor.getSampleRate()>=44100?processor.getSampleRate():48000;
  const double cutoff=juce::jlimit(30.,std::min(18000.,sr*.4),double(value("cutoff")));
  const double tangent=std::tan(memorylab::pi*cutoff/sr);
  for(int i=0;i<=400;++i){const double frequency=30*std::pow(18000./30,i/400.);const double w=std::tan(memorylab::pi*std::min(frequency,sr*.49)/sr)/tangent;
   const double hi=p<=.5?1:2-2*p,lo=p<=.5?2*p:1;
   const double magnitude=std::abs(lo-hi*w*w)/std::sqrt((1-w*w)*(1-w*w)+w*w/(q*q));
   const double db=juce::jlimit(-42.,18.,20*std::log10(std::max(1e-8,magnitude)));
   const float x=area.getX()+area.getWidth()*i/400.f,y=area.getBottom()-float((db+42)/60)*area.getHeight();
   if(i==0)curve.startNewSubPath(x,y);else curve.lineTo(x,y);
  }g.setColour(mint);g.strokePath(curve,juce::PathStrokeType(2));
 }
 void motionGraph(juce::Graphics&g,juce::Rectangle<float>area){juce::Path curve;const bool envelope=std::string(controls()[std::size_t(selected)].id)=="filterEnv";
  for(int i=0;i<=400;++i){double t=i/400.;double y;
   if(envelope){t*=1.1;y=t<.02?t/.02:t<.37?1-.85*(t-.02)/.35:t<.7?.15:std::max(0.,.15*(1-(t-.7)/.4));}
   else y=.5+.42*std::sin(2*memorylab::pi*value("lfoRate")*t);
   const float x=area.getX()+area.getWidth()*i/400.f,py=area.getBottom()-float(y)*area.getHeight();if(i==0)curve.startNewSubPath(x,py);else curve.lineTo(x,py);
  }g.setColour(amber);g.strokePath(curve,juce::PathStrokeType(2));
 }
public:
 explicit Surface(MemoryLabProcessor&p):processor(p),keys(p.keyboard,juce::MidiKeyboardComponent::horizontalKeyboard){
  setLookAndFeel(&look);setSize(1040,740);
  for(int i=0;i<8;++i){auto&k=knobs[std::size_t(i)];addAndMakeVisible(k.label);addAndMakeVisible(k.slider);k.label.setJustificationType(juce::Justification::centred);k.label.setColour(juce::Label::textColourId,muted);k.label.setFont(juce::Font(juce::FontOptions(12.f)));k.slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);k.slider.setTextBoxStyle(juce::Slider::TextBoxBelow,false,92,23);k.slider.onDragStart=[this,i]{select(i);};k.slider.onValueChange=[this,i]{if(!binding)select(i);};}
  addAndMakeVisible(output);output.setSliderStyle(juce::Slider::LinearVertical);output.setTextBoxStyle(juce::Slider::TextBoxBelow,false,85,24);output.setName("OUTPUT GAIN");output.setColour(juce::Slider::thumbColourId,mint);output.setColour(juce::Slider::trackColourId,mint);output.setDoubleClickReturnValue(true,.25);outputAttachment=std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(p.state,"volume",output);
  addAndMakeVisible(phase);phaseAttachment=std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(p.state,"phaseReset",phase);
  addAndMakeVisible(voicePage);addAndMakeVisible(motionPage);voicePage.setComponentID("voicePage");motionPage.setComponentID("motionPage");voicePage.onClick=[this]{page(false);};motionPage.onClick=[this]{page(true);};
  addAndMakeVisible(keys);keys.setAvailableRange(36,96);keys.setKeyWidth(980.f/36.f);keys.setLowestVisibleKey(36);keys.setOctaveForMiddleC(4);keys.setColour(juce::MidiKeyboardComponent::whiteNoteColourId,juce::Colour(0xffd8e0d7));keys.setColour(juce::MidiKeyboardComponent::blackNoteColourId,juce::Colour(0xff14201c));keys.setColour(juce::MidiKeyboardComponent::keyDownOverlayColourId,mint.withAlpha(.7f));
  page(false);startTimerHz(20);
 }
 ~Surface()override{stopTimer();for(auto&k:knobs)k.attachment.reset();outputAttachment.reset();phaseAttachment.reset();setLookAndFeel(nullptr);}
 void resized()override{
  voicePage.setBounds(660,25,110,32);motionPage.setBounds(780,25,110,32);
  for(int i=0;i<8;++i){auto&k=knobs[std::size_t(i)];k.label.setBounds(28+i*108,344,106,22);k.slider.setBounds(32+i*108,372,98,126);}
  output.setBounds(926,144,78,325);phase.setBounds(40,526,210,30);keys.setBounds(30,594,980,94);
 }
 void paint(juce::Graphics&g)override{
  g.fillAll(ink);g.setColour(white);g.setFont(juce::Font(juce::FontOptions(27.f,juce::Font::bold)));g.drawText("MEMORY LAB",30,20,320,36,juce::Justification::centredLeft);
  g.setColour(muted);g.setFont(juce::Font(juce::FontOptions(12.f)));g.drawText("STUDY / 0.2",335,29,180,20,juce::Justification::centredLeft);
  g.setColour(panel);g.fillRoundedRectangle(28,82,866,236,12);g.fillRoundedRectangle(912,82,100,426,12);
  g.setColour(juce::Colour(0xff090f10));g.fillRoundedRectangle(40,94,842,212,7);
  const auto&c=controls()[std::size_t(selected)];g.setColour(isMotion?amber:mint);g.setFont(juce::Font(juce::FontOptions(14.f)));g.drawText(c.name,60,110,285,26,juce::Justification::centredLeft);
  auto*parameter=processor.state.getParameter(c.id);juce::String text=parameter->getText(parameter->getValue(),24);
  if(std::string(c.id)=="profile"){const float f=value(c.id);text=f<.01f?"HIGH-PASS":f>.99f?"LOW-PASS":std::abs(f-.5f)<.01f?"NOTCH":juce::String(f,2);}
  g.setColour(white);g.setFont(juce::Font(juce::FontOptions(38.f,juce::Font::bold)));g.drawText(text,58,145,290,54,juce::Justification::centredLeft);
  g.setColour(muted);g.setFont(juce::Font(juce::FontOptions(13.f)));g.drawText(c.unit,61,197,260,20,juce::Justification::centredLeft);g.drawFittedText(c.help,60,229,285,61,juce::Justification::topLeft,3);
  const bool env=std::string(c.id)=="filterEnv";g.setColour(muted);g.drawText(isMotion?(env?"FILTER ENVELOPE / FIXED TIMING":"PER-VOICE SINE LFO / 1 SECOND"):"STATIC FILTER RESPONSE / 30 Hz - 18 kHz",376,110,480,22,juce::Justification::centredLeft);
  if(isMotion)motionGraph(g,{376,147,476,134});else filterGraph(g,{376,147,476,134});
  g.setColour(muted);g.setFont(juce::Font(juce::FontOptions(12.f)));g.drawText("OUTPUT",916,108,94,24,juce::Justification::centred);g.drawText("LEVEL",916,471,94,20,juce::Justification::centred);
  g.setColour(juce::Colour(0xff30443b));g.fillRoundedRectangle(924,495,76,4,2);g.setColour(meter>.95f?amber:mint);g.fillRoundedRectangle(924,495,76*juce::jlimit(0.f,1.f,meter),4,2);
  g.setColour(muted);g.drawText("OSCILLATORS   /   CLASSIC FILTER   /   MEMORY DRIVE",320,531,570,22,juce::Justification::centredLeft);
  g.drawText("Drag to edit    |    Double-click to restore a control    |    Click a value to type",40,567,970,20,juce::Justification::centredLeft);
  g.drawText(juce::String(processor.voiceCount.load())+" / 8 VOICES",34,704,180,20,juce::Justification::centredLeft);g.drawText("ORIGINAL SUBTRACTIVE INSTRUMENT STUDY",420,704,590,20,juce::Justification::centredRight);
 }
};

MemoryLabEditor::MemoryLabEditor(MemoryLabProcessor&p):AudioProcessorEditor(p),surface(std::make_unique<Surface>(p)){
 addAndMakeVisible(*surface);setResizable(true,true);setResizeLimits(832,592,1560,1110);if(auto*c=getConstrainer())c->setFixedAspectRatio(1040./740);setSize(1040,740);
}
MemoryLabEditor::~MemoryLabEditor()=default;
void MemoryLabEditor::resized(){const float scale=getWidth()/1040.f;surface->setTransform(juce::AffineTransform::scale(scale));surface->setBounds(0,0,1040,740);}
