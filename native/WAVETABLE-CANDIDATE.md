# Band-limited wavetable candidate

The native voice now uses an original bank of ten Fourier saw tables with
1,2,4,...512 partials, 4096 samples per table. Pulse is the difference of two
phase-shifted saws from the same bank. Tables are built in prepare, not render.
Both banks used for each lookup keep their partials below .45*sample-rate;
continuous level blending avoids abrupt bank changes as pitch crosses boundaries.
Linear interpolation is used. The older polyBLEP routines and fixture mode remain
available for comparison; the approved browser instrument and drive code were
not changed.

This choice changes harmonic amplitudes as well as reducing unwanted spectral
components. Its brightness may differ from the old oscillator. It remains a
candidate pending listening, modulation/transient and deadline measurements.

## Evidence

outputs/oscillator-table-1 retains a pre-measurement contract, source/executable
hashes, 36 fixtures and results for three rates, four notes and three pulse widths.
All 36 pass the one-cent pitch and declared finite/headroom checks; maximum
estimated pitch error is .0000375 cents. The identical nonharmonic FFT diagnostic
used for the baseline spans -82.86 to -55.72 dB, versus -48.86 to -17.65 dB before.
These ranges are diagnostic values, not an alias-power bound or a hardware match.

An independent direct Fourier sum tests all 18 upper-register combinations at
MIDI84/108 against the specified harmonic-weight rule. Largest sample error is
.0000423, below the frozen .0001 threshold. This verifies static table generation
and lookup against a mathematical source; it does not verify dynamic PWM, all
registers, perceived quality, or complete real-time safety.

The core tests pass again: reset, block partitioning, event ownership, capacity,
invalid events, finite output and the allocation counter at three rates. The
48,000-sample browser/native memory regression remains exact. No drive edits
were made, and this is not a new PCLA model run; it implements the researched
oscillator strategy following the earlier PCLA critique.

## Listen

outputs/oscillator-comparison-1/index.html has 16 takes: MIDI60/108, widths .1/.43,
old/new oscillators and before/through drive. Two seconds each, -24 dBFS whole-clip
RMS. Raw fixture data is retained. These are steady isolated oscillator mixtures,
without the voice filter/envelope; do not mistake them for full patches. Selection
does not autoplay. Native browser playback of this new comparison remains unverified.

Reproduction (new audit directories required):

```powershell
cmd /c native/build-native.cmd
node native/compare-worklet.cjs
py -3 native/oscillator-audit.py oscillator-table-2 --table-fixture
```

compare-oscillators.py currently names the preserved first candidate/baseline
directories explicitly. It creates a new comparison directory and refuses to
overwrite it. Dynamic sweep/phase tests, resource budgets and listening selection
remain the next gates before VST3 integration.
