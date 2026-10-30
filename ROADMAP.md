# Memory Lab Study roadmap

## Release policy and current installation

2026-10-30 priority: Holger reports possible persistent distortion in 0.8 and is
testing before 0.9. Keep the regression open. [Investigation](plugin/DISTORTION-INVESTIGATION.md)
found no parameter writes from tab switching and byte-identical actual VST3 audio
across 0.4/0.6/0.7/0.8 for three controlled settings. His affected patch/FL chain
still needs reproduction; do not infer musical approval from these checks.

Every verified, delivered numbered synth version receives an annotated source
tag and a published GitHub prerelease. This is a standing user instruction,
recorded in AGENTS.md. Tags point to the exact verified source and are never moved.
Versions 0.6, 0.7 and 0.8 are preserved as v0.6.0-alpha.1, v0.7.0-alpha.1 and
v0.8.0-alpha.1 respectively; their release notes live under releases/.
Installed 0.8 passed actual-module validation, with a verified 0.7 backup retained.

## Preserved milestone

v0.4.0-alpha.1 is the first tagged source prerelease: editable amplitude/filter
envelopes and an optional arpeggiator. Holger confirmed extended musical playing
in FL Studio and values the current sound's perceived analog character.
Preserve existing patches and the approved sound while adding features.

v0.4.0-alpha.2 preserves the subsequent patch save/load development milestone,
automation/accessibility checks and interview evidence as a source prerelease.
The installed plugin remains unchanged. See
[release notes](releases/v0.4.0-alpha.2.md).

## Later milestone: DAW knob automation

Initial work has started: FL Studio recognized Cutoff as last tweaked and played
an edited automation clip with visible knob/curve movement. Source-side parameter
and gesture checks passed. Project reload was followed by an unexpected FL exit
with a UI Automation provider-cleanup failure in the captured dump; provider
ownership and reliable recall remain unverified. See
[the detailed status](plugin/AUTOMATION.md).

Decision on 2026-10-29: park active crash investigation and continue instrument
development. Preserve the dump, analysis and isolated lifecycle regression;
resume triage if the crash recurs. Passing the isolated test does not establish
that FL Studio reload or VST3 module unloading is safe.

Requested by Holger on 2026-10-28: automate knob movements in the DAW to create
evolving sounds and record performances.

The current plug-in already exposes synthesis and arp controls through JUCE's
AudioProcessorValueTreeState, and editor knobs use parameter attachments. This
provides the host-parameter foundation; a complete FL Studio automation workflow
has not yet been verified. Current parameter changes reach the native core at
audio-block boundaries. Sample-accurate parameter automation is not claimed.

Acceptance criteria for the future milestone:

- Record a mouse-driven knob movement in FL Studio and replay it accurately.
- Create/edit host automation for cutoff, resonance, drive, envelope stages and
  arp controls; document a short verified FL Studio workflow.
- Have visible knobs and their response/curve displays follow host automation,
  including when switching editor pages.
- Verify begin/end gestures and undo behavior so recording does not merge
  unrelated knob movements into a single edit.
- Preserve parameter IDs and automation mappings when reopening projects or
  upgrading the plug-in. Confirm project save/reload with automation present.
- Check rapid ramps for zipper noise and bounded output; define smoothing and
  timing behavior per parameter without making discrete modes continuous.
- Keep envelope stage edits latched to the next note, unless a future explicit
  design changes that contract. Distinguish MIDI learn/controller mapping from
  host automation and verify it separately if added.

This is a recorded future task, not a claim that these host checks have passed.
The installed synth remains available for playing while the work is planned.

## Broader instrument direction

Version 0.5 is installed and verified. Holger approved the integrated per-voice
drive sound in FL Studio on 2026-10-29. The SAVE/LOAD and drive milestone is
preserved in v0.5.0-alpha.1; see releases/v0.5.0-alpha.1.md.

Next listening candidate: [independent oscillator levels and oscillator 2
intervals](native/OSCILLATOR-MIX-STUDY.md). Twelve level-matched clips and raw
renders are ready locally. All four native checks and independent WAV checks
pass. This experiment is not compiled into the plugin; listening review precedes
any live-control integration.

Holger approved all oscillator study examples as correct and very clean.
Version 0.6 development integrates separate saw/pulse levels and an oscillator 2
interval, with 20 ms smoothing, stable appended parameter IDs and old patch/state
migration. See plugin/OSCILLATORS.md. The installed 0.5 milestone and its tag
remain preserved while the new build is validated.

Version 0.6 is now installed and its actual installed module passed validation.
Holger approved the integrated instrument during musical playing. The 0.5 backup
and v0.5.0-alpha.1 tag remain preserved. Next listening candidate:
[noise before the filter](native/NOISE-STUDY.md), with six level-matched examples
and independent WAV checks. All five native CTest checks pass. The installed
plugin remains unchanged while this new source is auditioned.

Holger approved both noise-solo fixtures and the other noise examples as clean
and musically useful, and requested live integration. Version 0.7 development
adds WHITE NOISE to OSC, defaulting to zero, with old patch/state migration.
See plugin/NOISE.md. The installed 0.6 baseline is retained until validation and
the guarded update complete.

Version 0.7 is installed and its installed module passed validation. Holger's
integrated noise listening judgment remains open; the matched production noise
diagnostic reproduces all six approved offline examples exactly (see
native/NOISE-DIAGNOSTIC.md). Keep that concern distinct from technical checks.

Next isolated candidate: [filter families](native/FILTER-FAMILY-STUDY.md).
Twelve level-matched examples compare the existing LP2, cascaded LP4, normalized
BP4 and a BP4/LP2 blend, including a shared-drive context. Equation/impulse,
render and independent WAV checks pass. Installed 0.7 is unchanged.

Following the request to continue, 0.8 development integrates optional filter
families with a FILTER page, preserved Classic default, continuously running
filter paths and 20 ms output-weight smoothing. Old patch schemas restore Classic.
Musical approval remains open. The production engine matches all twelve study
contexts within 3.72529e-09 absolute sample error after settling. See
plugin/FILTER-FAMILIES.md for behavior and validation status.

The next sound experiment is an isolated [per-voice post-filter drive study](native/VOICE-DRIVE-STUDY.md).
Six matched listening clips compare bypass/+8/+24 dB on a note and chord. The
original experimental stage was not compiled into the plugin. Holger found all three
settings clear and good, without perceived flattening or unwanted distortion;
no preferred gain setting was specified. Version 0.5 development integrates an optional per-voice drive, defaulting to exact bypass. See plugin/VOICE-DRIVE.md for its signal path and limits.

Development step on 2026-10-29: SAVE/LOAD patch files now preserve all 35 controls
independently of the host project. File validation and roundtrip tests pass, as
do the existing wrapper/automation/accessibility and actual-module checks. The
installed plugin is unchanged; native dialogs in FL still need verification.
See [patch file behavior](plugin/PATCHES.md).

Continue incrementally toward richer oscillator, filter, modulation and allocator
features using the hardware evidence and explicit modeling limits. Earlier
research/implementation ordering is retained in NEXT-VOICE.md and
VOICE-EVIDENCE.md; those documents also contain historical milestone status.
