# Relic interview evidence by generation

Recorded 2026-10-29. Keep historical prototype descriptions separate from later
specifications. A creator's interview is primary evidence of stated design intent
and features, not an independent circuit verification or measured sound match.

## Quick source index for duplicate checks

| YouTube ID | Identified source | Material retained here |
| --- | --- | --- |
| RT608NU78CM | Sound On Sound, Relic-6 / NAMM 2017 | Supplied transcript and link |
| CUqPG2ycv1A | Sonicstate, SynthPlex 2022 | Supplied transcript and panel screenshots |
| s51LRL1qUpM | Marinelli, Oberheim Inspired Analog Synth / NAMM 2024 | Supplied transcript, link and title screenshot |
| fXIGjvYH1cM | RockoN Report, NAMM 2024 Day 4 | Report page read; supplied walkthrough text; title screenshot |
| Owvc3p4HrnA | Marinelli, Did Jacob Brashears Reinvent the Synthesizer? | Link supplied with long session transcript; association supplied by user |
| 7cFAWbFDX9k | Marinelli, Some of the Most Expressive Analog Sounds | Indexed via Synthtopia/Matrixsynth; exact transcript association unverified |
| _osrNgzRG7E | Sonicstate, NAMM 2024 Relic — No Talking | New listening candidate; video not independently reviewed |
| Ty1hH-6b4Qw | Filter-profile short | Supplied transcript and link |
| wZbcKiuWqjY | Encoder short / NAMM 2024 | Supplied transcript and screenshot |

The two title screenshots supplied at the end of tab cleanup confirm s51LRL1qUpM
and fXIGjvYH1cM as distinct NAMM 2024 recordings. Being indexed does not mean
the video audio has been independently reviewed. A repeated URL adds no new source.

Other pages read directly: official specs and voice-core pages; the official
2024 newsletter (www hostname); RockoN's report; Synth Anatomy's rolling Relic
article; Synthtopia's October 2026 article; Matrixsynth's Shear Electronics archive.
Full references, access limits and version differences are documented below.

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
Holger subsequently supplied the matching transcript with video URL
https://www.youtube.com/watch?v=Owvc3p4HrnA . Title, publication date and timestamps
are still pending. The repeated transcript is not a separate corroborating source. These are
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

### Rosen Sound long-form session: supplied transcript reviewed

Holger supplied a screenshot identifying Rosen Sound's "A Day in the Shop Ep 18 -
Shear Electronics (Jacob Brashears), ARP 2500, Rob's Birthday". The player shows
1:53:26 duration. Holger subsequently supplied the transcript, repeatedly; count
those copies once. A directly supplied URL and timestamps remain pending.
The transcript contains substantial unrelated repairs, ARP 2500 and shop discussion;
do not attribute those instruments' features or faults to Relic.

The Relic segment explicitly describes four physical cards with two independent
voice cores each, pre-filter VCAs for oscillator 1, oscillator 2 and white noise,
and stereo output VCAs numbered 6/7 per voice. VCAs 4/5 are not fully mapped by
that passage. Four system outputs, stereo grouping and four-part operation repeat
earlier claims. This refines the signal-path inventory without supplying equations.

The hands-on session supports a usability lesson: briefly explain the bank buttons,
then let the player explore controls. Preset discovery and clear control mapping
matter alongside detailed engineering explanations. Put a playable first-sound
path before technical reference material in any future manual.

Pulse-width zippering is described during the session and the player likes that
particular sound. This does not imply every control should be unsmoothed, or that
all clicks are acceptable. Keep intentional stepped movement separate from bugs.
The filter-envelope numbering discussion is confused in the transcript; do not
freeze a universal envelope-to-destination mapping from it.

A controller interoperability issue is discussed. MIDI running status is proposed
as one possible explanation, not diagnosed. The lesson for our wrapper is to test
different controllers and correctly handle or ignore applicable events. Raw MIDI
byte-stream parsing belongs below the VST3/JUCE event boundary; do not add a running
status parser to our already decoded host-event handler based on this speculation.

Serviceability comments are commitments/opinions, not proof of future parts or
schematic availability. Positive playing impressions are not measured fidelity.
Neither the unrelated shop repairs nor the personal anecdotes are needed in our
instrument specification.

### Individually supplied Matrixsynth posts

The January 8, 2018 update was read directly:
https://www.matrixsynth.com/2018/01/an-update-on-shear-electronics-relic.html .
Its reproduced manufacturer statement describes adding individual encoder bars,
optical encoders and MIDI-cable software updates during the post-2017 redesign.
Treat these as historical plans, not present firmware/update specifications.

The directly supplied October 2026 post URL is
https://www.matrixsynth.com/2026/10/the-relic-some-of-most-expressive.html .
Direct fetching failed; its entry was already read on the accessible label archive
and linked to 7cFAWbFDX9k. Do not claim the standalone page or embedded audio was
successfully reviewed.

### Matrixsynth archive: source index and historical envelope detail

Read on 2026-10-29: https://www.matrixsynth.com/search/label/Shear%20Electronics .
Its embedded-video URLs distinguish these recordings (dates below are archive
post dates, not independently verified video upload dates):

| Archive date | Video title / description | YouTube ID |
| --- | --- | --- |
| 2026-10-16 | The Relic: Some of the Most Expressive Analog Sounds I've Ever Played | 7cFAWbFDX9k |
| 2026-09-25 | Did Jacob Brashears Reinvent the Synthesizer? — THE RELIC | Owvc3p4HrnA |
| 2024-02-07 | Oberheim Inspired Analog Synth by Shear Electronics, NAMM 2024 | s51LRL1qUpM |
| 2024-01-31 | Sonicstate: NAMM 2024 Relic — No Talking | _osrNgzRG7E |

Direct video fetching failed. The long transcript supplied with Owvc3p4HrnA
therefore retains its user-provided association; the archive does not establish
that it duplicates 7cFAWbFDX9k. The no-talking demo is a new listening candidate.

The January 22, 2017 post reproduces historical manufacturer copy:
https://www.matrixsynth.com/2017/01/namm-2017-shear-electronics-relic-6-new.html .
It claims envelope output at 40 kHz / 14 bits and describes attack toward 6.5 V
terminated at 5 V, plus adjustable voice-to-voice envelope timing variation.
These are early design claims preserved by a secondary archive, not an independently
verified CEM3310 measurement or a specification for 2026 firmware. They motivate
testing a truncated exponential attack rather than assuming every envelope stage
is a simple exponential. Verify authentic chip documentation before claiming a
chip-specific formula; do not introduce a 40 kHz control clock or quantization
into the plugin merely because the old hardware description mentions them.

### Synthtopia October 18, 2026 article

Holger supplied this new URL and the article was read on 2026-10-29:
https://www.synthtopia.com/content/2026/10/18/shear-electronics-relic-features-eight-independent-analog-voice-cores/ .
It summarizes the later official per-voice and four-part feature inventory and
links Marinelli's "The Relic: Some of the Most Expressive Analog Sounds I've Ever
Played", video https://www.youtube.com/watch?v=7cFAWbFDX9k . An earlier tentative
association with the long transcript remains unverified: Holger subsequently
supplied that transcript alongside the different video ID Owvc3p4HrnA. Keep the
two links distinct until their actual content is compared. The linked video could
not be fetched to independently verify its content or audio.
The article adds no circuit equations or measured calibration data. Its reported
16-20-week availability wording must not re-date the July 2026 manufacturer status.

Holger also pasted the Synth Anatomy text after direct reading had succeeded.
The Relic section matches that page; the appended Polyend Drums "Page 2" is an
unrelated article and supplies no Relic evidence. Comments are reader opinions,
not independently verified specifications or measurements.

### Secondary chronology: Synth Anatomy rolling article

Holger supplied https://synthanatomy.com/2026/05/shear-electronics-relic-bi-timbral-ob-x-clone-made-a-comeback-at-syntplex-22.html .
The page was successfully read on 2026-10-29. It combines a May 7, 2026 update,
a January 31, 2024 section and the original October 31, 2022 article. Its older
bi-timbral URL/summary must not override its explicit 2024 four-part update or the
manufacturer specs. The 2024 section repeats the newsletter inventory, rather
than independently measuring its features. The 2022 section's screenless claim
conflicts with the supplied creator transcript describing an IPS display; do not
promote that secondary statement into a UI requirement. Use this page mainly for
chronology and source discovery, not circuit equations or sound-validation data.

Holger also supplied the RockoN/MIROC interview page directly:
https://www.miroc.co.jp/report2/namm-2024-day-4-shear-electronics . This is the same
interview-page URL linked by the manufacturer newsletter, not a separate new
newsletter. An initial web fetch failed, but retrying with a trailing slash
succeeded. The page is dated January 29, 2024 and says RockoN obtained a direct
line recording alongside the interview. That makes its linked video a promising
audio reference, but the page text does not prove every segment is effects-free
or supply recording settings, calibration or downloadable raw audio. Reading
the page is not the same as having watched or analyzed the video.


### Manufacturer NAMM 2024 newsletter supplied by Holger

The supplied newsletter is explicitly labeled NAMM 2024 and signed Jacob and
Cheryl. It links to https://www.miroc.co.jp/report2/namm-2024-day-4-shear-electronics .
Holger subsequently supplied the direct manufacturer archive URL:
https://shearelectronics.com/relic/status/2024-0130-newsletter-namm-2024/ .
Its URL identifies the archive entry as 2024-01-30. The repeated supplied text
matches the newsletter already summarized here and is not a separate source.
Direct page reading succeeded on 2026-10-29 using the www hostname:
https://www.shearelectronics.com/relic/status/2024-0130-newsletter-namm-2024/ .
The filter sequences, four-part architecture and LFO warp wording match the
supplied text. The non-www URL failed in the web tool; this does not imply
different content. The RockoN page also links to video fXIGjvYH1cM; direct video
fetching failed, so page reading is not independent audio analysis.
This dates its stated design stage without relying on a video's relative shipping
claims. It explicitly lists eight independent cores, four-way multitimbrality,
four-zone allocation and 512 programs containing four patches each.

Its filter inventory differs from the later specs:

| Source version | Normal / modern profile sweep | Classic profile sweep |
| --- | --- | --- |
| NAMM 2024 newsletter | LP4 / LP3 / LP2 / XF2 / BP4 | LP4 / LP3 / LP2 / NF2 / HP2 |
| Later supplied official specs | BP4 / XF4 / LP2 / LP3 / LP4 | HP2 / NF2 / LP2 |

Sequence orientation alone is not a contradiction, but the early XF2 naming and
Classic-mode LP3/LP4 coverage differ. Do not assume that the modern Classic mode
contains the extra slopes, or that the earlier label was simply an error. These
may reflect revisions or documentation changes; actual mode behavior needs
confirmation before a hardware-correspondence claim.

The newsletter also describes four LFOs with three-dimensional warp capability.
This adds a feature direction not captured by merely listing eight waveform shapes.
It does not define the warp axes, ranges, equations, modulation destinations or
which later firmware retains that behavior. Record it as a historical stated
capability, not enough information to implement a matching warp engine.

This is useful evidence that later-generation four-part architecture was already
being described in 2024, while the SynthPlex 2022 transcript described two parts.
It does not prove production readiness independently of the manufacturer's claim.


### Official voice-core page text and four supplied images

Holger supplied the page text and images on 2026-10-29. Source page:
https://www.shearelectronics.com/relic/voice-core/ . The supplied images show a
multi-layer PCB illustration with one strip highlighted, a sawtooth scope trace,
an artistic component photograph and a scope photograph of an amplitude-shaped
audio signal. Original capture settings and measurement files are not supplied.

- The PCB artwork supports discussion of a voice board, but is not a readable
  schematic/netlist. Do not derive filter wiring, component values or circuit
  equations from its appearance, or treat the four illustrated strips as the
  instrument's total voice count.
- The saw trace has an approximately straight rising ramp and a fast but finite,
  visibly rounded reset. That is a useful qualitative waveform reference, not
  evidence for an infinitely sharp ideal saw or a particular digital correction.
  Scope/probe bandwidth and capture conditions could contribute to the reset shape.
  Visible scale markings alone do not establish the production oscillator's full
  voltage range, tuning, drift or spectrum across register.
- The second scope image shows a dense audio waveform with a rising amplitude,
  a curved fall toward a sustained region and a curved release. It is not an
  isolated envelope control-voltage trace, and a readable time scale is absent.
  Do not fit exact envelope coefficients/time constants or claim a particular
  attack law from this picture. It can guide future qualitative comparisons.
- The component photograph is illustrative; it does not measure transconductance,
  clipping, matching, thermal behavior or the source of audible character.
- The page explicitly describes an SVF plus an extra dipole and additional gain
  cells for profile sweeping. This strengthens the stated architecture direction,
  but does not specify the dipole's complete connection, transfer function or
  feedback/gain laws. A simple interpolation between arbitrary filter outputs
  must remain labeled as our own design rather than a reconstruction.

The envelope algorithm and component-authenticity language still supplies no
complete implementable model. Preserve the source images as qualitative evidence;
reference audio, isolated control signals and capture settings would support much
stronger measurements. No runtime changes follow automatically from these images.


### Official product/specification text supplied by Holger

On 2026-10-29 Holger supplied the official product-page text and its specification
section, then the separate specs-page text. The repeated specification block is
one manufacturer specification, not independent corroboration. Reference pages:
https://www.shearelectronics.com/relic/specs/ and
https://www.shearelectronics.com/relic/relic . These observations use the supplied
text, not a new independently dated site capture.

The per-voice inventory is explicit: two SEM-topology VCOs, seven VCAs, four
dAHDSR envelopes, four eight-shape LFOs and one supervariable VCF. Thus one VCF
means per voice in this inventory, resolving the isolated interview ambiguity.
The system inventory is four-way multitimbral, four-zone voice allocation and
512 programs with four patches each. Do not turn eight independent physical
voice processors into a software requirement for eight OS threads.

Published filter terminology is CLASSIC HP2/NF2/LP2 and MODERN
BP4/XF4/LP2/LP3/LP4 with variable bandwidth. XF4 is specified as the 50/50
BP4+LP2 blend. This is the later naming baseline; the photographed 2022 XF2 label
remains valid historical evidence. Blend endpoints do not specify interpolation
law, phase alignment, normalization or a complete nonlinear realization.

Envelope contours are described as CEM3310-inspired and generated with an
exponential algorithm. Our envelope segments are currently linear, so matching
the stage names is not matching their contour. The named summation algorithm
does not supply implementable coefficients, target overshoot, time calibration,
or retrigger/release rules. Claims of mathematical perfection and glitch freedom
remain manufacturer claims, not independently verified numerical properties.
Preserve existing envelope sound when adding any future optional contour mode.

The LFO inventory explicitly includes triangle, sine, pulse and saw, each with
bipolar variants: eight shapes. Our single per-voice sine LFO is a subset. Shape
names alone do not fix polarity offsets, phase convention or modulation scaling.

The oscillator range text gives note names and numeric endpoints that warrant
care: its quoted C0/C7 frequencies do not span seven octaves. Do not silently
copy that pair into a tuning/calibration test. Pitch standard, octave naming and
the numeric endpoint must be clarified before treating them as a consistent range.

The product narrative places hardware freeze in early 2024 and manufacturing
development afterward. Its status paragraph is explicitly as of July 2026 and
offers an estimated shipping window. It does not resolve the recording date of
interviews claiming shipping, or independently verify current availability.
Treat material-authenticity and comparative-sound statements as maker claims;
they do not establish a measured match for our original software instrument.


### Further creator walkthrough: compensated gain staging and external reverb

Holger supplied another transcript describing the first eight-voice prototype;
URL, date and title remain pending. Features overlap the NAMM 2024 walkthroughs,
so do not assume this is an independent instrument or recording session.

The new detail is an explicit example of core VCA gain at setting 7 and final-stage
gain at setting 3: the creator says their gains offset while additional distortion
remains. This supports separate drive and output-level controls, and motivates
matched-level comparisons. It does not prove that the complete nonlinear chain
has identical RMS/peaks, or establish all lower gain settings in dB. Our listening
study's offline RMS matching is a comparison aid, not that hardware gain circuit.

This transcript explicitly identifies Lexicon reverb on a wet sync patch, with a
dry comparison described. Therefore the wet sound is not evidence of built-in
effects. It also describes five oscillator modulation slots; exact destinations,
source ranges and combinations are not fully specified by this excerpt.
The continuous filter-profile sweep, bandwidth control, +24 dB top gain setting
and touch-dependent encoder display repeat earlier stated features.


### Sonicstate SynthPlex 2022: fuller transcript and panel screenshots

Source identified by Holger's screenshots as Sonicstate's
"SynthPlex 22: Shear Relic OB Inspired Polly Is Back!", published 2022-10-31:
https://www.youtube.com/watch?v=CUqPG2ycv1A . Screenshots show 4:10 and 4:27 of
24:37. The description explicitly says no audio was ready. This is interface and
design-intent evidence, not a source for comparing audible timbre.

- Eight voices and bi-timbral operation are described at this stage, before the
  later four-way multitimbral descriptions. Four envelopes and four LFOs per voice
  are already present. Do not collapse the generations into one fixed feature set.
- The transcript explicitly names both a built-in arpeggiator and sequencer.
  This is stronger evidence than a printed ARP label, though their operating modes
  remain unspecified and no working demonstration is established here.
- The creator states 256 encoder steps per revolution. That is an encoder-motion
  resolution claim, not a bound on the total parameter resolution or proof of
  uniform spacing in seconds/Hz. Touching a control reveals its value without
  requiring a value change. Eight mini bars provide simultaneous context.
- Envelope timing uses a nonlinear scale, described here as reaching 20 seconds;
  later material describes 100 seconds. The apparent reference to sustain as a
  time is not adopted: sustain is a level. No precise scale law is established.
- Separate oscillator coarse/fine tuning, fine range +/-100 cents, saw/pulse/off,
  key/bend tracking switches, hard sync from VCO2 to VCO1, and disabling VCO2
  portamento are described. Exact mode interactions and extreme pulse-width
  behavior still need direct verification.
- Mixing includes VCO crossfade and white noise. Gain setting 5 is described as
  nominal 1 Vrms, then 6/+4 dB, 7/+8 dB, 8/+12 dB, 9/+16 dB and 10/+20 dB;
  setting 11 is the extra range. This fills in the positive-gain mapping more
  clearly than the later short excerpts. It does not calibrate our digital
  full scale, clipping voltage or negative-side mapping.
- Envelope and LFO level modulation can come from other modulation sources.
  The example uses envelope 3 to fade in LFO2; velocity, wheel, aftertouch and
  key position are also described as sources. Time modulation can shorten attack
  with harder playing. The spoken 30-to-4-ms / -2 example is insufficient to
  derive the exact timing law or source normalization. A velocity-level example
  has ambiguous depth semantics; do not invent a formula from it.
- Detune/variation controls are described beyond VCOs, including filters, LFOs,
  envelopes and VCAs. Distribution, per-voice persistence and update behavior
  are not established, so this is not evidence for randomizing every sample.
- The filter discussion describes two dipoles and variable bandwidth. The panel
  at 4:10 visibly separates CUTOFF, RESONANCE, PROFILE and BANDWIDTH, with four
  further modulation controls. Scale labels include LP4, LP3, LP2, XF2 (NF2),
  BP4 (HP2). Later official material uses XF4 for its modern X blend; preserve
  the early panel label rather than silently rewriting it to match later terms.
- At 4:27 the selected envelope row visibly includes DELAY/HOLD, ATTACK, DECAY,
  SUSTAIN, RELEASE, LEVEL MOD and TIME MOD. This supports the interface inventory,
  not a complete determination of the shared DELAY/HOLD control's semantics.

The creator's broad analog-versus-digital judgments are opinions, not measured
limits on software synthesis. Our aim remains musical usefulness and explicit
original modeling choices, with hardware correspondence requiring comparisons.


### Fuller NAMM 2024 transcript supplied subsequently

Holger subsequently provided the fuller interview that appears to contain the
encoder and filter shorts above. Treat those clips and the fuller transcript as
overlapping evidence. Holger subsequently matched this transcript to
https://www.youtube.com/watch?v=s51LRL1qUpM . Timestamps remain pending.
This differs from RockoN's linked fXIGjvYH1cM video; similar demonstration material
does not establish that the two video IDs identify the same recording.
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
