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

## Later Marinelli / Brashears session: supplied transcript

Holger supplied a further interview transcript on 2026-10-29, with repeated copies
of the same passage. Treat those as one source, not independent corroboration.
Video URL, title, publication date and timestamps are still pending. These are
speaker claims extracted from text; the audio and demonstrations have not been
independently reviewed here. Do not date the video from its reference to October.

Important additions and changes relative to the early prototype:

- Oscillator mixing: the creator describes VCO crossfading, adjustable gain staging
  and white noise before the filter. This supersedes treating the 2017 off/soft/loud
  description as a universal constraint on later Relic designs.
- Voice path: oscillators/noise feed the filter, then the output VCA. He attributes
  much of the demonstrated distortion to the output VCA on each voice, following
  distortion in the filter itself. No equations, drive limits or time constants
  are established. The amplifier-feedback comparison is a listening analogy,
  not evidence of a literal feedback topology or an envelope-memory mechanism.
- Output architecture: each voice has stereo output VCAs; the system is described
  as having four outputs, grouping and voice panning. Our present stereo output
  duplicates a mono signal and does not implement that architecture.
- Unison: four-voice and eight-voice unison are described, with two VCOs per voice.
  Their preference for four voices is a musical judgment, not a universal rule
  that larger unison stacks sound worse. Detune, pan, allocation and normalization
  rules are not specified.
- Envelope timing: the creator states a 100-microsecond to 100-second range for
  envelope times (0.1 ms to 100,000 ms), and explains audible clicks at extremely
  short decay settings. Sustain remains a level. This establishes stated range
  and intended sharpness, not a measured minimum or exact curve/retrigger contract.
  Our current UI uses integer milliseconds with shorter maximum times; broadening
  that range requires parameter-mapping and patch/automation compatibility work.
- Modulation: resonance and filter profile are described as destinations, including
  envelope/expression control. Four digital envelopes and four LFOs per voice are
  reiterated. The abbreviated spoken waveform list does not establish removal of
  pulse shapes listed in the official specifications.
- Expression: individual-key bends are discussed. This motivates studying note
  expression, but does not by itself establish a complete MPE protocol contract.
  Our pitch bend and pressure are currently channel based.
- Filter: the continuous classic and modern profile sweeps, variable bandpass
  bandwidth and midpoint X blend agree with the official feature description.
  The transcript supplies no exact sweep law, filter coefficients or nonlinear
  circuit model. Descriptive synth comparisons are not evidence of shared circuitry.

Concrete implementation difference to preserve in future experiments: our current
voice applies its amplitude envelope before the filter and the memory drive after
voices are summed. The described later hardware puts an output VCA after each
voice filter. Per-voice post-filter saturation and shared post-sum memory drive
are different experiments, especially with chords. Keep the approved existing
path as the default; compare a new optional architecture with matched levels and
single-note/chord fixtures before deciding whether it is musically useful.

The strongest next sound experiments are independent oscillator mixing/gain,
optional per-voice post-filter drive, resonance/profile modulation and wider
envelope timing. This is an evidence-backed candidate list, not an instruction to
change all modules simultaneously or a claim of a hardware match.

### NAMM 2024 encoder short: screenshot and supplied transcript

Holger supplied a screenshot from Anthony Marinelli's channel and a short
transcript identifying NAMM 2024. The visible URL is
https://www.youtube.com/shorts/wZbcKiuWqjY . The title is truncated in the screenshot;
publication date and full-video timestamps are not established here.

The speakers demonstrate selecting an attack time of 62 ms, contrasting precise
selection with a 20 ms example. They describe the display returning to its overall
value indication when the finger leaves the encoder. This supports touch-dependent
contextual display and precise time editing as interface goals. It does not
establish encoder bit depth, uniform 1 ms spacing across the whole range, or
measured envelope timing accuracy. Our typed values already allow integer-ms
selection; sub-ms support and hardware-style touch/release display remain separate.

The screenshot visibly includes an ARP GATE % panel label as well as sequencer
labels. This is direct visual evidence of an arp-related control on the shown
prototype. It narrows the earlier uncertainty: the 2017 interview's sequencer-only
description must not be generalized to all later Relic versions. A printed label
does not establish working arp modes, tempo/transport behavior or shipping support.


### Filter-profile short: linked supplied transcript

Source: https://www.youtube.com/shorts/Ty1hH-6b4Qw . Holger supplied the link and
transcript on 2026-10-29. Direct web fetching failed; audio, date and title were
not independently verified. Treat likely transcription errors in the introductory
words and manufacturer name as transcription errors, not technical terminology.

The speaker contrasts the classic low-pass/high-pass blend with an expanded
profile sweep between two-pole low-pass and four-pole bandpass, then describes
returning to two-pole low-pass and continuing further. He explicitly states that
filter profile is voltage controllable. This is consistent with the later full
interview and official specifications. The excerpt does not name the endpoint
beyond low-pass, so LP3/LP4 still rely on those other sources, not this excerpt.

Implementation implication: a future modern filter needs continuously variable
profile, including intermediate responses, rather than just a mode selector or
a steeper low-pass. The current classic HP/notch/LP sweep remains useful, but
does not cover BP4 or the expanded architecture. The transcript establishes no
blend coefficients, sweep law, resonance normalization or bandwidth mapping.
Sources from the same demonstration/session may overlap; do not count clips as
independent measurements or assume that phase/gain behavior is established.

## Next evidence to collect

### Fuller NAMM 2024 transcript supplied subsequently

Holger subsequently provided the fuller interview that appears to contain the
encoder and filter shorts above. Treat those clips and the fuller transcript as
overlapping evidence. A direct full-video URL and timestamps remain pending.
The creator describes per-voice VCA gain reaching +24 dB at setting 11, a filter
envelope example moving BP4 through LP2 toward LP4, and a sync patch. An external
effect is apparently muted for a dry comparison; the transcript does not reliably
identify that effect. Do not treat every demonstrated sound as dry hardware audio.
The supplied text alone does not establish the full gain-control mapping.

### Further eight-voice filter/gain and split demonstration transcript

Holger supplied another transcript on 2026-10-29; URL, publication date, title and
timestamps remain pending. The speaker describes first production units and a
shipping start, but those relative statements do not date the recording or verify
present availability. No independently reviewed audio or measured response is
claimed here.

New specific statements:

- Gain setting 7 is described as +8 dB. Subsequent levels are stated as +12,
  +16 and +20 dB, with setting 11 at +24 dB. The four-dB sequence suggests a mapping
  for settings 7 through 11, but only the endpoint setting numbers are explicit.
  Do not extrapolate the whole control range or equate nominal gain with measured
  clipping threshold. The speaker explicitly describes hard clipping in the
  output VCA on the voice core at the high setting.
- The filter sweep is demonstrated at the lower and higher gain settings. That
  gives us a useful future comparison fixture: hold the source and sweep constant,
  vary post-filter per-voice gain, then assess both spectrum and matched-level sound.
- Bandpass bandwidth can be wide or narrow. The spoken preferred value 4.4 has
  no established unit or calibration; do not implement it as Q, octaves or Hz.
- Four-way multitimbrality, layering and a keyboard split are described. The text
  marks a demonstration, but its split point, MIDI routing and voice allocation
  policy are not established.

Two ambiguities need explicit handling. First, the opening says there is one VCF.
In context and alongside the documented independent voice cores, this is best read
as one continuously variable filter per voice, not proof that all eight voices
share a single global filter. Confirm against the actual recording if needed.
Second, this supplied transcript calls the classic SEM sweep LP2 to BP4, then
introduces BP4 as an added capability. That is internally inconsistent and differs
from the earlier LP2/notch/HP2 descriptions and official specs. Keep the discrepancy
visible; it may be a transcription error or spoken slip. Do not change our classic
filter definition on that sentence.

These accounts strengthen the per-voice output-drive experiment; they do not
establish a shared envelope-memory circuit, exact waveshaper, oversampling needs,
or nonlinear filter equations. Keep our approved shared-drive path available.


For the supplied Marinelli transcript and any further video, record titles,
dates, links and timestamps when available; distinguish spoken claims from
visible controls and audible demonstrations. Compare oscillator levels/waveforms,
sync, expression routing, envelope contours and sequencing across generations.
Do not choose undocumented gain ratios or envelope curves as hardware facts.

Preserve the current approved sound and patches. Adding controls can be based on
these feature directions; claiming hardware correspondence still requires reference
recordings, declared comparison conditions and measurements.
