# Motion — waveforms and clock

## 0.10 clock controls

MOTION has MODULATION and CLOCK subpages. CLOCK exposes TEMPO SYNC, DIVISION,
NOTE RETRIGGER, FALLBACK BPM, FREE RATE, SHAPE, FILTER MOTION and OUTPUT.
Sync off retains the original smoothed 0.05–20 Hz rate. Sync on uses one cycle per
quarter, eighth, sixteenth, 32nd, eighth-triplet or sixteenth-triplet note. Valid
host BPM (20–400) takes priority; otherwise the independent fallback BPM is used.
The arpeggiator's tempo controls do not configure the LFO. FREE RATE is dimmed
while synced, and DIVISION is dimmed while unsynced. The preview reports effective
rate, BPM source and retrigger mode. Sync controls speed, not host PPQ/bar alignment.

NOTE RETRIGGER on retains per-note phase-zero starts. Off makes all voices use one
shared continuously advancing phase, including through silence while the host is
processing audio. This phase resets on engine prepare/reset, is not saved as part
of a patch, and does not follow transport seeks or restart on host play/stop.
Switching sync/retrigger/shape on held notes may cause modulation jumps. Oscillator
phase reset remains independent. Host tempo updates take effect at block boundaries;
synced speed changes immediately, while the old unsynced rate smoothing is retained.

Four new host parameters append after the original 42: `lfoSync`, `lfoDivision`,
`lfoRetrigger`, `lfoBpm` (indices 42–45). Patch schema 7 stores all 46 controls.
Schemas 1–6 and old host states restore sync off, sixteenth division, retrigger on
and fallback 130 BPM; saved free rate and LFO shape remain as before. Old builds
cannot read schema 7. Musical approval of 0.10's new timing modes is pending.

## 0.9 waveform milestone

Holger confirmed the new shapes work and sound cool in FL on November 1, 2026.
The following describes the original 0.9 milestone; 0.10 adds the clock options above.

MOTION now has an LFO SHAPE knob: SINE, TRIANGLE, SAW and PULSE. It replaces
the duplicated MEMORY depth knob on that page; memory depth remains on VOICE,
AMPLITUDE and DRIVE. RECOVERY on MOTION still belongs to shared drive memory,
not to the LFO.

There is still one LFO per voice, restarting at phase zero on each note. Its
selected shape drives the existing VIBRATO, PULSE MOTION and FILTER MOTION routes.
Depths of zero disable those routes. RATE remains 0.05–20 Hz; this version adds
neither tempo sync nor free-running operation. The preview covers one second.

All waveforms are bipolar. Sine retains its original calculation and phase.
Triangle starts at -1, rises to +1 halfway through a cycle, then returns to -1.
Saw rises from -1 towards +1 and jumps down at the cycle boundary. Pulse is +1
for the first half and -1 for the second half. Saw/pulse jumps are intentional:
at larger depths they can produce abrupt pitch, timbre or cutoff changes.
Changing the shape of a held note takes effect without restarting its phase;
switching shapes can itself create a modulation jump. No slew or crossfade is
claimed. This is modulation, not a change to the audio oscillator waveforms.

The existing 41 host IDs and indices are retained. LFO shape appends at index 41
as `lfoShape` (0 sine, 1 triangle, 2 saw, 3 pulse), for 42 controls in total.
New patch files use schema 6; schemas 1–5 and old host states restore sine even
if a different shape had been selected in the current instance. New files need
0.9 or later. The default sound remains unchanged.

This is an incremental Relic-inspired instrument feature, not a claim that its
modulation architecture or sound now duplicates the hardware Relic.
