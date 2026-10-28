# Native foundation

Framework decision: JUCE for the eventual VST3/standalone wrapper and UI, with
this C++17 DSP independent of JUCE. JUCE has not been downloaded, linked, licensed
on Holger's behalf or installed in a host by this milestone. Use a pinned JUCE
release when adding the wrapper; record the selected licence then. Current
official build and licensing information was checked on 2026-10-28:
https://juce.com/tutorials/tutorial_create_projucer_basic_plugin/
https://juce.com/get-juce/

## Build and run on this machine

```powershell
cmd /c native\build-native.cmd
node native\compare-worklet.cjs
```

The build helper uses the installed Visual Studio 2026 Insiders MSVC compiler
and bundled CMake. It configures an NMake Release build, runs tests and creates
outputs/native-phrase.wav (mono 48 kHz float WAV). Paths in the helper are local;
the CMake project itself is portable. No SDK/dependency installer is needed for
this executable. An earlier shell inventory failed to find the compiler on PATH;
direct inspection found a working toolchain, correcting that uncertainty.

## Contracts and current sound

Synth.prepare accepts 44.1–192 kHz; invalid rates return false without resetting.
reset clears voices, filter/memory state, age and parameters to the default patch.
render accepts caller-owned output and sorted events with sample offsets strictly
within that block. Same-offset events are processed in caller order before that
sample. Invalid offset/order/null-event blocks return false and write silence,
without mutating DSP state. A null output returns false. Parameter values are
clamped; nonfinite parameter/note velocities and invalid note/channel values are
ignored. Note-on velocity is normalized 0..1, with zero acting as note-off.

Eight fixed voice slots; same channel/pitch retriggers its existing slot; an
additional note steals the oldest allocated slot. Note-off owns channel/pitch.
AllOff releases every active voice over 500 ms. Sustain, pressure, pitch bend,
MPE and multi-part allocation are future work. Stealing is abrupt and still needs
click mitigation before production use. Phase mode currently selects reset versus
retaining a slot's phase; it is not a fully free-running inactive oscillator.

Original Fourier wavetable saw/pulse oscillators, detune, pulse-width control, velocity
envelope and trapezoidal state-variable low-pass feed the shared memory drive.
This voice/filter differs from both browser and frozen source. No aliasing,
tuning or perceptual-equivalence claim is made beyond the recorded audit cases.
The older polyBLEP routines remain for comparison. See WAVETABLE-CANDIDATE.md.
The SVF
uses the standard implicit integration equations; engineering reference:
https://cytomic.com/files/dsp/SvfLinearTrapezoidalSin.pdf
It does not reproduce Relic's undocumented supervariable circuit.

Default drive 5/depth .8/recovery 250 ms ports the approved live settings, including
smooth depth fading below drive2 and the DC blocker. The code retains the browser
stage's four interpolated substeps and averaging, with its aliasing limitations.
Live volume is .25; native phrase output is raw, not -24 dBFS matched.

## Verification completed

- Three sample rates: finite bounded default-phrase output, exact reset and block
  partition agreement, release completion, fixed voice capacity and all-off.
- Channel-specific note-off, declared repeated-note policy, invalid-block silence
  and state preservation. C++ new allocation counter detected none during the
  tested render. This does not prove every realtime performance property.
- Actual live-drive.js algorithm executed in a Node worklet stub versus the
  compiled native MemoryDrive over 48,000 samples at 48 kHz: max and RMS float
  output error both zero, including drive/depth/recovery changes. See
  outputs/native-worklet-comparison.json. This is an algorithm comparison, not
  browser/device timing or whole-instrument equivalence.

Next: freeze oscillator pitch/spectrum/transient criteria, test the native source
before and through drive, then audition level-matched renders. Only after that
extend profile filters/modulation and integrate the chosen JUCE wrapper.
