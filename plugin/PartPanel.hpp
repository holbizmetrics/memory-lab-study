#pragma once
#include "Processor.hpp"

class PartPanel final:public juce::Component {
 MemoryLabProcessor&processor;
 juce::ComboBox mode;
 juce::Slider split;
 std::array<juce::ComboBox,4>channels;
 std::array<juce::TextButton,4>edit,copy;
 std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment>modeAttachment;
 std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>splitAttachment;
 std::array<std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment>,4>channelAttachments;
 int allocation()const{return int(std::round(processor.state.getRawParameterValue("partMode")->load()));}
public:
 std::function<void()> onSelect;
 explicit PartPanel(MemoryLabProcessor&p):processor(p){
  setComponentID("partPanel");
  addAndMakeVisible(mode);mode.setComponentID("partMode");mode.addItem("Single - Part 1 / 8 voices",1);mode.addItem("2-layer - 4 voices per part",2);mode.addItem("2-way split - 4 per side",3);mode.addItem("4-layer - 2 voices per part",4);
  mode.setTooltip("One eight-voice budget. Changing mode clears held notes; layers consume a voice in each active part.");
  modeAttachment=std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(p.state,"partMode",mode);
  addAndMakeVisible(split);split.setComponentID("splitNote");split.setSliderStyle(juce::Slider::LinearHorizontal);split.setTextBoxStyle(juce::Slider::TextBoxRight,false,55,26);
  split.setTooltip("2-way split: Part 1 below this MIDI note number; Part 2 from this note upward. Routing edits clear held notes.");
  splitAttachment=std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(p.state,"splitNote",split);
  for(int part=0;part<4;++part){
   addAndMakeVisible(edit[part]);edit[part].setComponentID("editPart"+juce::String(part+1));edit[part].setButtonText("EDIT PART "+juce::String(part+1));
   edit[part].onClick=[this,part]{processor.editingPart=part;if(onSelect)onSelect();};
   addAndMakeVisible(copy[part]);copy[part].setComponentID("copyPart1To"+juce::String(part+1));copy[part].setButtonText("COPY P1 HERE");copy[part].setEnabled(part!=0);
   copy[part].setTooltip("Replace this part's sound controls with Part 1. Routing, channels and global arp stay unchanged.");
   copy[part].onClick=[this,part]{processor.copyPart(0,part);};
   auto&channel=channels[part];addAndMakeVisible(channel);channel.setComponentID("part"+juce::String(part+1)+"Channel");channel.addItem("OMNI",1);
   for(int c=1;c<=16;++c)channel.addItem("Channel "+juce::String(c),c+1);
   channel.setTooltip("Receive notes on this MIDI channel, or OMNI. Channel changes clear held notes.");
   channelAttachments[part]=std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(p.state,"part"+juce::String(part+1)+"Channel",channel);
  }
 }
 void refresh(){split.setEnabled(allocation()==2);split.setAlpha(allocation()==2?1.f:.35f);for(int p=0;p<4;++p)edit[p].setToggleState(processor.editingPart.load()==p,juce::dontSendNotification);repaint();}
 void resized()override{
  mode.setBounds(32,72,260,32);split.setBounds(335,72,330,32);
  for(int p=0;p<4;++p){const int y=150+p*44;edit[p].setBounds(32,y,165,32);copy[p].setBounds(222,y,135,32);channels[p].setBounds(390,y,165,32);}
 }
 void paint(juce::Graphics&g)override{
  g.fillAll(juce::Colour(0xff101819));g.setColour(juce::Colour(0xffe5eee8));g.setFont(juce::Font(juce::FontOptions(23.f,juce::Font::bold)));g.drawText("PARTS / EIGHT VOICES TOTAL",32,8,760,38,juce::Justification::centredLeft);
  g.setColour(juce::Colour(0xff91a9a1));g.setFont(juce::Font(juce::FontOptions(12.f)));g.drawText("ALLOCATION",32,48,260,20,juce::Justification::centredLeft);g.drawText("SPLIT / MIDI NOTE NUMBER",335,48,330,20,juce::Justification::centredLeft);g.drawText("EDIT SOUND",32,121,165,22,juce::Justification::centredLeft);g.drawText("COPY SOUND",222,121,135,22,juce::Justification::centredLeft);g.drawText("MIDI INPUT",390,121,165,22,juce::Justification::centredLeft);g.drawText("VOICE RESERVATION / ZONE",585,121,245,22,juce::Justification::centredLeft);
  const int m=allocation(),n=int(std::round(processor.state.getRawParameterValue("splitNote")->load()));
  for(int p=0;p<4;++p){
   const int count=m==0?(p==0?8:0):m==3?2:p<2?4:0;
   const auto zone=m==2&&p<2?(p==0?"MIDI 0-"+juce::String(n-1):"MIDI "+juce::String(n)+"-127"):juce::String("Full keyboard");
   g.setColour(count?juce::Colour(0xffb9e7cb):juce::Colour(0xff657b75));g.drawText(count?juce::String(count)+" voices / "+zone:"Inactive in this allocation",585,150+p*44,245,32,juce::Justification::centredLeft);
  }
  g.setColour(juce::Colour(0xff91a9a1));g.drawText("Routing edits silence held notes. Layer outputs add together; lower part OUTPUT levels as needed.",32,337,800,24,juce::Justification::centredLeft);
  g.drawText("Sound pages, OUTPUT, SOUNDS and DICE edit the selected part. ARP is global.",32,364,800,22,juce::Justification::centredLeft);
  g.drawText("SAVE / LOAD store the whole setup. INIT resets every part and allocation.",32,388,800,22,juce::Justification::centredLeft);
 }
};

