# Relic interview evidence by generation

Recorded 2026-10-29. Keep historical prototype descriptions separate from later
specifications. A creator's interview is primary evidence of stated design intent
and features, not an independent circuit verification or measured sound match.

## NAMM 2017: Relic-6

Source: [Sound On Sound interview with Jacob Brashears](https://www.youtube.com/watch?v=RT608NU78CM).
Holger supplied the transcript and a screenshot identifying the interview.
The transcript has no timestamps and may contain transcription errors. The web
fetch of the video failed during this review; the claims below were extracted
from the supplied transcript, not from independently listening to its audio.

| Creator's stated feature | Implication for our study | Unresolved detail |
| --- | --- | --- |
| Two VCOs per voice; discrete saw cores described as twelve transistors per oscillator | Separate oscillator controls are a useful next expansion | No schematic, calibration or waveform measurements supplied |
| Saw and pulse; oscillator 2 tuning in semitones and cents | Add independent interval tuning and waveform selection deliberately | Exact waveform switching/combination and tuning ranges not established |
| Oscillator synchronization | A sync mode is a candidate feature | Interview does not specify the reset rule; the supplied secondary article calls it hard sync |
| Oscillator 2 off/soft/loud rather than a continuous mixer | Switched oscillator level is a distinct musical behavior worth studying | The soft/loud gain ratio and oscillator 1 routing are unknown |
| Original Oberheim signal path and 12 dB/oct state-variable low-pass; emphasis on matching components and silver-mica capacitors | Treat the classic low-pass as a reference direction | Component materials alone do not determine a digital transfer function or prove nonlinear behavior |
| Digital modulation, matrix plus expression routes for velocity, wheel and aftertouch | Expression routing deserves explicit controls and contracts | Exact destinations, depth ranges and response curves are not given |
| Simple 64-step polyphonic recording/playback sequencer | Keep sequencing separate from arpeggiation in the feature inventory | Step timing, gates, ties, memory and editing behavior not demonstrated in the transcript |
| Touch-sensitive high-resolution encoders with immediate value feedback | Our contextual parameter display follows a useful interaction principle | Encoder resolution and scan/update rates are not established |

The creator explicitly distinguishes the sequencer from an arpeggiator in this
interview. It therefore does not confirm a native Relic arpeggiator. Memory Lab's
existing arp is our own feature; do not remove it or present it as hardware parity.
Do not rely on the transcript's shipping-price rendering for technical decisions.

## Later official documentation

Reviewed against the [manufacturer's specifications](https://www.shearelectronics.com/relic/specs/)
and [voice-core description](https://www.shearelectronics.com/relic/voice-core/).
Those pages describe eight voices, two SEM-topology VCOs per voice, four dAHDSR
envelopes, four LFOs, and a broader classic/modern filter architecture. The envelope
contours are described as CEM3310-inspired. The voice-core page explicitly places
Relic-6 at NAMM 2017 and describes subsequent oscillator development.

The supplied AMAZONA article combines a 2026 report with older 2022/Relic-6
coverage. Preserve those generation boundaries: a feature mentioned in an older
section is not automatically confirmed for the later instrument. In particular,
do not replace the later official filter architecture with the 2017 low-pass-only
description or assume old voice counts and routing carried forward unchanged.

## Next evidence to collect

Holger expects two further videos, possibly with Anthony Marinelli. When supplied,
record their titles, dates, links and timestamps; distinguish spoken claims from
visible controls and audible demonstrations. Compare oscillator levels/waveforms,
sync, expression routing, envelope contours and sequencing across generations.
Do not choose undocumented gain ratios or envelope curves as hardware facts.

Preserve the current approved sound and patches. Adding controls can be based on
these feature directions; claiming hardware correspondence still requires reference
recordings, declared comparison conditions and measurements.
