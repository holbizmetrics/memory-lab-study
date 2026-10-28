# Classic filter profiles

Implemented an original continuous HP/notch/LP profile on the existing trapezoidal
SVF. Profile0 = HP, .5 = HP+LP notch, 1 = LP. Piecewise linear weights keep the
same filter state throughout the blend. The target profile uses the same 20 ms
parameter smoothing as cutoff/resonance. Default profile remains LP.

This follows the documented classic profile direction, not a claim of reproducing
the hardware circuit or its exact interpolation. The current filter does not add
an independent bandwidth dimension or modern BP4/XF4/LP3/LP4 architecture.
Sources:
- https://www.shearelectronics.com/relic/specs/
- https://cytomic.com/files/dsp/SvfLinearTrapezoidalSin.pdf

## Response verification

outputs/filter-audit-1 holds the frozen contract, source/executable hashes, raw
double impulse responses and report. Independent SciPy bilinear-transform targets
start from the analog denominator s²+(w0/Q)s+w0² with prewarped w0. Numerator weights
define the same declared HP/notch/LP mathematical targets without using the native
state equations. Five profiles x four cutoffs x three Q values x three rates = 180
cases. Two-second impulse responses match the independent reference with maximum
sample error 1.92e-13 and maximum complex frequency-response error 2.00e-9, within
the preregistered limits 2e-8 and 2e-6. Absolute complex error handles deep notches
without misleading logarithmic ratios. Grid cutoffs:30/300/1450/12000 Hz;
Q:.5/2.4/8; rates:44.1/48/96 kHz. This is bounded grid evidence, not universal proof.

## Automation and listening artifacts

outputs/filter-profiles-1 references that completed static audit by hash and
checks that the filter implementation has not changed. A harness-only correction
makes the sweep pre-roll at HP rather than briefly starting at LP. The initial
audit's clips remain retained; use filter-profiles-1 for the corrected listening
comparison. Static response tests were not needlessly repeated for the clip fix.

Three 3-second cutoff/Q/profile stress traces with 20 ms smoothing stayed finite,
peak .202 or less for the declared .2-bounded two-tone input. Actual full-voice
profile events match one-block output exactly at block sizes17/127/512 at all
three rates, with no C++ new allocations detected during the tested render and no
stuck notes. Default core tests pass again; the separate memory-port regression
was still exact after filter extraction. No oscillator or memory-stage edits.

Four 6.8-second WAVs use the identical seven-note phrase, waveform settings,
cutoff1450 Hz, Q2.4 and approved memory drive: HP, notch, LP and profile sweep.
Each complete clip is globally matched to -24 dBFS RMS; decoded float WAVs were
checked within .001 dB and with peak headroom. Relative note dynamics remain.
The sweep traverses HP to notch to LP in six seconds, with 30 ms target steps
and 20 ms smoothing; it does not retrigger additional notes. Its initial 100 ms
pre-roll is trimmed from listening clips. LP raw output matches the ordinary
default-patch native phrase bit for bit.

Browser playback and musical approval of these new filter clips remain pending.
No automation test by itself proves absence of perceived clicks. Open
outputs/filter-profiles-1/index.html; all four audio controls have metadata preload,
and nothing autoplays. The original browser instrument remains unchanged.

Holger confirmed the previous oscillator motion clips matched the intended
control changes during one held note. That approval concerns the motion checkpoint,
not these new filter clips. Preserve this distinction in later reports.

## Reproduce

```powershell
cmd /c native/build-native.cmd
outputs/native-build/memorylab_native.exe --profile-test
```

filter-audit.py and filter-listening.py create fixed new output directories and
refuse to overwrite existing runs. Choose a new run name before another full
audit. Next decisions should use listening feedback and performance measurements
before extending to modern profiles, more modulation or VST3 integration.
