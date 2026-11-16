# Optional envelope character and timing — 0.22

AMPLITUDE, FILTER, ENV 3 and ENV 4 each add CURVE and TIME SCALE. These are
independent for every part. CURVE defaults to LEGACY and TIME SCALE to x1.
The six existing stage controls keep their host IDs, ranges and mapping.
Stages display base milliseconds; the effective duration is that value times
TIME SCALE. The graph uses effective times and the selected curve.

The new controls replace duplicated drive/depth or route knobs on the envelope
pages. DRIVE still exposes its controls. Envelope pitch/cutoff/PWM routes stay
on ROUTES; filter envelope amount is also on MOTION, VOICE / EXPRESSION and
FILTER / MODERN. The output fader remains available. No destination is removed.

## Curve contract

LEGACY preserves the original linear attack/decay and sample-by-sample linear
release arithmetic. With x1, existing sounds remain unchanged.

CURVED is an original, stage-specific approximation inspired by the historical
description, not a measured Relic/CEM3310 emulation. Attack rises toward 1.3
and ends at 1 after its set duration: `1.3*(1-exp(-ln(1.3/0.3)*t/T))`.
Decay/release use a normalized five-time-constant exponential tail that reaches
the stage endpoint at its set duration. Hold remains flat and sustain a level.
Release begins from the current level, including during attack or delay.
Zero-duration stages remain immediate. Extremely short stages can click;
that is not hidden by adding a mandatory fade.

Curve and timing settings latch when a new note starts, as existing stage
settings already did. Editing the graph/controls does not redraw a held note's
actual envelope. Play fresh notes to hear changes. Sustain, stealing and panic
keep their prior contracts; modulation-envelope tails do not extend amplitude
voice lifetime.

## Timing contract

TIME SCALE ranges from x0.010 to x20.000, independent of CURVE. At scales
other than x1, each nonzero timed stage is bounded to 0.1 ms–100 seconds;
zero stays zero and sustain is not scaled. For example, 10 ms x0.01 becomes
0.1 ms, and 10,000 ms x10 becomes 100 seconds. Delay/Hold can reach 100 seconds
with 5,000 ms x20. The cap means the scaling is not proportional beyond 100 s.
Near-x1 float roundtrip residue is canonicalized to x1 to preserve old sound.
The reported host tail is now 101 seconds, accommodating the maximum release.

## Persistence and compatibility

32 host parameters append after the original 425: for each part, amp, flt,
env3 and env4 each append Curve then TimeScale. Additional parts use their
existing `partN__` prefixes. Total 457; no old ID/order/range changes.
Patch schema 17 / host state 19. Older sounds restore LEGACY/x1 even in an
already edited instance; their LFO polarities are retained where present.
INIT and selected-part starter loading reset the new options; part copying
includes them. DICE preserves these options and its existing scope. New files
require the new plugin; source prereleases remain without binary assets.

## Verification

Automated checks cover independent formula references, curve range/monotonicity,
zero stages, early release, latched settings, four sample rates, timing bounds,
append-only IDs, migration/recall, copying/starter isolation, UI bindings and
curved multipart block partition agreement. Actual VST3 captures compare old
sounds byte-for-byte and render each envelope in both modes, fast/long timing
and a combined four-part case. These are separate from musical approval and
realtime CPU/aliasing or measured hardware-fidelity claims.
