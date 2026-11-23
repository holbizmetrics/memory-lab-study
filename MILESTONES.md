# Memory Lab: visible progress toward the Relic-inspired instrument

Updated 2026-11-17. An implementation milestone and musical approval are separate.
This is an original instrument inspired by documented Relic features, not yet a
faithful hardware emulation. There is no defensible completion percentage: one
four-part voice architecture can be more work than several UI milestones.

## Current checkpoints

| Area | Implemented and technically tested through 0.23 | Remaining boundary |
| --- | --- | --- |
| Voices/parts | Eight voices, four sounds, layers/split/channels; Holger approved routing and recall | Fixed reservations; hardware allocation policy unknown |
| Oscillators | Four saw/pulse levels, separate static widths, OSC 2 tuning/PWM, noise | OSC 1 PWM, measured SEM behavior; current-model sync evidence incomplete |
| Filters | Classic sweep and modern BP4/XF4/LP2/LP3/LP4 profile with bandwidth; approved | ENV 3/LFO 1 profile/Q routes added; new listening pending. Exact hardware laws, nonlinearities and extended LP bandwidth remain |
| Envelopes | Four dAHDSR contours, optional curves/time scale; amplitude and recall approved | Measured hardware laws and further time modulation remain |
| LFOs | Four sources with four shapes, bipolar/unipolar options and explicit routes; approved | Further evidenced routing, ranges and modulation of modulators |
| Output/drive | Post-filter voice clipping, shared memory drive and bypass | Original drive; post-filter amp VCA, stereo voice pan, unison and multi-out incomplete |
| Workflow | Whole-setup patch/project recall, INIT, Dice, starters, auto-load; approved | Full FL automation/undo and library extras |
| Integration | 21 plugin checks, 56 exact prior VST3 captures; live approval through 0.22 | Realtime CPU/dropout, aliasing, long playing and hardware equivalence |

See the [fresh hardware audit](RELIC-COVERAGE-AUDIT.md) for evidence levels and
the provisional 0.21–0.28 route. The older schedule below is historical.

## Proposed next checkpoints — scope can split when needed

| Version | Concrete next goal | What it advances |
| --- | --- | --- |
| 0.13 | Starting presets, category browser, explicit load and undo | Workflow; no major synthesis architecture gap closed |
| 0.14 | Second independent LFO and recall tested/approved by Holger | Two-source independent simultaneous modulation |
| 0.15 | LFO 3/4 solo/combined and patch/project recall confirmed | Closes LFO-count gap; new-source musical test next |
| 0.16 | ENV 3/4 listening and patch/project recall confirmed | Documented envelope-count gap |
| 0.17 | Tested routes, auto-load and patch/project recall confirmed | Source/destination flexibility |
| 0.18 | First oscillator waveform step technically verified; modern filter step remains | Source and filter architecture; split if needed |
| 0.19 | Four-part allocation, layers and splits | Part/voice architecture; likely multiple milestones |
| 0.20 | Cross-feature, compatibility, CPU and DAW integration checks | Evaluate the combined instrument, not individual modules alone |

These are proposed checkpoints, not a frozen schedule or a guarantee of hardware
completion by 0.20. Scope each against manufacturer evidence before implementation.
The next synthesis milestones take priority over additional convenience features
after the already-started 0.13. Keep the approved sound, stable parameter IDs and
old-patch defaults throughout. Matching hardware tone/control behaviour requires
reference measurements; feature count alone cannot establish it.

Holger completed the combined 0.13/0.14 listening round and explicitly approved
continuing to four LFOs. The 0.15 listening round confirmed LFO 3 filter and LFO 4 pitch, both together,
patch-file recall and FL-project recall after restart. Stop after 0.16 delivery
for the new-envelope listening round before proceeding to broader routing.
Holger has now confirmed ENV 3 pitch, ENV 4 cutoff and patch/FL restart recall.
0.17 technical checks pass; its new routes and auto-load interaction await their
own listening/UI check. Installation status is separate from source publication. No new-source approval is
inferred from his enjoyment of the first two sources or from automated checks.

Evidence: [recorded voice architecture](VOICE-EVIDENCE.md),
[interview evidence](RELIC-INTERVIEW-EVIDENCE.md), [roadmap/history](ROADMAP.md).

0.17 listening update: Holger confirmed LFO 1 tremolo/bypass, ENV 3 PWM,
filter-envelope and amplitude-envelope pitch/bypass, patch recall, FL restart
recall and auto-load preference. No blanket approval of every new route implied.
0.18 complementary-waveform listening is next.

0.18 alpha.2: patch endpoint recall fix and VOICE hint spacing; oscillator width and OSC 2 octave listening confirmed. Original blend patch now passes read-only loader checks; live recall pending. No DSP or parameter-layout changes.

0.18 alpha.2 listening update: Holger confirmed the unchanged Four Wave Blend
file loads, sounds correct, and survives FL project close/reopen without manual
patch loading. Patch endpoint repair and project recall are now human-confirmed.

0.18 alpha.3: opt-in FILTER / MODERN profile/BW controls, 106 total; original
103 IDs/order preserved. Schema 14 / host state 16 migrate old sounds to Modern
off. 16 plugin and 7 native suites pass; all 16 legacy family/drive/noise
captures match installed alpha.2. New modern-filter listening/recall pending.

0.18 alpha.3 listening update (2026-11-12): Holger confirmed Modern on/off,
profile sweep, audible/visible BP4 bandwidth change, Classic/BW inactive states,
and Modern LP2 Test (300 Hz) patch recall plus FL project close/reopen recall.
The planned listening round is complete; no hardware-equivalence claim.

Next: four-part allocation foundation, with eight total voice slots and fixed
part reservations, keyboard zones and MIDI channel routing. The native policy
is isolated until DSP/part state and host integration are ready. No new FL
controls or delivered 0.19 version are implied by the foundation alone.

Four-part native audio checkpoint (2026-11-13): independent part sounds now
render through the eight-slot allocator. 9/9 native suites pass, including
four-rate exact one-part parity, independent four-synth reference summation,
layer block parity, voice quotas, split/channel/sustain/panic handling,
independent noise and no audio-path heap allocations. The pre-refactor native
phrase is byte-identical. Plugin part controls, host/patch state and musical
approval remain pending; no new installed or numbered synth release yet.

0.19 alpha.1 source checkpoint: Parts UI, independent sound editing and
complete patch/host state are connected. Single / 2-layer / 2-way split /
4-layer, split note, four MIDI channels, Copy Part 1, selected-part presets and
Dice. 409 parameters, first 106 preserved; schema 15, state 17. 17 plugin and
9 native suites pass; 21 actual old filter captures and earlier sound fixtures
remain byte-identical. New musical/live FL approval was pending at publication.

0.19 alpha.1 listening update: Holger confirmed Round Bass + Slow Air layering,
the MIDI-60 keyboard split, INIT/whole-patch reload and FL project reopen recall.
Four-layer use showed 4 voices for one key and 8 for two; channel 1/2 isolation
was confirmed in both directions. The planned listening round is complete.

Next: 0.20 combined integration checks across part routing, host tempo,
modulation, envelopes, filters, drive, state and automation. No added control
count or hardware-equivalence claim is implied by this quality checkpoint.

0.20 alpha.1 listening update: Holger confirmed the fresh 0.20 instance,
Bass and Air Split, Slow Wah Fast Heart and normal release/no stuck notes.
The short regression round is complete. Next: refresh hardware coverage from
manufacturer evidence, separating implemented features from approximations
and from behaviors whose hardware specification is still unknown.

0.21 alpha.1: per-source/per-part LFO polarity, CLOCK page control and target
waveform preview. 425 parameters with the first 409 preserved, schema 16 / state
18 and bipolar legacy migration. 19 plugin and 9 native checks pass; 37 actual
0.20 recordings stay exact, eight polarity fixtures differ as intended. New
musical listening pending. AMP DEPTH retains its full attenuation cycle.

0.21 listening update (2026-11-15): Holger confirmed PULSE and SINE polarity
behavior, patch recall, project recall after FL close/reopen, and successful
loading of a 0.17 .mlpatch. The planned listening round is complete.

0.22 alpha.1: optional per-envelope/per-part curved character and timing scale.
457 controls, first 425 IDs/order/ranges preserved; schema 17 / state 19.
All 20 plugin checks pass (new UI helper ID corrected and its check rerun),
9/9 native checks pass; 45 actual prior VST3 captures stay byte-identical.
Eleven new module fixtures cover all four contours, short/long timing and
combined parts. Normal/compact previews inspected. Musical/live recall pending.

0.22 listening update (2026-11-16): Holger confirmed amplitude Legacy/Curved
contours, x0.250 versus x2.000 timing, “0.22 Curved Swell” patch recall and
FL project recall after completely closing/reopening FL. Planned round complete;
no claim of exhaustive live approval of every envelope/routing combination.

0.23 alpha.1: FILTER / MOD ROUTES, per-part ENV 3/LFO 1 profile and Q depths.
473 controls, first 457 preserved; schema 18 / state 20. All depths default
inactive. 21/21 plugin and 9/9 native checks pass. 56 actual old recordings are
byte-identical; 13 new actual VST3 examples pass. New-route musical/live recall
approval pending. Base-response graph is explicitly static; source steps remain.

0.23 listening update (2026-11-21): Holger heard each of the four new routes in
FL Studio. A WAV analysis identified beating between detuned oscillators in the
initial ENV 3 profile test. Muting OSC 2 isolated the single envelope sweep.
This listening round covered individual routes; full live project recall was
not repeated for 0.23.

0.24 alpha.1: six independent OSC 1 PWM depths per part, 497 controls, patch
schema 19 and host state 21. All 22 plugin checks and 10 native checks passed.
Thirteen 0.23 VST3 recordings stayed exact and fourteen new captures passed.
OSC 1 modulation leaves OSC 2 unchanged. FL listening is pending.

0.24 listening update: Holger confirmed OSC 1 PWM from LFO 1 and ENV 3
independently, OSC 2 isolation, and recall from a saved 0.24 patch in FL Studio.

0.25 alpha.1: DRIVE / AMP POSITION for Legacy and Post-filter, latched per
note. 501 controls, patch schema 20 and host state 22. All 23 plugin checks and
11 native checks passed, alongside 27 exact prior and eight new VST3 captures.
FL listening is pending.

0.25 listening update: Holger heard the Legacy/Post-filter difference
with both clean and driven sounds, and confirmed an older 0.24 patch loaded
correctly in FL Studio. New-patch and project recall were verified offline.

0.26 alpha.1: VOICE / EXPRESSION / STEREO SPREAD per part. 505 controls,
patch schema 21 and host state 23. All 24 plugin and 12 native checks passed,
with 35 prior VST3 recordings exact in both channels and three new stereo
fixtures. FL listening is pending; unison remains separate.

0.26 alpha.1 listening update: Holger heard stereo spread with two notes in FL.

0.26 alpha.2: VOICE / EXPRESSION adds per-part 1-, 2- and 4-voice unison with
symmetric detune and the original eight-voice limit. 513 controls, patch
schema 22 and host state 24. All 25 plugin and 13 native checks passed.
38 earlier VST3 recordings stayed exact in both channels and four new
stack captures passed. FL listening and project recall are pending.

0.26 alpha.2 listening update: Holger confirmed the width and tone of the
two- and four-voice stacks, and recalled four-voice unison after reopening
the FL Studio project.

0.27 alpha.1: PARTS / MPE PITCH adds opt-in lower-zone member pitch and
manager bend, sustain and panic. 514 controls, patch schema 23 and host
state 25. All 26 plug-in and 13 native checks passed, including measured
pitch checks through the built VST3. 42 prior captures and Two Lanterns
matched both channels exactly. FL routing, listening and recall are pending.

0.27 alpha.1 listening update: Holger used two FL Studio MIDI Out channels
on internal port 20 and heard channel 2 bend while channel 3 stayed steady.
He confirmed MPE PITCH and the bend routing survived reopening the project.

0.27 alpha.2: VOICE / EXPRESSION / MPE PRESSURE routes member aftertouch
to cutoff. Four controls append for 518 total, patch schema 24 and host
state 26. 27 plug-in and 13 native checks passed. The built VST3 passed
pitch and pressure checks, and 42 prior captures plus Two Lanterns matched
in both channels. Holger confirmed the existing PRESSURE route using the
FL MIDI Out AFT knob. Dedicated MPE pressure listening and recall are pending.

0.27 alpha.2 listening update: Holger heard positive pressure depth and
confirmed that at −2.00 with the original PRESSURE depth at zero, channel 2
darkened while channel 3 stayed steady. He repeated the sweep successfully
after reopening the FL project. Internal port 20 worked with loopMIDI closed.

0.27 alpha.3: FILTER / MOD ROUTES / MPE TIMBRE adds bipolar CC 74
to cutoff per part. 522 controls, schema 25 and host state 27. 28 plug-in
and 13 native checks passed. Pitch, pressure and timbre passed in the built
VST3. 42 existing captures and Two Lanterns stayed exact in both channels.
FL timbre listening and project recall are pending.

0.27 alpha.3 listening update: Holger confirmed that CC 74 on MIDI Out
channel 2 changed C4's tone while G4 on channel 3 stayed steady. He confirmed
that MPE TIMBRE at +2.00 and the routing worked after reopening the FL project.

0.28 alpha.1: MOTION / LFO 1 / ENV 3 FADE shapes LFO 1 depth per part and
per note. 526 controls, schema 26 and host state 28. All 29 plug-in and
13 native checks passed, including fade endpoints, partial depth, tremolo
and vibrato. LFO 2 isolation, patch migration and host recall passed.
42 previous VST3 captures and Two Lanterns stayed exact. FL listening and
project recall are pending.
