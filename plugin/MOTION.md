# Motion waveforms — 0.9

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
