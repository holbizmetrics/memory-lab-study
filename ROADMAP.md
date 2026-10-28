# Memory Lab Study roadmap

## Preserved milestone

v0.4.0-alpha.1 is the first tagged source prerelease: editable amplitude/filter
envelopes and an optional arpeggiator. Holger confirmed extended musical playing
in FL Studio and values the current sound's perceived analog character.
Preserve existing patches and the approved sound while adding features.

## Later milestone: DAW knob automation

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

Continue incrementally toward richer oscillator, filter, modulation and allocator
features using the hardware evidence and explicit modeling limits. Earlier
research/implementation ordering is retained in NEXT-VOICE.md and
VOICE-EVIDENCE.md; those documents also contain historical milestone status.
