# Next milestone: deterministic native voice core

Decision after PCLA voice-research-1 (2026-10-28): establish the native DSP and
event boundary first, then oscillator relationships, classic filter profiles,
and explicit modulation/voice ownership. VST3 packaging follows this work.
The live browser study remains playable, and its original frozen audio is retained.

## What PCLA contributed

Four attended Codex calls ran the existing PCLA image, variants, calibration and
refinement contracts. All three original and three refined experiment descriptions
were retained and passed local JSON-field checks. Stage five here is a plan
integrity audit, not the earlier numerical runner and not a DSP test. Prompts,
contracts, raw replies, receipts and the audit are in outputs/voice-research-1.
The engine did not independently browse: researched facts were supplied as input.

Useful refinements were source tests both before and through drive; separately
specified control targets; near-notch error rules; and explicit event ownership
for repeated notes, sustain, pressure and voice stealing. Shared memory drive
coupling must not be confused with unintended coupling inside individual voices.

An incorrect claim survived calibration and refinement: XF4's meaning was called
unknown. Manufacturer text defines a 50/50 BP4/LP2 blend. Apply the correction in
VOICE-EVIDENCE.md; preserve the raw mistake rather than rewriting the receipts.
Generated numerical tolerances are proposals, not measured properties or approved
acceptance criteria. They must be completed and frozen before implementation tests.

## Implementation order and reviewable deliverables

1. Portable native core with prepare/reset/render and sample-offset note/parameter
   events, a fixed eight-voice pool, no render-time allocation, and an offline WAV
   harness. Port the approved memory stage with explicit initial-state fixtures.
   Verify reset reproducibility, block partitioning and sample-rate behavior.
2. Two oscillators with separately adjustable detune, pulse width and phase policy.
   First audition fixed axes around the current seven-cent baseline. Test tuning,
   spectrum and transients across register before adding drift or interactions.
3. An original classic HP/notch/LP profile filter with declared response equations.
   Compare static responses, impulse decay and automation; only then extend slopes
   and the additional bandwidth architecture. Keep resonance/bandwidth coupling
   explicit and avoid claiming the manufacturer's undocumented circuitry.
4. Envelopes/LFOs and pressure routing, then sustain and reliable eight-voice
   allocation, then four-part routing. Add full four-envelope/four-LFO coverage
   incrementally; an initial modulation slice is not the complete instrument.
5. VST3 processor/controller, state persistence, automation and host validation,
   with the offline DSP harness remaining independently usable.

The first implementation target is items 1 and 2, not all items simultaneously.
Compiler/SDK discovery is still needed; no dependency installation or host/plugin
deployment has been performed. Hardware reference recordings will be needed for
any claim of resemblance beyond feature inspiration. Current listening approval
applies to our timbre and response, not the Relic's measured behavior.
