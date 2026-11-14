# Relic coverage audit — verified Memory Lab 0.20 baseline

Reviewed 2026-11-14. Target: the later eight-voice Relic, keeping the 2017
Relic-6 evidence separate. This is a feature and evidence audit, not a measured
sound match. No synth code, installed plugin or approved sound changes here.

## Evidence and limits

The [manufacturer specs](https://www.shearelectronics.com/relic/specs/) list
eight voices, two VCOs per voice, four dAHDSR envelopes, four LFOs with eight
unipolar/bipolar waveform choices, classic/modern filter sweeps, and four-part
allocation. They describe exponential, CEM3310-inspired contours. The
[voice-core page](https://www.shearelectronics.com/relic/voice-core/) identifies
SEM oscillators, a state-variable filter plus an extra dipole and seven VCAs.
These establish architecture, not coefficients, control laws or measured tone.

The [July 2026 manufacturer newsletter](https://www.shearelectronics.com/relic/status/2026-0720-los-angeles-demo-room/)
identifies Superbooth 2026 demonstrations as production hardware. The
[status index](https://www.shearelectronics.com/relic/status/) currently links
that newsletter and January 2024 updates. No public operator/service manual
or schematic was located in this pass; this does not establish their absence.
The old `/relic/system/` address could not be fetched. Search results contained
unrelated manuals; those were excluded. No newly located full manual closes
the remaining numerical or routing uncertainties.

[Previously supplied interview evidence](RELIC-INTERVIEW-EVIDENCE.md) adds
creator statements about signal order, stereo/panning, unison, modulation and
envelope timing. Those are transcript evidence with the provenance limits
recorded there, not newly verified measurements. Rolling press articles mix
2017, 2022, 2024 and 2026 descriptions; they do not override current primary specs.

## Implementation comparison

| Area | Verified 0.20 implementation | Audit result / remaining evidence |
| --- | --- | --- |
| Voice and part counts | Eight active voices; four editable sounds, layers, split and MIDI channels | Counts covered and musically confirmed. Fixed reservations are our policy; exact hardware allocation rules unknown. |
| Oscillator sources | Two digital phase cores, four saw/pulse levels, independent static widths, OSC 2 interval/fine tuning | Functional foundation present; no measured SEM waveform, tuning or drift match. PWM currently changes OSC 2 only. |
| LFOs | Four per-voice sources, four bipolar shapes, clock/reset controls, pitch/PWM/cutoff/amp routes | Confirmed shape-polarity gap: unipolar variants missing. Hardware warp/rate limits and route table need evidence. |
| Envelopes | Four dAHDSR contours with linear attack/decay/release; settings latch on note-on | Count/stages covered; exponential contour behavior missing. Our stage range tops at 10 seconds; the supplied later interview states 0.1 ms–100 seconds. Exact curves and retrigger laws unknown. |
| Filters | Continuous classic profile; opt-in modern five-anchor profile and bandwidth | Top-level profile coverage implemented and approved. Interpolation, bandwidth law, resonance/gain and nonlinear behavior are approximations. |
| Modulation | Explicit summed routes to pitch, OSC 2 width, cutoff and amplitude | Resonance/profile modulation described in supplied later interview is missing. General routing, wheel and per-note expression remain incomplete or unspecified. |
| Signal order and drive | Source mix multiplied by amp envelope before filters; post-filter voice clip; shared per-part memory drive after sum | Interview describes a post-filter output VCA. Our memory drive is an original experiment, not evidence of hardware envelope memory. Preserve legacy path; compare any alternative separately. |
| Stereo and outputs | Mono voice signal duplicated to stereo; summed parts | Interview describes voice panning/stereo VCAs and four outputs. Missing here; exact output assignment/control needs verification. |
| Unison | Polyphonic allocation only | Four/eight-voice unison described in supplied later interview; detune, pan and stealing rules unknown. |
| Storage and interface | Whole-setup files/host state, eight starter sounds, contextual knobs and plots | Useful software equivalent, not the hardware program bank or physical lightfaders. File count need not mimic fixed slots. |
| Sequencing and arp | Global software arp | Early interview distinguishes a 64-step polyphonic sequencer. It does not prove current-model sequencing. Our arp is an extra, not verified hardware parity. |
| Reliability and fidelity | 18 plugin checks, 34 unchanged actual VST3 recordings, combined fixtures, live regression approval | Backward sound preservation demonstrated; CPU/dropout, aliasing, long playing and measured hardware equivalence remain open. |

Implementation anchors: `native/voice.hpp` (`lfoSample`, `envelopeLevel`,
`FilterEnvelope`, `Synth::tick`), `native/part-allocator.hpp`,
`plugin/Processor.hpp`, [modern filter research](native/MODERN-FILTER-RESEARCH.md)
and [0.20 integration evidence](plugin/INTEGRATION.md).

## Corrected priority and proposed route

These scopes can split or change. Version numbers are provisional, not a
promise that 0.28 finishes the conversion. Each delivered numbered step still
needs tests, a source prerelease and Holger's listening round.

1. **0.21 — LFO polarity:** add unipolar choices without reordering or widening
   existing shape parameters. Prefer appended, neutral-default controls. Preview
   and destination math must agree; audit tremolo's bipolar assumptions first.
   Gate: old captures exact; all four sources, signs, recall and mixed parts tested.
2. **0.22 — Envelope character:** optional exponential contours and an additive
   timing design that can cover the interview's range without changing old host
   normalization. Do not call a guessed curve an exact CEM3310 emulation.
   Gate: reference stage plots/audio, edge/release tests and old-patch parity.
3. **0.23 — Filter modulation:** explicit profile/resonance routes with bounded
   smoothing and useful visual feedback. Establish a route contract before adding
   a general matrix. Gate: isolated/combined source tests, stability and headroom.
4. **0.24 — Oscillator coverage:** independent PWM first; investigate sync and
   cross-modulation only against revision-specific evidence. Early sync evidence
   is not automatic proof for production Relic. Gate: spectral/aliasing checks,
   matching UI and repeatable single-note/chord comparisons.
5. **0.25 — Voice-path study:** opt-in post-filter amplitude/VCA architecture and
   characterized saturation. Keep the approved legacy path/default intact.
   Gate: matched-level clean/drive listening, noise interactions and old captures.
6. **0.26 — Stereo and unison:** investigate voice pan, output buses and 4/8-voice
   stacks. Distinguish useful software policy from verified hardware rules.
   Gate: routing/voice-cap/normalization tests and live MIDI behavior.
7. **0.27 — Parts and expression:** extend zones/reservations or performance
   routing where hardware evidence justifies them. MPE is not inferred solely
   from a demonstration of individual-key bends. Gate: controller/state tests.
8. **0.28 — Fidelity and release review:** controlled hardware references where
   available, realtime performance, aliasing and full FL automation/undo/recall.
   Declare remaining limits explicitly; absent hardware data means approximate,
   not verified identical. Licensing/distribution stays a separate decision.

Hardware reference collection should run alongside these stages, not wait for
0.28. Useful recordings need revision, settings, dry audio, note/velocity and
sample rate; compressed mixed demos alone cannot identify circuit equations.
Readable preset recipes and other conveniences remain recorded extras, behind
the core conversion work. No new user listening test is needed for this audit.
