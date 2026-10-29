# Drive Memory Lab — standalone PCLA-assisted audio investigation

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
