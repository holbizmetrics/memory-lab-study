# First standalone and VST3 build

Built and installed on2026-10-28: Memory Lab Study0.1.0, Windows x64, JUCE9.0.4,
MSVC19.52.37025 with CMake4.3. Dependency commit is recorded in dependency.json.

The standalone executable and VST3 bundle built successfully. Wrapper smoke checks
passed exact native-default parity, MIDI sample positions, stereo duplication,
channel-specific CC120 behavior, sustain, bend/pressure mapping, state recall and
MIDI block partitioning. The first smoke attempt failed default parity; the wrapper
now canonicalizes float defaults within a fraction of a control increment.

The separate validator scanned the actual VST3 bundle, classified it as an
instrument, instantiated it, produced MIDI audio, round-tripped state, reached
silence after release, and reloaded at44.1 kHz. This is a basic JUCE-host check,
not the full Steinberg validator or an FL Studio test. The native core checks also
passed after adding channel-specific dispatch to the wrapper boundary.

Installed bundle:
`C:/Program Files/Common Files/VST3/Memory Lab Study.vst3`

Standalone executable:
`outputs/plugin-build/MemoryLab_artefacts/Release/Standalone/Memory Lab Study.exe`

The installer found no existing bundle at the destination, copied the new bundle
and verified every file hash against the build. Other plug-ins were untouched.
The local source/build outputs remain in this standalone project. Nothing was
published or pushed. Dependency licensing/distribution decisions are documented
in README.md and dependency.json.

Next host check: FL Studio > Options > Manage plugins > Find installed plugins,
with Verify plugins enabled. Find Memory Lab Study, favorite it and load it as an
instrument in a new test project. Confirm audio, MIDI, UI controls, automation,
project save/reopen and release behavior. Target-host behavior and standalone UI/
audio-device operation have not yet been verified. The prototype remains partial:
one filter ADSR and one sine LFO, classic profiles, eight voices; full Relic feature
coverage and hardware matching are not claimed.

Actual-module check output: outputs/vst3-module-check.txt. Binary/source hashes and
installed-path evidence are saved in outputs/first-plugin-build.json.
