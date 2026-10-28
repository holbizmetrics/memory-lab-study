# Oscillator audit 2 — characterization checkpoint

The actual compiled saw/pulse routines used by the voice were exposed to an
offline fixture harness. The extraction into shared functions preserves their
equations. The existing core checks and browser/native drive regression passed
again after this change; memory output error remains zero for its fixture.

Contract frozen before measurement in outputs/oscillator-audit-2/contract.json:
44.1/48/96 kHz, MIDI 36/60/84/108, pulse widths .1/.43/.9, detune seven cents,
two seconds per case. Executable and source hashes are recorded. The original
attempt, outputs/oscillator-audit-1, failed because a missing string header
prevented compilation; its partial contract is retained and is not a successful
run. Audit 2 ran only after the rebuild and regression checks passed.

## Results

- All 36 cases meet the declared finite/headroom bounds and one-cent pitch
  tolerance. Largest estimated pitch error: 0.0000358 cents. This is an offline
  estimator result, not device-clock accuracy or a universal tuning guarantee.
- Nonharmonic spectral energy diagnostics span approximately -48.86 to -17.65 dB
  relative to total windowed oscillator power. Worst cases are high-register,
  narrow/wide pulses (MIDI108, 48 kHz). The current oscillator is not approved
  against a stringent low-alias target.
- Before/after shared drive RMS is retained for every case. Six 48 kHz/MIDI60
  clips (three pulse widths, source/driven) are globally matched to -24 dBFS RMS
  for later listening. They are isolated oscillator mixtures, without the voice
  filter/envelope; they are not comparisons of complete patches.

The spectral diagnostic excludes +/-6 Hz around intended harmonics and frequencies
below 10 Hz from a one-second Hann-window FFT. It can include leakage and can hide
folded aliases coincident with harmonic lines. It is NOT total alias power, does
not support an alias-free claim, and cannot be used as evidence of Relic similarity.
The pitch estimator searches locally around the intended fundamental; it does not
independently identify arbitrary pitches or exclude every spurious component.

## Decision

Keep these results and the baseline implementation. Improve the high-register
source before expanding the instrument or packaging it. Next compare the actual
native source against an independently synthesized band-limited harmonic reference,
with converged analysis and declared error/CPU targets. Candidate strategies are
band-limited tables or better oversampling; select using spectrum, transient,
resource and listening evidence, preserving approved drive behavior.

Re-run into a new directory; existing outputs are never overwritten:

```powershell
py -3 native/oscillator-audit.py oscillator-audit-3
```

Full measurements and raw fixtures: outputs/oscillator-audit-2/report.json and
the neighboring float files. Oscillator improvements, phase-retrigger transient
acceptance, rigorous alias bounds, listening approval and JUCE integration remain
pending. No full-instrument completion is claimed.
