# Noise before the filter: isolated listening study

The next experiment after the approved 0.6 oscillator controls adds pseudorandom
white noise to each voice before its existing amplitude envelope and classic
filter. This is an original noise source, not a model of a hardware noise circuit.
Only memorylab_noise compiles MEMORYLAB_EXPERIMENTAL_NOISE; the installed plugin
does not contain the experiment and is unchanged.

Each active voice uses a nonzero xorshift32 state, seeded deterministically from
note-on order, note and MIDI channel. Voices have separate streams rather than
sharing one identical noise sample. Resetting the synth and replaying the same
events reproduces the same output. The source maps uint32 output approximately
to −1…+1 and multiplies by a fixed study level before the amplitude envelope.
This fixed configuration has no live control/smoothing contract yet.

Two fixtures compare oscillators alone, oscillators plus noise at level .08,
and noise alone at level .35. The oscillator mix retains .42 saw/.28 pulse with
+7-cent detune. A classic low-pass sweeps 120 Hz–6 kHz at Q 2.72. Both drive
stages are bypassed; existing output gain/DC filtering remain.

The percussive fixture repeats E3 five times with a 2 ms attack, 160 ms decay,
zero sustain and 120 ms release. The held fixture plays E3/G3/B3, with an 80 ms
attack, .394 sustain and 300 ms release. Compare settings within each fixture.
The noise-only chord represents three filtered noise voices, not pitched noise.

Run outputs/noise-study-1 contains index.html, six listening WAVs, six raw WAVs,
measurements.csv and verification.json. Each listening file is independently
matched over its three seconds to −24 dBFS RMS. Raw renders and gains remain
available; RMS matching does not establish equal perceived loudness. The page
has six labelled audio players, no autoplay, and pauses other players on playback.
Browser playback has not been exercised by automation.

All five native CTest checks pass. The noise test covers 18 combinations of
three settings, two fixtures and 44.1/48/96 kHz: finite output, released voices,
exact 512/127 block agreement, rejection of invalid levels without changing
configuration, and enabled modes differing from the no-noise baseline.
The separate PowerShell/C# verifier decodes all 12 saved float WAVs, checks their
format/length/finiteness and verifies six listening files within .001 dB of the
RMS target and peak below .95, player links and hashes. These checks do not
establish a measured noise spectrum, hardware resemblance or realtime deadlines.

The default native phrase hash remains unchanged:
B3BB94ED8CB9A7618498FBA2AFC6DB363BFC3BE4F30C24F087FA58105D309AD5.
Installed 0.6 VST3 hash remains:
771008429B9463B747529312BC98207753E4AA38A13DB75FE46246825F906342.
Listening review precedes integration, which would need a smoothed default-zero
level, preserved parameter IDs, patch/state migration and plugin/host checks.
