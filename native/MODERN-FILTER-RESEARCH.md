# Modern filter groundwork

The independent study is now connected as the opt-in 0.18 alpha.3 modern
filter. The legacy FilterFamily path remains unchanged.
See [plugin integration](../plugin/MODERN-FILTER.md).

The manufacturer's [specifications](https://www.shearelectronics.com/relic/specs/)
describe a continuous BP4 / XF4 / LP2 / LP3 / LP4 profile, separate bandwidth
and resonance, and XF4 as a half BP4 / half LP2 blend. The
[voice-core description](https://www.shearelectronics.com/relic/voice-core/)
describes an SVF plus an extra dipole. Checked 2026-11-09. These statements do
not establish circuit coefficients, gain calibration, bandwidth law or sweep
interpolation. The following choices are ours, pending measurements/listening.

Prototype choices:

- Five equally spaced profile anchors, linearly mixed with shared running
  states; no filter reset when profile changes.
- LP2 uses a resonant TPT state-variable section. LP3 adds one TPT pole;
  LP4 adds a fixed Butterworth-damped second section. This avoids the legacy
  LP4 cascade's Q-squared center gain. It does not automatically equalize
  broadband RMS or make high resonance quiet.
- BP4 uses a high-pass section below center and low-pass section above center,
  with corner separation from 0.25 to 4 octaves. Both sections use sqrt(Q).
  Bandwidth and resonance can therefore interact. Variable Q is not replaced
  by a bandwidth knob.
- Bandwidth affects BP4 and consequently XF4 and nearby sweep positions;
  it leaves LP2/LP3/LP4 unchanged. The hardware's full bandwidth law remains
  unknown. Corner limits can shift effective center near sample-rate bounds.
- XF4 is the complex signal sum, not a magnitude-only crossfade. Cancellation
  between paths is possible and must be checked musically.

Validation: the separate `modern_filter_research` executable compares impulse
DFTs against an independent complex transfer reference at 44.1/48/96/192 kHz,
Q 0.5/2.4/3.72/8, and three bandwidths. It checks anchor interpolation,
bandwidth isolation, 12/18/24 dB/oct low-pass slopes, high-Q center gain,
one million samples of unsmoothed extreme parameter changes per sample rate,
and zero residual after reset. This is filter-level evidence, not a full synth
headroom, aliasing, realtime CPU, patch recall or musical approval claim.

Integration appends three IDs without widening or repurposing the legacy
family parameter. Old patches migrate to exact bypass. Controls smooth over
20 ms; the static response uses the same topology and is independently tested.
Legacy family captures match alpha.2 byte-for-byte. Musical approval and
hardware coefficient/gain matching remain open. Local installation is tracked
separately from source publication.
