# Drive Memory Lab — standalone PCLA-assisted audio investigation

0.20 is the [combined-feature integration checkpoint](plugin/INTEGRATION.md).
It adds no sound controls: the approved default, 409 IDs/order and old patch
compatibility remain. Thirty-four actual VST3 recordings are unchanged;
four-rate combined tests cover parts, modulation, filters, drives and arp.
See [prerelease notes](releases/v0.20.0-alpha.1.md) for validation limits.

0.19 connects [four editable parts](plugin/PARTS.md): Single, 2-layer, 2-way
split and 4-layer within eight voices. Existing sound pages follow part
selection; SAVE/LOAD store all parts. The first 106 IDs/order and earlier
sounds are preserved; 409 controls total. Holger confirmed layers, split,
patch/project recall, voice reservations and MIDI routing.
[Prerelease notes](releases/v0.19.0-alpha.1.md) retain the delivery-time status.

0.18 adds [complementary waveforms](plugin/VCO-MIX.md) and a
[continuous modern filter profile with bandwidth](plugin/MODERN-FILTER.md).
FILTER / MODERN offers BP4 / XF4 / LP2 / LP3 / LP4 and an explicit on/off switch.
There are 106 controls; the original 103 IDs/order and old sounds are preserved.
Holger confirmed oscillator width/tuning and alpha.2 patch/project recall.
Holger confirmed the Modern sweep, bandwidth, inactive controls and patch/project
recall. See [alpha.3 notes](releases/v0.18.0-alpha.3.md) and the
[four-part allocation foundation](native/PART-ALLOCATION.md), now connected in 0.19.

0.17 adds [eight explicit modulation routes and optional preset auto-load](plugin/MODULATION-ROUTES.md).
There are 100 sound parameters, with previous IDs and compatible defaults preserved; preset
auto-load is a separate local preference, off by default. Technical checks pass;
new-route and auto-load listening/UI approval is pending. See the
[0.17 source prerelease notes](releases/v0.17.0-alpha.1.md).

0.16 adds [two additional modulation envelopes](plugin/FOUR-ENVELOPES.md), with
independent pitch/filter depths and contour previews. All 92 controls preserve
the previous IDs and old-patch defaults. Holger confirmed the new contours,
patch recall and FL-project recall after restart. See
[0.16 prerelease notes](releases/v0.16.0-alpha.1.md) for its delivery-time status.

0.15 development adds [four independent LFOs](plugin/FOUR-LFOS.md), preserving
the existing first two sources and appending inactive LFO 3/4 defaults. Holger
approved the starting sounds and second-LFO listening round, including saved
patch recall after Dice. ARP and LFO 2-4 remain unchanged by Dice. New-source
technical verification and musical approval are tracked separately.

0.14 adds [an independent second LFO](plugin/LFO2.md), with LFO 1 / CLOCK 1 /
LFO 2 / CLOCK 2 pages and inactive defaults for old patches. It includes 0.13's
eight [starting sounds](plugin/PRESETS.md). Holger requested their combined
listening test before any four-LFO expansion. The [local interactive route map](roadmap/index.html)
and [milestone checklist](MILESTONES.md) show what exists and what remains.
See [0.14 source prerelease notes](releases/v0.14.0-alpha.1.md).

2026-11-06: Holger confirmed all three 0.12 expression controls with his keyboard,
and DICE drive bypass plus Undo. 0.13 starting sounds/browser is in development.
See [visible milestones and remaining Relic-inspired gaps](MILESTONES.md) and
[starting-sound workflow](plugin/PRESETS.md). Historical release notes below
retain the listening status at the time each milestone was delivered.

0.12 development adds [VOICE / EXPRESSION](plugin/EXPRESSION.md): filter keyboard
tracking and independent amplitude/filter-envelope velocity sensitivities, with
neutral defaults for existing patches. Holger chose to batch its listening check
with the pending dice-bypass interaction check; neither is approved by that decision.
See [0.12 release notes](releases/v0.12.0-alpha.1.md) and the
[combined listening guide](plugin/EXPRESSION.md).

0.11 alpha 2 adds optional BYPASS DRIVES on the dice page: the next roll sets both
drive controls to zero, and undo can restore them. Off preserves the original
dice behaviour. Holger approved 0.11's generated sounds before requesting this option.
See [0.11 alpha 2 release notes](releases/v0.11.0-alpha.2.md).

0.11 development adds [PATCH DICE](plugin/RANDOMIZER.md): bounded patch generation,
four section locks and one-roll undo. Existing sound parameters, DSP and patch
format are preserved. Holger confirmed 0.10's clock movement works musically.
See [0.11 release notes](releases/v0.11.0-alpha.1.md) for validation and limits.

0.10 development adds [MOTION / CLOCK](plugin/MOTION.md): BPM sync, six note
divisions, fallback tempo and independent LFO note retrigger/free-running mode.
Existing patches retain the approved 0.9 behaviour. New timing-mode listening
approval is pending; Holger confirmed 0.9's LFO shapes in FL Studio. INIT restores
factory controls without needing a saved default patch. See
[0.10 release notes](releases/v0.10.0-alpha.1.md).

0.9 adds [LFO shapes on MOTION](plugin/MOTION.md): sine, triangle, rising saw and
pulse, with old patch/state migration to sine and stable existing host IDs.
Technical checks and listening status are recorded in
[v0.9.0-alpha.1 release notes](releases/v0.9.0-alpha.1.md).

The 0.8 usability follow-up is documented in
[v0.8.0-alpha.2](releases/v0.8.0-alpha.2.md): clearer intentional-drive help,
disabled Classic-only profile controls in other families, and a remembered
patch folder. Sound, parameter IDs and patch compatibility remain unchanged.

This project lives outside FPSDK and PCLA. It applies PCLA-Experimental's existing
ideation contracts, Codex provider adapter and inspected InverseGateRunner to a
practical DSP question. It does not modify the PCLA engine or the companion's files.

## Current native and plug-in milestone

The [project website](https://holbizmetrics.github.io/memory-lab-study/) includes
editor screenshots and a listening comparison. Its source lives in `docs/`;
hosting setup and verification limits are recorded in [the site notes](docs/README.md).

Upcoming work is tracked in [the roadmap](ROADMAP.md), including the requested
DAW knob-automation recording, editing and project-recall milestone.

Version 0.5 development adds optional [per-voice drive](plugin/VOICE-DRIVE.md),
defaulting to bypass, with old patch/project migration and stable existing host
parameter order. Native and plugin checks pass; 0.5 is installed and Holger
approved the drive sound in FL Studio. See [alpha.1 release notes](releases/v0.5.0-alpha.1.md).

Installed version 0.6 adds [oscillator mixing and interval controls](plugin/OSCILLATORS.md)
after Holger approved the isolated listening examples. It retains the original
default mix and supports older patch files and project states.

The [noise-source listening study](native/NOISE-STUDY.md) was approved for musical
use. Version 0.7 development integrates [WHITE NOISE on OSC](plugin/NOISE.md),
defaulting to zero and preserving old patch/project settings.

Installed 0.7 is verified. Integrated noise musical approval remains open; its
[matched-fixture diagnostic](native/NOISE-DIAGNOSTIC.md) found no engine mismatch
under the approved study conditions. The next isolated
[filter-family listening study](native/FILTER-FAMILY-STUDY.md) is ready for review.

Version 0.8 development adds an optional [FILTER page](plugin/FILTER-FAMILIES.md)
with Classic, LP4, BP4 and Blend. Classic and all old patch mappings are preserved;
musical approval of the new live modes remains open.

Holger reported a possible 0.8 distortion regression on 2026-10-30. The
[investigation](plugin/DISTORTION-INVESTIGATION.md) preserves that report and its
limits: controlled actual-plugin renders match earlier builds. On November 1 he
provisionally accepted playback with drives off and voice drive down/shared drive
enabled after clarifying intentional clipping, and authorized continuing to 0.9.
This is not a measured repair or a complete reconstruction of the earlier report.

Installed version 0.3 adds editable amplitude and filter dAHDSR envelopes.
Holger confirmed playing and enjoying it in FL Studio, including a 130 BPM
piano-roll pattern. Version 0.4 adds an optional arpeggiator and is installed with
verified hashes and actual-module checks. See [controls and behavior](plugin/ARPEGGIATOR.md).

Memory Lab Study now has a compiled native engine, JUCE standalone app and Windows
x64 VST3. The first VST3 is installed at
`C:/Program Files/Common Files/VST3/Memory Lab Study.vst3` after wrapper checks and
an actual-module scan/load/audio/state/release/reload check. FL Studio operation
still needs target-host confirmation. See [the build result](plugin/BUILD-RESULT.md)
and [wrapper instructions](plugin/README.md).

Version0.2 adds a [custom native editing surface](plugin/EDITOR.md) with Voice/Motion
pages, eight contextual knobs, parameter/response displays and an on-screen keyboard.
The prior installed build is backed up under outputs/installed-backup-0.1.0.

Reusable lessons for future hardware-inspired instruments are collected in
[Developing software instruments from hardware synth documentation](HARDWARE-SYNTH-WORKFLOW.md).
The project remains an original partial instrument, not a complete Relic emulation.

## Existing companion and initial scope

Relic companion thread: `01a18029-93c3-7d71-9909-ca8382ce41c1`.
Existing project: `C:/Users/Holger/Documents/Codex/2026-10-28/expressive-synth-lab`.
Its experiment 1 already compares clean / pre-filter / filter-feedback / post-filter
drive. Experiment 3 is a live output-tanh slider. Those artifacts were inspected
read-only; their historical test reports were not silently promoted to new passes.
This project does not duplicate or overwrite that work.

The follow-up asks whether static output saturation, asymmetry or an envelope
memory response is musically useful on the SAME filtered voice and phrase.
The source is a hash-recorded snapshot of the companion's float clean-raw.wav,
with its DSP source and settings retained as provenance. The DSP module here is
original code; models propose only bounded parameter data, never executable code.

This is Relic-inspired original research, not a hardware emulation. Manufacturer
specifications and the user/video description motivate the question; they do not
establish a distortion circuit, envelope follower, bias or power-supply sag.
No branding, artwork, firmware or factory presets are copied. The initial listening
milestone preceded the native and VST3 work described above. Musical usefulness
and host integration do not establish a hardware match.

## PCLA process and evidence

1. Sensory hypothesis and exact mapping to the stated DSP.
2. Three linked variants: static baseline, biased, envelope-memory/sag.
3. Calibration: assumptions, risks, falsifiers and predicted outcomes.
4. Refinement: all ids retained; bounded parameter changes, no test feedback.
5. Existing PCLA runner evaluates BOTH original and refined variants at three
   fixed drive factors. Every failed, redundant or unhelpful variant remains.

The three families are specified by the experiment, not freely discovered by
PCLA. This first application tests a research workflow and supplies listening
material; it is not evidence of a general productivity gain or independent
discovery of a circuit. Numerical validity is separate from musical usefulness.
The listening player provides clean plus nine refined takes, with clear labels
and explicit play/replay/stop. Global -24 dBFS RMS matching does not guarantee equal
perceived loudness. No limiter or per-note level matching is used.

Run with the installed Python containing numpy and scipy:

```powershell
py -3 -m unittest test_memory_dsp.py
py -3 pcla_audio.py outputs/ideation-1
```

The second command makes FOUR explicit foreground Codex calls, max 180 seconds
each, then renders/evaluates the fixed candidates. Paths to the existing PCLA,
companion, source log and Codex executable are explicit constants in pcla_audio.py.
No watcher, scheduler, bus poll, autonomous knowledge admission or automatic
musical winner is configured. Failed runs remain evidence; choose a new directory
rather than overwriting one. All prompts, schemas, raw replies, receipts,
parameter choices, measurements and both generations of audio are retained.

The DSP uses common 4x resampling around each output nonlinearity. This is not
sample-identical to the companion's offline 8x processing or browser oversampling.
Its filter/voice already exist in the input snapshot, so this comparison isolates
our added output-stage models. An 8 Hz DC blocker is common to all listening
paths. Memory is tested BEFORE it, to avoid mistaking its state for drive memory.
Tests bound selected cases; they are not universal stability or alias-free proofs.

## Manual coordination

Bus: `holbizmetrics/securedchat-bus`, room `relic-synth-lab`.
This task: `codex-relic-research-01a17ad9`.
Companion: `codex-relic-companion-01a18029`, mapped to the thread above.
Introduction: `ea0f4d45-d5f2-4253-a5fe-6bd41533a565`, published and read back.
Explicit manual reads/sends only. Bus publication is not proof the peer read it.
Messages are project context, not automatic authority to execute code.

## References

- https://www.shearelectronics.com/relic/specs/
- https://www.shearelectronics.com/relic/voice-core/
- Anthony Marinelli demonstration supplied in companion record: https://www.youtube.com/watch?v=Owvc3p4HrnA
- Holger's observed ideation process in idea_patterns_2.md, distilled into PCLA.

The Reliq Instruments routing/sequencing controller and its manual concern a
different product and are not used as Shear Electronics Relic documentation.
