# Evidence boundary for the fuller instrument

Reviewed 2026-10-28. The objective is a broader original Relic-inspired instrument.
Neither musical approval of our drive nor implementation of listed features
establishes an emulation of the hardware.

| Area | Published feature direction | Current study | Evidence still needed |
| --- | --- | --- | --- |
| Oscillators | Two SEM-topology VCOs per voice | Browser saw/square, fixed detune | Waveform captures, tuning and modulation behavior; no circuit values known |
| Filter | Cutoff, resonance, bandwidth, profile; classic and modern response families | One browser low-pass | Transfer/impulse responses, control interaction and nonlinear behavior |
| Envelopes / LFOs | Four dAHDSR envelopes and four eight-shape LFOs per voice | One velocity envelope | Retrigger, time/shape and modulation routing behavior |
| Voices | Eight cores and four-part allocation | Eight held browser notes | Native allocator, tails, stealing, repeated-note and channel/part tests |
| Drive | Expressive distortion described qualitatively | Original memory model, approved informally by Holger | Hardware recordings/measurements to establish any similarity |

The published core description mentions an SVF with an additional dipole and
gain cells. That does not specify a realizable full filter schematic. An ordinary
two-pole SVF links resonance and bandwidth; adding a second knob cannot by itself
establish the documented filter architecture. Start with declared mathematical
response targets and label any expanded architecture as our own design.

Human review correction to the PCLA outputs: the official specification explicitly
defines XF4 as a 50/50 blend of BP4 and LP2. The calibration stage incorrectly
calls its meaning unknown. The blend definition is known; underlying transfer
functions, gains and circuit implementation are not fully specified. Preserve
the raw model output as evidence and apply this correction to implementation.

Manufacturer sources (primary):
- https://www.shearelectronics.com/relic/specs/
- https://www.shearelectronics.com/relic/voice-core/

Engineering references (primary):
- Andrew Simper, linear trapezoidal SVF derivation:
  https://cytomic.com/files/dsp/SvfLinearTrapezoidalSin.pdf
  This supports studying implicit integration, not hardware identification.
- Steinberg, VST3 API and threading model:
  https://steinbergmedia.github.io/vst3_dev_portal/pages/Technical%2BDocumentation/API%2BDocumentation/Index.html
  Keep DSP usable without its edit controller; avoid audio-thread allocation.

## Preservation and implementation gates

1. Retain the frozen listening experiment and approved live default. Exact frozen
   PCM files have independently checked 48 kHz / 6.8 s format, -24 dBFS RMS within
   .001 dB and peak headroom; see outputs/ideation-1/pcm-verification.json.
2. Give native DSP explicit sample-rate, reset, note-event and parameter contracts.
   Compare deterministic offline renders and block-partition behavior before a UI.
3. Change one sound-generating module at a time; audition both isolated modules
   and their interaction with the approved memory drive. Keep rejected variants.
4. Add feature coverage and performance routing incrementally, measuring against
   our declared targets. Hardware similarity remains a separate research question.
5. Validate a VST3 in a host after DSP/event/state contracts work; packaging is not
   a substitute for developing the wider instrument.

The bounded PCLA investigation lives in outputs/voice-research-1. Its model
stages receive this task's researched facts, not direct autonomous web access.
Its final local audit checks plan structure only: no DSP/audio experiment is
marked passed merely because a plan is well formed.

Local toolchain inventory: cmake/clang++/g++/cl were not on the shell PATH, and
vswhere did not find an installation advertising the C++ tools component.
This does not prove no compiler is installed. Locate a suitable compiler/SDK
before choosing or installing dependencies; no installer was run.
