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

## Supplement: supplied Deep Research report (2026-11-14)

Holger supplied a timestamped research report, duplicated in the chat; treat
it as one report. The following are useful verification leads, not independently
checked video findings. They qualify several earlier uncertainties and should
be reviewed before their respective implementation stages.

| Lead reported | Source passage | Consequence if verified |
| --- | --- | --- |
| Hard sync, selective glide and key/bend tracking on the eight-voice 2022 prototype | [Sonicstate 7:38–8:03](https://www.youtube.com/watch?v=CUqPG2ycv1A&t=458s); [2024 sync patch 6:46](https://www.youtube.com/watch?v=fXIGjvYH1cM&t=406s) | Sync evidence extends beyond 2017; production reset law remains unknown. Add glide/tracking to oscillator scope review. |
| Three FM slots and two PWM slots per oscillator | [2022 walkthrough 8:38–8:56](https://www.youtube.com/watch?v=CUqPG2ycv1A&t=518s) | Independent routing warrants closer inspection; do not infer audio-rate oscillator cross-FM from slot names alone. |
| Envelope/LFO levels can themselves be modulated; signed envelope-time example | [2022 17:10–18:40](https://www.youtube.com/watch?v=CUqPG2ycv1A&t=1030s) | Our fixed-depth routes do not cover modulation of modulators. Review as a separate bounded routing stage. |
| Bandwidth affects low-pass behavior as well as band-pass | [2022 14:32–15:08](https://www.youtube.com/watch?v=CUqPG2ycv1A&t=872s) | Important possible fidelity gap: our modern bandwidth deliberately leaves LP2/LP3/LP4 unchanged. Do not change that approved behavior from a report alone; verify revision and seek response evidence. |
| ENV3 assigned to filter profile | [2026 demonstration 17:33–17:53](https://www.youtube.com/watch?v=7cFAWbFDX9k&t=1053s) | Concrete source/destination example for proposed 0.23. |
| All eight voices assigned to patch B | [2026 demonstration 6:10–6:33](https://www.youtube.com/watch?v=7cFAWbFDX9k&t=370s) | Our Single mode uses P1 only; flexible single-part assignment merits verification in allocation scope. |
| Explicit MIDI/MPE discussion | [2026 26:15–26:31](https://www.youtube.com/watch?v=7cFAWbFDX9k&t=1575s) | Stronger lead than merely inferring MPE from finger bends. Protocol, zones and mappings still need verification/documentation. |
| Internal arp/sequencer assertion | [2022 8:03](https://www.youtube.com/watch?v=CUqPG2ycv1A&t=483s) | Our arp is not automatically unrelated to all hardware generations; production function/timing remains unestablished. |

The report also flags 2024 XF2/Normal terminology versus current XF4/Modern,
including an older panel label. Retain a documentation-conflict note; do not
invent a hardware revision or change current filters to resolve the names.
The system-architecture page is reported accessible without `www` and marked
unfinished; its dated repository statement is not a current repository search.

The shortened link supplied by Holger resolves to the already-known
[2017 Matrixsynth page](https://www.matrixsynth.com/2017/01/namm-2017-shear-electronics-relic-6-new.html).
Tracking-query variants are the same source. Historical 40 kHz/14-bit envelope
claims in that page must not be transferred to production hardware.

Next evidence checks: confirm the bandwidth passage, routing/modulator passage
and explicit MPE passage first. The proposed 0.21 LFO-polarity step remains
supported by written manufacturer specs and need not depend on those videos.
