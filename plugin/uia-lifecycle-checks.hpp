#pragma once
#if JUCE_WINDOWS
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <UIAutomation.h>
#include <thread>
#include <chrono>
#include <cstdlib>

namespace uia_checks {
template<class T>struct Ref{
 T* value=nullptr;
 ~Ref(){if(value)value->Release();}
 T** out(){return &value;}
 T* operator->()const{return value;}
};
inline void requireHr(HRESULT hr,const char*message){if(FAILED(hr))throw std::runtime_error(std::string(message)+" HRESULT="+std::to_string(hr));}
inline void pump(){MSG message;while(PeekMessageW(&message,nullptr,0,0,PM_REMOVE)){TranslateMessage(&message);DispatchMessageW(&message);}}
inline void waitFor(const std::atomic<int>&stage,int wanted){
 const auto deadline=std::chrono::steady_clock::now()+std::chrono::seconds(20);
 while(stage.load()<wanted){pump();if(std::chrono::steady_clock::now()>deadline){std::cerr<<"FAIL: UIA lifecycle harness timeout\n";std::_Exit(2);}std::this_thread::sleep_for(std::chrono::milliseconds(1));}
}
}

void uiaLifecycleChecks(){
 using namespace uia_checks;
 for(int cycle=0;cycle<8;++cycle){
  auto processor=std::make_unique<MemoryLabProcessor>();
  auto editor=std::unique_ptr<juce::AudioProcessorEditor>(processor->createEditor());
  editor->setName("Memory Lab isolated UIA lifecycle test");
  editor->setBounds(20,20,832,592);
  editor->addToDesktop(juce::ComponentPeer::windowHasTitleBar|juce::ComponentPeer::windowIsTemporary);
  editor->setVisible(true);
  const auto hwnd=static_cast<HWND>(editor->getWindowHandle());check(hwnd!=nullptr,"UIA harness HWND missing");
  std::atomic<int> stage{0};std::string error;int sliders=0;HRESULT afterClose=S_OK,writeAfterClose=S_OK;double closedValue=0;
  std::thread client([&]{
   const auto init=CoInitializeEx(nullptr,COINIT_MULTITHREADED);
   if(FAILED(init)){error="UIA client COM initialization failed";stage=3;return;}
   try{
    Ref<IUIAutomation> automation;
    requireHr(CoCreateInstance(__uuidof(CUIAutomation),nullptr,CLSCTX_INPROC_SERVER,__uuidof(IUIAutomation),reinterpret_cast<void**>(automation.out())),"create UIA client");
    Ref<IUIAutomationElement> root;requireHr(automation->ElementFromHandle(hwnd,root.out()),"UIA own-window root");
    VARIANT value;VariantInit(&value);value.vt=VT_I4;value.lVal=UIA_SliderControlTypeId;
    Ref<IUIAutomationCondition> condition;requireHr(automation->CreatePropertyCondition(UIA_ControlTypePropertyId,value,condition.out()),"UIA slider condition");
    Ref<IUIAutomationElementArray> children;requireHr(root->FindAll(TreeScope_Subtree,condition.value,children.out()),"UIA slider enumeration");
    requireHr(children->get_Length(&sliders),"UIA slider count");check(sliders>=8,"UIA harness did not reach synth controls");
    Ref<IUIAutomationElement> slider;requireHr(children->GetElement(0,slider.out()),"UIA first slider");
    Ref<IUIAutomationRangeValuePattern> pattern;
    requireHr(slider->GetCurrentPatternAs(UIA_RangeValuePatternId,__uuidof(IUIAutomationRangeValuePattern),reinterpret_cast<void**>(pattern.out())),"UIA range pattern");
    double current=0;requireHr(pattern->get_CurrentValue(&current),"UIA value before close");
    requireHr(pattern->SetValue(.251),"UIA write before close");
    requireHr(pattern->get_CurrentValue(&current),"UIA readback before close");
    check(std::abs(current-.251)<.0001,"UIA write before close did not update control");
    stage=1;
    while(stage.load()<2)std::this_thread::sleep_for(std::chrono::milliseconds(1));
    afterClose=pattern->get_CurrentValue(&closedValue);
    writeAfterClose=pattern->SetValue(.252);
    if(SUCCEEDED(writeAfterClose))throw std::runtime_error("UIA write accepted after editor destruction; sliders="+std::to_string(sliders)+" readHR="+std::to_string(afterClose)+" value="+std::to_string(closedValue));
    // COM references, including the tree array, release here after editor deletion.
   }catch(const std::exception&e){error=e.what();}
   CoUninitialize();stage=3;
  });
  waitFor(stage,1);
  if(stage.load()==1){
   if(auto*b=dynamic_cast<juce::Button*>(find(*editor,cycle%2?"envelopePage":"arpPage")))b->onClick();
   editor.reset();stage=2;
  }
  waitFor(stage,3);client.join();editor.reset();processor.reset();pump();
  check(error.empty(),error.c_str());
  std::cout<<"UIA cycle "<<cycle+1<<": "<<sliders<<" sliders; retained read HRESULT="<<std::hex<<static_cast<unsigned long>(afterClose)<<"; rejected write HRESULT="<<static_cast<unsigned long>(writeAfterClose)<<std::dec<<"; retained value="<<closedValue<<'\n';
 }
 std::cout<<"PASS: eight isolated editor/UIA-reference cleanup cycles. No VST3 DLL unload or FL Studio reload tested.\n";
}
#endif
