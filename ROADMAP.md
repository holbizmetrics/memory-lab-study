# Memory Lab Study roadmap

See [the current milestone checklist](MILESTONES.md) for synthesis coverage,
musical approval and the proposed 0.13-0.20 sequence. Historical entries below
are a development log, not a statement that every old limitation remains current.

## Listening approval and next milestone (2026-11-06)

Holger confirmed 0.12 AMP VELOCITY with his MIDI keyboard, then FILTER ENV VEL
and KEY TRACK in guided comparisons. He also confirmed DICE BYPASS DRIVES sets
both drives to zero and UNDO restores the pre-roll sound and drive values.
This closes that combined listening/interaction round, not the separate full
FL automation/undo/project-reload or saved-file migration verification.

0.13 in progress: eight original starting sounds with a category/sound browser,
explicit LOAD and one-step UNDO LOAD. Browsing is passive, output is preserved,
and starting sounds bypass both drives. No DSP, host-ID or schema change.
Holger wants progress toward the complete Relic-inspired instrument; preset
browsing is this bounded milestone, not completion of the hardware architecture.
Multiple independent LFOs/envelopes, wider modulation routing and multitimbral
voice organisation remain substantial separate work. Do not call this a faithful
Relic conversion or measured hardware match. Older entries below retain their
historical pending status; the confirmation above supersedes that status.

## Current 0.12 work (2026-11-03)

Holger explicitly authorized continuing before testing 0.11 alpha 2, intending
to listen to the changes together. Keep its bypass-checkbox check pending.
0.12 follows the planned expression milestone: filter note tracking around C4,
independent amp/filter-envelope velocity sensitivities, neutral defaults and
legacy migration. A VOICE/EXPRESSION subpage provides the new controls and
response previews. Dice preserves expression settings. See plugin/EXPRESSION.md
for the combined listening recipe and exact behaviour. Musical approval pending.
All nine plugin checks pass. Actual default and migrated clocked-patch renders
are byte-identical to prior captures; the expression UI was visually checked.

## Instrument project list (2026-11-02)

2026-11-03: Holger reports loving the installed 0.11 dice results and approves them
for now. He requested an extra drive-bypass option. 0.11 alpha 2 adds BYPASS DRIVES,
off by default, applying only on the next roll; undo restores the prior drives.
The new checkbox still needs its own live listening/interaction confirmation.

Finish Memory Lab before starting another instrument. Current list:

1. Memory Lab Study — active Relic-inspired hardware-to-VST3 experiment; not a
   verified faithful Relic emulation. Holger approved 0.11 dice; its new alpha-2
   bypass checkbox is technically verified and awaits live confirmation.
2. Wordless hybrid vocal/choir synth — blend a directly controlled formant layer
   with a physically modeled vocal source/tract and breath. One project with two
   approaches, not two separate instruments.
3. Genki Instruments Katla-inspired VST3 — previously recorded future idea.
4. Con Brio ADS 200 recreation — possible future research/build, not committed.
5. Technos 16pi / 16π recreation — possible future research/build, not committed.

### Con Brio ADS 200 and Technos 16pi (2026-11-02)

Holger explicitly requested retaining both as possible later instrument projects.
Con Brio's additive/FM/phase-modulation architecture and segmented envelopes are
the research interest. The 16pi's additive/resynthesis system and touch-console
workflow are the research interest; its inventor reports missing original
first-version hardware, making faithful reconstruction particularly uncertain.
Do not confuse 16pi with the later Acxel, for which a software recreation was
found during research. Search results do not prove the absence of other emulations.

Sources for later investigation:
- Con Brio original brochure: https://www.preservationsound.com/wp-content/uploads/2013/05/ConBrio_ADS_200.pdf
- Technos inventor's history: https://idarca.com/pages/acxel-en_hist1.html
- 16pi hardware history: https://idarca.com/pages/acxel-en_hist2b.html

Implementation strategy is undecided. Record the distinction discussed with
Holger: executing original firmware is different from independently implementing
behaviour. A formal clean-room project needs controlled analysis/specification
and implementation separation with records; merely using different AI assistants
does not establish that process. No firmware permission, rights clearance, exact
emulation feasibility or market demand has been established. Assess those before
committing to either recreation or distributing protected material. Current
Memory Lab work is independently written and hardware-inspired, not a documented
formal clean-room reconstruction.

## Requested extension: patch randomizer (2026-11-01)

0.11 implementation: DICE page with four section locks, protected output/drive/
arp/fallback BPM settings and one-roll undo invalidated by subsequent sound edits.
Per-instance UI session state only; no new sound parameters or patch schema.
All eight plugin checks pass; actual default/clocked audio is unchanged, and
generated-patch examples rendered through the VST3. Holger subsequently approved
the dice sounds; the alpha-2 checkbox remains to be checked live.
0.10 clock received Holger's musical confirmation; he also reports saving a
clocked patch. Live recall of that particular file was not independently checked.

Holger also requested INIT to restore the factory sound without needing a saved
default patch. Include that small usability addition in 0.10. It restores sound
parameters to their factory defaults and leaves the remembered patch folder alone.

Holger proposed a dice/randomizer for generating presets. Record it for after
0.10's LFO timing work, rather than expanding the current milestone. Proposed
first scope: bounded musical variation of oscillators, filters, envelopes and
motion, with output and intentional distortion controlled; optional section locks
and undo/restore of the previous patch. Keep it explicit and opt-in, preserve saved
patches/default sound, and distinguish it from hardware Relic equivalence. Exact
controls and implementation remain to be decided.

## Future project note (2026-10-31)

### Hybrid physically modeled choir (2026-11-01)

Holger proposed a future choir instrument with two blendable voice-generation
approaches: a directly controlled formant layer and a physical vocal model
(voiced excitation feeding a simplified vocal tract, with turbulent breath).
Both should offer vowel/tract shaping and expressive controls. Physical vocal
models naturally produce formants too; these are alternative implementations,
not a division between having formants and having none.

Holger confirmed the initial scope on 2026-11-01: wordless choir only, with
shapable vowels, evolving choir pads and breathy textures. Lyrics and intelligible
words are outside this first scope. Build
one expressive voice first, then add a small ensemble with individual pitch,
timing, vibrato, vowel and breath variation. Consonants, lyrics and intelligible
singing are separate, substantially harder milestones. No acoustic fidelity or
specific implementation has been established. Keep this as a future project;
finish the current Memory Lab work before starting implementation.

### Genki Instruments Katla

Holger requested recording Genki Instruments Katla as a future VST3 instrument
project/inspiration (video reference: "Genki Instruments Katla #2 | Winterdagen").
Research, implementation scope and any hardware-modeling claims are undecided.
Stay focused on completing the current Memory Lab project before starting it.

## Release policy and current installation

0.10 is implemented and technically verified: MOTION/CLOCK adds BPM sync and
retrigger/free-run, and INIT restores factory controls. Seven plugin checks pass;
actual default/legacy modulation parity and clock comparisons pass. Holger has
since confirmed the CLOCK effect works musically and reports saving a patch.
Holger also approved 0.9's shapes in FL on November 1.
See releases/v0.10.0-alpha.1.md for precise limits and validation scope.

2026-11-01: Holger provisionally accepts the current sound with drives off, and
also accepted voice drive down with shared drive enabled. Its intentional
clipping purpose was unclear. No DSP repair is claimed. Finish the 0.8 usability
update: explicit drive help, disabled/dimmed Classic Profile outside Classic,
and persistent successful patch-folder preference. Preserve sound/defaults.

0.9 scope (implemented and technically verified, 2026-11-01): extend the existing per-voice LFO
with triangle, saw and pulse waveforms alongside sine, update the waveform preview
and append a stable host control with old patches restoring sine. Six plugin tests
pass, with exact default and active sine-modulation VST3 audio parity against 0.8.
Holger subsequently confirmed the new shapes work musically. Waveform changes are
discrete and may cause intentional modulation jumps on held notes.
This is an incremental motion feature, not the full four-LFO Relic modulation system.

Tentative sequence discussed with Holger, not promised or frozen: 0.10 adds LFO
tempo sync/retrigger options; the proposed multiple-LFO/modulation expansion was
deferred when 0.11 became the requested patch-dice milestone;
0.12 expression/keyboard tracking; 0.13 starting patches and browsing; later 0.x
closes agreed gaps and performs compatibility/CPU/DAW validation. 1.0 is the
stable, documented, musically approved completion point. 1.x refines it; a possible
2.0 layering/splits/multitimbral expansion is a separate scope decision. Compare
hardware documentation before claiming equivalence or committing those features.

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
0.8 alpha 2 was installed and hash-verified on November 1, with the previous 0.8
bundle retained as a verified backup. 0.9 development follows that installation.

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
