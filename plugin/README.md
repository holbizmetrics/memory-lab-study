# Memory Lab Study — first JUCE wrapper

This wrapper builds the independently developed native engine as a Windows x64
VST3 instrument and standalone application. It is an original subtractive synth
study, not a complete Relic instrument or a measured hardware emulation.

JUCE source is pinned by plugin/dependency.json. No account or paid plan was
selected. Upstream offers AGPLv3 and JUCE commercial licensing; upstream source
and notices remain in the local ignored dependency checkout. No binaries are
published by this task and no whole-project licence declaration is made here.
Select/document an appropriate distribution licence before a public release.
Primary framework information: https://juce.com/get-juce/

## Build

```powershell
git clone --depth 1 --branch 9.0.4 https://github.com/juce-framework/JUCE.git outputs/deps/JUCE
cmd /c plugin/build-plugin.cmd
```

The helper uses the installed VS2026 Insiders MSVC and CMake. Build outputs are
ignored. Do not clone again over an existing dependency; verify its pinned commit.
The CMake source accepts JUCE_PATH for another checkout. It does not copy plug-ins
into system directories automatically. Framework version9.0.4 commit:
3fb5cbc0340758596dac39f7c8de47d562b4896e.

## UI and state

Initial editor is JUCE's plain generic parameter editor. The sixteen controls
cover drive/memory, detune/width, cutoff/Q/profile, output gain, LFO rate and
depths, filter envelope amount, pressure routing and phase reset. There is no
custom artwork, browser embed, factory-preset bank or on-screen keyboard yet.
The default patch preserves the approved native baseline. Float host defaults
are mapped to the exact corresponding native default values.

Host state saves/restores parameter values using the APVTS XML/binary format.
It does not save held notes, oscillator phases, pedal or other transient controller
states. One default program is advertised; use host project/preset state for now.
Parameter updates reach the core at block boundaries and are smoothed where the
core contract specifies. Sample-accurate host parameter automation is not claimed.

## Audio/MIDI boundary

Instrument input: MIDI, no audio input. Output: mono or stereo with duplicated
mono signal, zero reported algorithmic latency and1.5-second advertised tail.
Core sample rates:44.1–192 kHz. Unsupported rates produce silence; no lower-rate
support is claimed. Only32-bit float processing is advertised. No processing-time
allocation is introduced by the native adapter, but complete JUCE/host real-time
allocation/performance certification is not claimed.

The wrapper renders between MIDI sample offsets, dispatching events in buffer
order. It maps note-on/off, normalized velocities, pitch wheel endpoints/center,
channel pressure, CC64 sustain, CC120 channel sound-off, CC123 channel note-off,
and CC121 sustain/bend/pressure reset. Note-on velocity0 acts as note-off.
Pitch bend range is fixed +/-2 semitones. Polyphonic pressure, MPE, note IDs,
MIDI2, transport sync and program changes are not implemented. MIDI is consumed,
not emitted. Channel commands preserve other channels. Resetting controllers uses
the core's smoothing for bend/pressure. All state access/control updates must
follow JUCE's host threading contract.

## Expected artifacts

- outputs/plugin-build/MemoryLab_artefacts/Release/VST3/Memory Lab Study.vst3
- outputs/plugin-build/MemoryLab_artefacts/Release/Standalone/Memory Lab Study.exe

Build success alone is not FL Studio verification. See the generated build/check
report for what actually passed. The wrapper smoke tests instantiate the processor
directly. An actual-module scanner/loader check is separate from those tests.

FL Studio requires VST3 in a standard system folder, not an arbitrary custom VST2
search path. After the binary passes checks, install-for-fl.ps1 copies only this
new bundle to C:/Program Files/Common Files/VST3/Memory Lab Study.vst3 and verifies
all copied files. It refuses to overwrite an existing destination. Then use
Options > Manage plugins > Find installed plugins with Verify plugins enabled,
find Memory Lab Study and add it as an instrument. Host loading, UI interaction
and hardware MIDI must still be confirmed in FL Studio.
Primary host instructions:
https://www.image-line.com/fl-studio-learning/fl-studio-online-manual/html/basics_externalplugins.htm

The standalone application has its own audio/MIDI-device settings; enable your
controller there. The earlier browser prototype remains independent and unchanged.

## Musical evidence preserved

Holger approved all four native performance demonstrations as working as intended
and announced. This follows approvals of the filter and modulation studies.
That feedback applies to the existing listening examples; the new JUCE interface
and actual VST3 host behavior require their own checks.
