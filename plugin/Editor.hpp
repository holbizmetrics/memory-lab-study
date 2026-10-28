#pragma once
#include "Processor.hpp"

class MemoryLabEditor final:public juce::AudioProcessorEditor {
public:
 explicit MemoryLabEditor(MemoryLabProcessor&);
 ~MemoryLabEditor()override;
 void resized()override;
private:
 class Surface;
 std::unique_ptr<Surface> surface;
 juce::TooltipWindow tooltips{this,500};
 JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MemoryLabEditor)
};
