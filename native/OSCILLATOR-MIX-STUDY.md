# Oscillator mix and interval listening study

The next isolated experiment after v0.5.0-alpha.1 gives the two existing
oscillators independent levels and shifts oscillator 2 by a musical interval.
The installed plugin remains unchanged. Only memorylab_oscillator_mix compiles
MEMORYLAB_EXPERIMENTAL_OSCILLATOR_MIX; this is not a live-control implementation.

Oscillator 1 remains saw, oscillator 2 remains pulse at width .43. Its existing
+7-cent detune is retained in addition to the interval. The six comparisons are
original .42/.28 balance, saw only, pulse only, oscillator 2 an octave down,
a fifth up, and an octave up. Mixer configuration allows levels 0–1 and interval
−24 to +24 semitones, but these listening fixtures cover only the stated cases.
No waveform switching, sync, noise, extra modulation or hardware model is added.

The single E3 and E3/G3/B3 chord fixtures share velocity, amplitude envelope and
a 300 Hz–6 kHz low-pass sweep with Q 1.2. Both drive stages are bypassed to isolate
oscillator relationships; existing output gain/DC filtering remain. Each 2.7 s
listening file is independently matched to −24 dBFS RMS. This does not guarantee
equal perceived loudness. Raw files and applied gains are preserved.

## Validation and listening artifacts

Run outputs/oscillator-mix-study-2 contains index.html, 12 listening WAVs,
12 raw WAVs, measurements.csv and verification.json with file/source/binary hashes.
The earlier study-1 run is retained. Open index.html for labelled audio players;
only one plays at a time, with no autoplay.

All four native CTest checks pass. The new study verifies 36 combinations of six
settings, two note fixtures and 44.1/48/96 kHz: finite output, released voices,
exact 512/127 block agreement, zero output with both sources muted, and rejection
of invalid configuration. An isolated pulse output also has stronger spectral
energy at its expected fundamental than at comparison frequencies ±10 Hz away
for −12/0/+7/+12 semitone settings. This is a bounded interval check, not a full
pitch, tuning or aliasing audit across the keyboard.

The separate PowerShell/C# verifier independently decodes all 24 float WAVs,
checks format/length/finiteness, and verifies the 12 listening files are within
.001 dB of the RMS target with peak below .95. It verifies audio links and records
hashes. Browser playback itself has not been exercised by automation.

The standard native default phrase hash is unchanged:
B3BB94ED8CB9A7618498FBA2AFC6DB363BFC3BE4F30C24F087FA58105D309AD5.
The installed 0.5 VST3 hash is also unchanged:
2195E217D931F5F26ECA67DA3DDAD2746F69E6D7AA3DD384CE30F89A7512056A.

Listen before integrating. A plugin version will need smoothed live levels and
an explicit interval-change policy, appended stable host parameters, patch/state
migration, editor controls and separate wrapper/host checks.
