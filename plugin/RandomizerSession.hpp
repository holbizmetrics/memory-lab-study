#pragma once
#include <array>
#include <juce_core/juce_core.h>
namespace memorylab {
// Per-instance UI session state: not a sound parameter or saved project setting.
struct RandomizerSession {
 std::array<bool,4> locked{};
 juce::String beforeRoll,afterRoll;
};
}
