# Patch dice — 0.11

DICE below the output section opens a patch generator. ROLL NEW SOUND varies
unlocked sections; UNDO ROLL restores the whole patch from immediately before the
last roll. SAVE a result as a normal `.mlpatch`. No automatic files are created.

Four independent locks preserve the parameters in these sections:

- OSC: saw/pulse/noise mix, pulse width, detune and oscillator-2 interval.
- FILTER: filter family, cutoff, resonance and Classic profile.
- ENVELOPES: all amplitude/filter envelope stages and filter-envelope amount.
- MOTION: waveform, Hz rate, vibrato/PWM/cutoff/pressure depths, sync, division and
  LFO retrigger. Fallback BPM is always preserved.

Output, memory depth/recovery, all arpeggiator settings, oscillator phase reset
and fallback BPM never change in a roll. Both drives are preserved by default.
The optional BYPASS DRIVES checkbox sets VOICE DRIVE and SHARED DRIVE to zero
on the next roll. Checking it alone changes no sound controls. Undo restores
the whole previous patch, including its drives. This bypasses the two intentional
drive stages; it is not a limiter or a guarantee against clipping elsewhere.
Locks protect
base controls: other unlocked modulation can still affect a locked oscillator
or filter audibly. Lock those modulation sections as well if desired.

The generator uses constrained ranges: positive source levels, subtle noise,
moderate resonance, selected musical oscillator intervals, nonzero amplitude
sustain, and bounded envelope/modulation ranges. It is a source of starting
patches, not a guarantee that every roll will be pleasing, equally loud, or
audible with arbitrary locked controls. Preserving OUTPUT does not normalize
loudness or guarantee headroom for arbitrary output/drive/locked settings.
Release held notes and play again after a roll: existing envelopes remain latched.

Locks, the bypass checkbox and one-roll undo live in the instrument instance and survive editor
close/reopen. They are not host sound parameters and are not saved into patches
or projects. A new instance starts unlocked. Only the latest roll is undoable;
there is no multi-step history or redo. Changing sound controls after a roll
invalidates undo, so it cannot overwrite those later edits. INIT, load or host
automation changes can invalidate it too. Unchanged settings do not invalidate it.
If every section is locked, rolling is disabled unless BYPASS DRIVES would change
an active drive. In that case a roll changes only those drives. Once both are
zero, another all-locked roll is a no-op and retains the prior history.
Undo restores the entire prior patch regardless of current section locks.

This is a message-thread action notifying the host of changed parameters, like
patch loading. Stop conflicting automation before rolling; host automation may
override generated settings. DAW undo grouping has not been verified.

The 0.11 dice milestone changed no DSP, default sound, 46 host IDs/order or
schema-7 patch format. In 0.12 the three new expression parameters are always
preserved by dice rolls; current patch format is schema 8 with 49 controls.
The random seed is an internal generator input for reproducible development
checks; it is not a saved sound parameter. Generated parameter values are saved
normally. This is a software extension, not a hardware Relic modelling claim.
