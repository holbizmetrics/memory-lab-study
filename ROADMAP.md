# Memory Lab Study roadmap

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
