#include "Editor.hpp"
#include "Patch.hpp"
#include <string>
#include <complex>

namespace {
const auto ink=juce::Colour(0xff101819),panel=juce::Colour(0xff1a2729),mint=juce::Colour(0xffb9e7cb),amber=juce::Colour(0xffe8b879),white=juce::Colour(0xffe5eee8),muted=juce::Colour(0xff91a9a1);
struct Control{const char*id;const char*name;const char*unit;const char*help;};
const std::array<Control,8>voice{{
 {"detune","DETUNE","cents","Spacing between the two oscillators."},
 {"width","PULSE WIDTH","","Shape of oscillator 2. Range 0.10 to 0.90."},
 {"cutoff","CUTOFF","Hz","Sets the center frequency of the classic filter."},
 {"resonance","RESONANCE","Q","Emphasis and decay around the filter frequency."},
 {"profile","CLASSIC PROFILE","","Classic family only: high-pass through notch to low-pass."},
 {"drive","DRIVE","","Output saturation after the voice filter."},
 {"depth","MEMORY","","How strongly recent signal energy affects the drive."},
 {"recovery","RECOVERY","ms","How long the output drive takes to recover."}
}};
const std::array<Control,8>motion{{
 {"lfoRate","LFO RATE","Hz","One note-retriggered sine LFO per voice."},
 {"vibrato","VIBRATO","cents","Sine LFO pitch movement on both oscillators."},
 {"pwm","PULSE MOTION","","Sine LFO movement of pulse width."},
 {"filterLfo","FILTER MOTION","oct","Sine LFO movement of filter cutoff."},
 {"filterEnv","FILTER ENV","oct","Velocity-scaled envelope amount. Edit its stages on ENVELOPES / FILTER."},
 {"pressure","PRESSURE","oct","Channel pressure opens or closes cutoff. Zero disables this route."},
 {"depth","MEMORY","","Drive memory is shared by the summed voices."},
 {"recovery","RECOVERY","ms","Recovery time of the shared drive memory."}
}};
const std::array<Control,8>ampControls{{
 {"ampDelay","DELAY","ms","Wait after key-down before amplitude starts. Applies on the next note."},
 {"ampAttack","ATTACK","ms","Rise from silence to peak level. Applies on the next note."},
 {"ampHold","HOLD","ms","Time at peak level before decay. Applies on the next note."},
 {"ampDecay","DECAY","ms","Fall from peak to the sustain level. Applies on the next note."},
 {"ampSustain","SUSTAIN","","Level held while the key or sustain pedal remains down."},
 {"ampRelease","RELEASE","ms","Fade from the current level after key/pedal release. Applies on the next note."},
 {"drive","DRIVE","","Shared output drive. Its response also depends on the amplitude contour."},
 {"depth","MEMORY","","Depth of the shared drive memory."}
}};
const std::array<Control,8>filterControls{{
 {"fltDelay","DELAY","ms","Wait after key-down before the filter envelope starts."},
 {"fltAttack","ATTACK","ms","Rise to peak filter modulation. Applies on the next note."},
 {"fltHold","HOLD","ms","Time at peak filter modulation before decay."},
 {"fltDecay","DECAY","ms","Fall to the sustained filter modulation level."},
 {"fltSustain","SUSTAIN","","Envelope level held while the key or sustain pedal remains down."},
 {"fltRelease","RELEASE","ms","Fade filter modulation after key/pedal release, while amplitude remains audible."},
 {"filterEnv","ENV AMOUNT","oct","Envelope-to-cutoff amount, scaled by note velocity. Zero disables the route."},
 {"cutoff","BASE CUTOFF","Hz","The base frequency around which the envelope moves the filter."}
}};
const std::array<Control,8>arpControls{{
 {"arpOn","ARP ON","","Off plays your MIDI unchanged. On turns held notes into a repeating pattern. Re-press notes after switching."},
 {"arpSync","HOST TEMPO","","Use host BPM when available. Otherwise use INTERNAL BPM. First held note starts the pattern."},
 {"arpBpm","INTERNAL BPM","BPM","Tempo when HOST TEMPO is off or the host supplies no tempo."},
 {"arpRate","RATE","","Quarter, eighth, sixteenth, 32nd or triplet note spacing."},
 {"arpOrder","ORDER","","Up, Down or Up/Down through held pitches and octave layers."},
 {"arpOctaves","OCTAVES","","Repeat the held notes over one, two or three octave layers."},
 {"arpGate","GATE","%","Length of each generated MIDI note. Envelope release adds its own tail."},
 {"cutoff","BASE CUTOFF","Hz","Shape the sound while the arpeggio runs."}
}};
const std::array<Control,8>driveControls{{
 {"voiceDrive","VOICE DRIVE","dB","Per-voice clipping after the filter. Zero bypasses. Lower OUTPUT to compensate."},
 {"drive","SHARED DRIVE","","Memory drive after the voices mix."},
 {"depth","MEMORY","","Depth of the shared drive memory."},
 {"recovery","RECOVERY","ms","Recovery of the shared drive memory."},
 {"cutoff","CUTOFF","Hz","Filter frequency before voice drive."},
 {"resonance","RESONANCE","Q","Filter emphasis before clipping."},
 {"profile","CLASSIC PROFILE","","Classic family only: high-pass through notch to low-pass."},
 {"volume","OUTPUT","","Final level after both drive stages."}
}};
const std::array<Control,8>oscControls{{
 {"sawLevel","OSC 1 / SAW","","Level of oscillator 1 before the filter. Zero mutes this source."},
 {"pulseLevel","OSC 2 / PULSE","","Level of oscillator 2 before the filter. Zero mutes this source."},
 {"osc2Interval","OSC 2 INTERVAL","semitones","Oscillator 2 interval. Changes glide over about 20 ms on held notes."},
 {"detune","OSC 2 DETUNE","cents","Fine detune added to oscillator 2 interval. Original: +7 cents."},
 {"width","PULSE WIDTH","","Width of oscillator 2 pulse wave."},
 {"pwm","PULSE MOTION","","Depth of the per-voice sine LFO on pulse width."},
 {"cutoff","CUTOFF","Hz","Filter after the oscillator mix."},
 {"noiseLevel","WHITE NOISE","","Noise before the envelope and filter. Zero is off. Mute saw/pulse for noise alone."}
}};
const std::array<Control,8>familyControls{{
 {"filterFamily","FILTER FAMILY","","Classic keeps HP/notch/LP PROFILE. LP4 is steeper; BP4 is band-pass; BLEND mixes BP4 and LP2."},
 {"cutoff","CUTOFF","Hz","Frequency of the selected filter family."},
 {"resonance","RESONANCE","Q","Q for each section. Resonance and bandwidth interact."},
 {"profile","CLASSIC PROFILE","","HP/notch/LP only in CLASSIC. This control is inactive in the other families."},
 {"filterEnv","ENV AMOUNT","oct","Velocity-scaled filter envelope amount. Edit its stages under ENVELOPES / FILTER."},
 {"filterLfo","FILTER MOTION","oct","Per-voice sine LFO amount on filter cutoff."},
 {"voiceDrive","VOICE DRIVE","dB","Per-voice clipping after the selected filter. Zero bypasses."},
 {"noiseLevel","WHITE NOISE","","Noise joins the oscillators before the shared voice filter."}
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
 juce::TextButton savePatch{"SAVE"},loadPatch{"LOAD"};
 std::unique_ptr<juce::FileChooser> patchChooser;
 juce::File patchFile{juce::File::getSpecialLocation(juce::File::userDocumentsDirectory).getChildFile("Memory Lab Study.mlpatch")};
 void choosePatch(bool saving){
  if(patchChooser)return;
  patchChooser=std::make_unique<juce::FileChooser>(saving?"Save Memory Lab patch":"Load Memory Lab patch",patchFile,"*.mlpatch",true,false,this);
  const auto chooserFlags=saving?(juce::FileBrowserComponent::saveMode|juce::FileBrowserComponent::canSelectFiles|juce::FileBrowserComponent::warnAboutOverwriting):(juce::FileBrowserComponent::openMode|juce::FileBrowserComponent::canSelectFiles);
  patchChooser->launchAsync(chooserFlags,[safe=juce::Component::SafePointer<Surface>(this),saving](const juce::FileChooser& chooser){
   if(!safe)return;auto file=chooser.getResult();
   if(file!=juce::File{}){
    const auto result=saving?memorylab::patch::save(safe->processor,file):memorylab::patch::load(safe->processor,file);
    if(result.wasOk())safe->patchFile=file;
    else juce::AlertWindow::showMessageBoxAsync(juce::MessageBoxIconType::WarningIcon,"Memory Lab patch",result.getErrorMessage());
   }
   safe->patchChooser.reset();
  });
 }
 juce::TextButton voicePage{"VOICE"},oscPage{"OSC"},familyPage{"FILTER"},motionPage{"MOTION"},envelopePage{"ENVELOPES"},arpPage{"ARP"},drivePage{"DRIVE"},ampPage{"AMPLITUDE"},filterPage{"FILTER"};
 juce::MidiKeyboardComponent keys;
 std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>outputAttachment;
 std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment>phaseAttachment;
 bool isMotion=false,isEnvelope=false,binding=false;int pageIndex=0,selected=2;float meter=0;
 const auto&controls()const{return pageIndex==0?voice:pageIndex==1?motion:pageIndex==2?ampControls:pageIndex==3?filterControls:pageIndex==4?arpControls:pageIndex==5?driveControls:pageIndex==6?oscControls:familyControls;}
 float value(const char*id)const{return processor.state.getRawParameterValue(id)->load();}
 void select(int index){selected=index;for(int i=0;i<8;++i){knobs[std::size_t(i)].slider.getProperties().set("selected",i==selected);knobs[std::size_t(i)].slider.repaint();}repaint();}
 void page(int next){pageIndex=next;isMotion=next==1;isEnvelope=next==2||next==3;binding=true;voicePage.setToggleState(next==0,juce::dontSendNotification);motionPage.setToggleState(next==1,juce::dontSendNotification);envelopePage.setToggleState(isEnvelope,juce::dontSendNotification);arpPage.setToggleState(next==4,juce::dontSendNotification);drivePage.setToggleState(next==5,juce::dontSendNotification);oscPage.setToggleState(next==6,juce::dontSendNotification);familyPage.setToggleState(next==7,juce::dontSendNotification);ampPage.setToggleState(next==2,juce::dontSendNotification);filterPage.setToggleState(next==3,juce::dontSendNotification);ampPage.setVisible(isEnvelope);filterPage.setVisible(isEnvelope);
  for(int i=0;i<8;++i){auto&k=knobs[std::size_t(i)];const auto&c=controls()[std::size_t(i)];k.attachment.reset();k.label.setText(c.name,juce::dontSendNotification);k.slider.setName(c.name);k.slider.setTooltip(c.help);
   k.slider.setComponentID(c.id);k.attachment=std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processor.state,c.id,k.slider);
   k.slider.setDoubleClickReturnValue(true,processor.state.getParameter(c.id)->convertFrom0to1(processor.state.getParameter(c.id)->getDefaultValue()));
  }binding=false;select(pageIndex>=5?0:isEnvelope?1:isMotion?0:2);
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
   double magnitude=std::abs(lo-hi*w*w)/std::sqrt((1-w*w)*(1-w*w)+w*w/(q*q));
   const int family=int(std::round(value("filterFamily")));if(family){const std::complex<double>den(1-w*w,w/q),lp=1./den,bp=std::complex<double>(0,w/q)/den;magnitude=std::abs(family==1?lp*lp:family==2?bp*bp:.5*(bp*bp+lp));}
   const double db=juce::jlimit(-42.,18.,20*std::log10(std::max(1e-8,magnitude)));
   const float x=area.getX()+area.getWidth()*i/400.f,y=area.getBottom()-float((db+42)/60)*area.getHeight();
   if(i==0)curve.startNewSubPath(x,y);else curve.lineTo(x,y);
  }g.setColour(mint);g.strokePath(curve,juce::PathStrokeType(2));
 }
 void envelopeGraph(juce::Graphics&g,juce::Rectangle<float>area){
  const bool amp=pageIndex==2;const std::string prefix=amp?"amp":"flt";
  auto get=[&](const char*s){return double(value((prefix+s).c_str()));};
  memorylab::EnvelopeSettings shape{get("Delay")/1000.,get("Attack")/1000.,get("Hold")/1000.,get("Decay")/1000.,get("Sustain"),get("Release")/1000.};
  const double releaseTime=shape.delay+shape.attack+shape.hold+shape.decay+.5,total=std::max(.1,releaseTime+shape.release);juce::Path curve;
  for(int i=0;i<=400;++i){const double t=total*i/400.;const double level=t<releaseTime?memorylab::envelopeLevel(t,shape):shape.release>0?std::max(0.,shape.sustain*(1-(t-releaseTime)/shape.release)):0.;
   const float x=area.getX()+area.getWidth()*i/400.f,y=area.getBottom()-16-float(level)*(area.getHeight()-20);if(i==0)curve.startNewSubPath(x,y);else curve.lineTo(x,y);
  }g.setColour(amber);g.strokePath(curve,juce::PathStrokeType(2));g.setColour(muted);g.setFont(juce::Font(juce::FontOptions(11.f)));g.drawText(!amp&&value("filterEnv")==0?"ENV AMOUNT is zero / route off":"Sustain preview: 0.5 s",int(area.getX()),int(area.getBottom()-12),280,15,juce::Justification::centredLeft);g.drawText(juce::String(total,2)+" s",int(area.getRight()-100),int(area.getBottom()-12),100,15,juce::Justification::centredRight);
 }
 void motionGraph(juce::Graphics&g,juce::Rectangle<float>area){juce::Path curve;
  for(int i=0;i<=400;++i){double t=i/400.;double y;
   y=.5+.42*std::sin(2*memorylab::pi*value("lfoRate")*t);
   const float x=area.getX()+area.getWidth()*i/400.f,py=area.getBottom()-float(y)*area.getHeight();if(i==0)curve.startNewSubPath(x,py);else curve.lineTo(x,py);
  }g.setColour(amber);g.strokePath(curve,juce::PathStrokeType(2));
 }
 void oscillatorGraph(juce::Graphics&g,juce::Rectangle<float>area){
  int row=0;for(const char*id:{"sawLevel","pulseLevel","noiseLevel"}){const float y=area.getY()+row*34;
   g.setColour(muted);g.drawText(row==0?"OSC 1 / SAW":row==1?"OSC 2 / PULSE":"WHITE NOISE",int(area.getX()),int(y),120,24,juce::Justification::centredLeft);
   g.setColour(panel);g.fillRoundedRectangle(area.getX()+130,y+3,area.getWidth()-130,18,3);
   g.setColour(row?amber:mint);g.fillRoundedRectangle(area.getX()+130,y+3,(area.getWidth()-130)*value(id),18,3);++row;
  }
  g.setColour(muted);g.drawText("OSC 2: "+juce::String(value("osc2Interval"),0)+" semitones / "+juce::String(value("detune"),1)+" cents",int(area.getX()),int(area.getBottom()-24),int(area.getWidth()),24,juce::Justification::centredLeft);
 }
 void driveGraph(juce::Graphics&g,juce::Rectangle<float>area){
  juce::Path curve;const double db=value("voiceDrive"),mix=std::min(1.,db),gain=std::pow(10.,db/20.);
  g.setColour(muted.withAlpha(.3f));g.drawLine(area.getX(),area.getCentreY(),area.getRight(),area.getCentreY());g.drawLine(area.getCentreX(),area.getY(),area.getCentreX(),area.getBottom());
  for(int i=0;i<=400;++i){const double x=i/200.-1,y=x+(std::clamp(x*gain,-1.,1.)-x)*mix;const float px=area.getX()+area.getWidth()*i/400.f,py=area.getCentreY()-float(y)*area.getHeight()*.45f;if(i==0)curve.startNewSubPath(px,py);else curve.lineTo(px,py);}
  g.setColour(amber);g.strokePath(curve,juce::PathStrokeType(2));
 }
 void arpGraph(juce::Graphics&g,juce::Rectangle<float>area){
  const float width=area.getWidth()/16,gate=value("arpGate")*.01f;
  const bool enabled=value("arpOn")>=.5f;
  for(int i=0;i<16;++i){const float x=area.getX()+i*width;
   g.setColour(juce::Colour(0xff263b38));g.drawVerticalLine(int(x),area.getY()+10,area.getBottom()-25);
   g.setColour(!enabled?muted.withAlpha(.35f):processor.arpStep.load()%16==i?amber:mint);
   g.fillRoundedRectangle(x+1,area.getY()+28,width*gate,48,2);
  }
  g.setColour(muted);g.setFont(juce::Font(juce::FontOptions(11.f)));
  const auto tempo=processor.arpTempo.load();
  g.drawText(juce::String(tempo,1)+" BPM / "+(processor.arpHasHostTempo.load()?"HOST":"INTERNAL")+" / "+(enabled?"HOLD NOTES TO PLAY":"ARP OFF"),int(area.getX()),int(area.getBottom()-16),int(area.getWidth()),18,juce::Justification::centredLeft);
 }
public:
 explicit Surface(MemoryLabProcessor&p):processor(p),keys(p.keyboard,juce::MidiKeyboardComponent::horizontalKeyboard){
  setLookAndFeel(&look);setSize(1040,740);
  addAndMakeVisible(savePatch);addAndMakeVisible(loadPatch);savePatch.setComponentID("savePatch");loadPatch.setComponentID("loadPatch");
  savePatch.setTooltip("Save all sound and arpeggiator controls as a patch.");loadPatch.setTooltip("Load a patch. Stop playback first; DAW automation can override its controls.");
  savePatch.onClick=[this]{choosePatch(true);};loadPatch.onClick=[this]{choosePatch(false);};
  for(int i=0;i<8;++i){auto&k=knobs[std::size_t(i)];addAndMakeVisible(k.label);addAndMakeVisible(k.slider);k.label.setJustificationType(juce::Justification::centred);k.label.setColour(juce::Label::textColourId,muted);k.label.setFont(juce::Font(juce::FontOptions(12.f)));k.slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);k.slider.setTextBoxStyle(juce::Slider::TextBoxBelow,false,92,23);k.slider.onDragStart=[this,i]{select(i);};k.slider.onValueChange=[this,i]{if(!binding)select(i);};}
  addAndMakeVisible(output);output.setSliderStyle(juce::Slider::LinearVertical);output.setTextBoxStyle(juce::Slider::TextBoxBelow,false,85,24);output.setName("OUTPUT GAIN");output.setColour(juce::Slider::thumbColourId,mint);output.setColour(juce::Slider::trackColourId,mint);output.setDoubleClickReturnValue(true,.25);outputAttachment=std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(p.state,"volume",output);
  addAndMakeVisible(phase);phaseAttachment=std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(p.state,"phaseReset",phase);
  addAndMakeVisible(voicePage);addAndMakeVisible(motionPage);voicePage.setComponentID("voicePage");motionPage.setComponentID("motionPage");voicePage.onClick=[this]{page(false);};motionPage.onClick=[this]{page(true);};
  addAndMakeVisible(envelopePage);addAndMakeVisible(ampPage);addAndMakeVisible(filterPage);envelopePage.setComponentID("envelopePage");ampPage.setComponentID("ampEnvelopePage");filterPage.setComponentID("filterEnvelopePage");envelopePage.onClick=[this]{page(2);};ampPage.onClick=[this]{page(2);};filterPage.onClick=[this]{page(3);};
  addAndMakeVisible(familyPage);familyPage.setComponentID("familyPage");familyPage.onClick=[this]{page(7);};addAndMakeVisible(oscPage);oscPage.setComponentID("oscPage");oscPage.onClick=[this]{page(6);};addAndMakeVisible(drivePage);drivePage.setComponentID("drivePage");drivePage.onClick=[this]{page(5);};addAndMakeVisible(arpPage);arpPage.setComponentID("arpPage");arpPage.onClick=[this]{page(4);};
  addAndMakeVisible(keys);keys.setAvailableRange(36,96);keys.setKeyWidth(980.f/36.f);keys.setLowestVisibleKey(36);keys.setOctaveForMiddleC(4);keys.setColour(juce::MidiKeyboardComponent::whiteNoteColourId,juce::Colour(0xffd8e0d7));keys.setColour(juce::MidiKeyboardComponent::blackNoteColourId,juce::Colour(0xff14201c));keys.setColour(juce::MidiKeyboardComponent::keyDownOverlayColourId,mint.withAlpha(.7f));
  page(false);startTimerHz(20);
 }
 ~Surface()override{stopTimer();for(auto&k:knobs)k.attachment.reset();outputAttachment.reset();phaseAttachment.reset();setLookAndFeel(nullptr);}
 void resized()override{
  savePatch.setBounds(912,25,47,32);loadPatch.setBounds(965,25,47,32);
  voicePage.setBounds(340,25,68,32);oscPage.setBounds(414,25,52,32);motionPage.setBounds(472,25,70,32);envelopePage.setBounds(548,25,94,32);familyPage.setBounds(648,25,70,32);arpPage.setBounds(724,25,52,32);drivePage.setBounds(782,25,76,32);ampPage.setBounds(300,526,130,30);filterPage.setBounds(440,526,130,30);
  for(int i=0;i<8;++i){auto&k=knobs[std::size_t(i)];k.label.setBounds(28+i*108,344,106,22);k.slider.setBounds(32+i*108,372,98,126);}
  output.setBounds(926,144,78,325);phase.setBounds(40,526,210,30);keys.setBounds(30,594,980,94);
 }
 void paint(juce::Graphics&g)override{
  g.fillAll(ink);g.setColour(white);g.setFont(juce::Font(juce::FontOptions(27.f,juce::Font::bold)));g.drawText("MEMORY LAB",30,20,320,36,juce::Justification::centredLeft);
  g.setColour(muted);g.setFont(juce::Font(juce::FontOptions(12.f)));g.drawText("STUDY / 0.8",240,29,90,20,juce::Justification::centredLeft);
  g.setColour(panel);g.fillRoundedRectangle(28,82,866,236,12);g.fillRoundedRectangle(912,82,100,426,12);
  g.setColour(juce::Colour(0xff090f10));g.fillRoundedRectangle(40,94,842,212,7);
  const auto&c=controls()[std::size_t(selected)];g.setColour(isMotion?amber:mint);g.setFont(juce::Font(juce::FontOptions(14.f)));g.drawText(c.name,60,110,285,26,juce::Justification::centredLeft);
  auto*parameter=processor.state.getParameter(c.id);juce::String text=parameter->getText(parameter->getValue(),24);
  if(std::string(c.id)=="voiceDrive"&&value(c.id)==0)text="OFF";
  if(std::string(c.id)=="profile"){const float f=value(c.id);text=f<.01f?"HIGH-PASS":f>.99f?"LOW-PASS":std::abs(f-.5f)<.01f?"NOTCH":juce::String(f,2);}
  if(std::string(c.id)=="profile"&&std::round(value("filterFamily"))!=0)text="INACTIVE";
  if(std::string(c.id)=="arpOn"||std::string(c.id)=="arpSync")text=value(c.id)>=.5f?"ON":"OFF";
  if(std::string(c.id)=="arpRate"){const char*names[]{"1/4","1/8","1/16","1/32","1/8 TRIPLET","1/16 TRIPLET"};text=names[juce::jlimit(0,5,int(value(c.id)))];}
  if(std::string(c.id)=="arpOrder"){const char*names[]{"UP","DOWN","UP / DOWN"};text=names[juce::jlimit(0,2,int(value(c.id)))];}
  g.setColour(white);g.setFont(juce::Font(juce::FontOptions(38.f,juce::Font::bold)));g.drawText(text,58,145,290,54,juce::Justification::centredLeft);
  g.setColour(muted);g.setFont(juce::Font(juce::FontOptions(13.f)));g.drawText(c.unit,61,197,260,20,juce::Justification::centredLeft);g.drawFittedText(c.help,60,229,285,61,juce::Justification::topLeft,3);
  const bool env=isEnvelope||(isMotion&&std::string(c.id)=="filterEnv");g.setColour(muted);g.drawText(pageIndex==6?"OSCILLATOR MIX / SOURCE LEVELS":pageIndex==5?"PER-VOICE DRIVE / STATIC TRANSFER":pageIndex==4?"ARPEGGIATOR / 16-STEP RHYTHM PREVIEW":env?(pageIndex==2?"AMPLITUDE ENVELOPE / NEXT NOTE":"FILTER ENVELOPE / NEXT NOTE"):isMotion?"PER-VOICE SINE LFO / 1 SECOND":"STATIC FILTER RESPONSE / 30 Hz - 18 kHz",376,110,480,22,juce::Justification::centredLeft);
  if(pageIndex==6)oscillatorGraph(g,{376,147,476,134});else if(pageIndex==5)driveGraph(g,{376,147,476,134});else if(pageIndex==4)arpGraph(g,{376,147,476,134});else if(env)envelopeGraph(g,{376,147,476,134});else if(isMotion)motionGraph(g,{376,147,476,134});else filterGraph(g,{376,147,476,134});
  g.setColour(muted);g.setFont(juce::Font(juce::FontOptions(12.f)));g.drawText("OUTPUT",916,108,94,24,juce::Justification::centred);g.drawText("LEVEL",916,471,94,20,juce::Justification::centred);
  g.setColour(juce::Colour(0xff30443b));g.fillRoundedRectangle(924,495,76,4,2);g.setColour(meter>.95f?amber:mint);g.fillRoundedRectangle(924,495,76*juce::jlimit(0.f,1.f,meter),4,2);
  g.setColour(muted);g.drawText(pageIndex==6?"SAW + PULSE + NOISE / FILTER / DRIVE":pageIndex==5?"VOICE DRIVE / VOICE MIX / SHARED MEMORY DRIVE":pageIndex==4?"FIRST NOTE STARTS THE CLOCK / PEDAL HOLDS NOTES":isEnvelope?"Times apply to the next note.":"SOURCES   /   FILTER   /   MEMORY DRIVE",isEnvelope?600:320,531,isEnvelope?290:570,22,juce::Justification::centredLeft);
  g.drawText("Drag to edit    |    Double-click to restore a control    |    Click a value to type",40,567,970,20,juce::Justification::centredLeft);
  g.drawText(juce::String(processor.voiceCount.load())+" / 8 VOICES",34,704,180,20,juce::Justification::centredLeft);g.drawText("ORIGINAL SUBTRACTIVE INSTRUMENT STUDY",420,704,590,20,juce::Justification::centredRight);
 }
};

MemoryLabEditor::MemoryLabEditor(MemoryLabProcessor&p):AudioProcessorEditor(p),surface(std::make_unique<Surface>(p)){
 addAndMakeVisible(*surface);setResizable(true,true);setResizeLimits(832,592,1560,1110);if(auto*c=getConstrainer())c->setFixedAspectRatio(1040./740);setSize(1040,740);
}
MemoryLabEditor::~MemoryLabEditor()=default;
void MemoryLabEditor::resized(){const float scale=getWidth()/1040.f;surface->setTransform(juce::AffineTransform::scale(scale));surface->setBounds(0,0,1040,740);}
