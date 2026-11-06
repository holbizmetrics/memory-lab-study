#pragma once
#include <juce_core/juce_core.h>
namespace memorylab {
struct PresetSession {
 int selected=0;
 juce::String beforeLoad,afterLoad;
};
}
