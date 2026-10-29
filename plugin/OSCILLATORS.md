# Oscillator controls — 0.6 development

Open OSC to set the two source levels independently. OSC 1 / SAW and OSC 2 / PULSE
range from 0 to 1 before the existing amplitude envelope and filter. Zero mutes
the source. Defaults .42/.28 retain the original mix. Raising both can increase
level into the filter and drive stages; use OUTPUT to balance the result.

OSC 2 INTERVAL shifts the pulse oscillator from −24 to +24 semitones in whole
semitone steps. OSC 2 DETUNE adds the existing fine offset, default +7 cents.
Oscillator 1 retains MIDI pitch/bend/vibrato, while oscillator 2 follows that
pitch plus its interval and detune. Phase is not reset when changing these
controls. Source levels and interval targets smooth over approximately 20 ms;
interval edits therefore make a short glide on held notes. Pulse width and pulse
motion retain their existing behavior. These controls do not add hard sync,
waveform selection, noise or independently routed modulation.

The three new parameter IDs sawLevel, pulseLevel and osc2Interval append after
the existing 36 host parameters. Older project states restore the original mix
and zero interval. Schema-1 and schema-2 patches also migrate to that original
balance; schema-2 preserves its saved voice-drive setting. New patch files use
schema 3 and contain all 39 controls, requiring this version or later.

Holger auditioned every note/chord example in oscillator-mix-study-2 and described
each as correct, as labelled and very clean. This approves those examples, not a
claim of measured hardware fidelity or every possible parameter combination.
Version 0.6 is installed, with a verified backup of 0.5 retained locally.
The v0.5.0-alpha.1 tag remains unchanged.

Verification on 2026-10-29: native CTest 4/4, plugin CTest 4/4 and actual VST3
validation passed. This includes held eight-note edits at four sample rates,
mute/unmute, all-sound-off, exact automated block partition parity, host state
recall, stable parameter order and both old patch schemas. Normal and compact
editor snapshots were inspected. The default native phrase remains byte-identical
to the approved baseline. Full FL automation recording/undo/reload remains a
separate, unverified host milestone. The installed VST3 also passed actual-module
validation. Holger approved the integrated instrument during musical playing.
