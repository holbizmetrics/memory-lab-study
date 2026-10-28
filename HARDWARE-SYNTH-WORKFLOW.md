# Developing software instruments from hardware synth documentation

This workflow captures lessons from the Memory Lab Study for Holger's next
hardware-inspired instrument project. Use descriptions and manuals to establish
features and testable behavior, then develop a musical software instrument with
an independently tested engine. Matching a particular hardware unit requires
reference measurements as well as feature coverage.

## Define what the project should reproduce

Agree whether the goal is an inspired instrument, matching documented controls,
matching recorded behavior, or reproducing known circuitry. These are different
deliverables. A complete plug-in format does not establish a complete instrument,
and an appealing sound does not establish hardware accuracy.

Verify the product identity before reading deeply. In this project, Relic from
Shear Electronics was confused with Reliq from another manufacturer. The latter's
manual describes a different device. Save exact manufacturer, model, revision,
manual version and source URLs with the source material.

Separate the hardware's synthesis method from the software implementation.
Relic's published architecture is analog subtractive. Our Fourier wavetables
generate saw/pulse waves inside a subtractive software signal path; that does not
imply the hardware is a wavetable synth or provide wavetable scanning as a feature.

## Build an evidence and coverage matrix

For each subsystem, record the documented behavior, implemented approximation,
open design decisions and the next measurement that would distinguish candidates.
Preserve the source passage and its location; diagrams and front-panel labels can
contain facts omitted from searchable PDF text.

| Evidence | Supports | Does not establish by itself |
| --- | --- | --- |
| Manual or official specification | Control ranges, routing, operating modes, feature inventory | Hidden circuit values or exact transfer curves |
| Schematic with component values | A specific circuit-model candidate | Every unit's tolerance, calibration or operating condition |
| Controlled hardware recordings | Measured behavior under recorded settings | Unrecorded modes or a unique internal mechanism |
| Demonstration video | Musical direction and candidate phenomena | Isolated behavior if playing, effects or loudness also change |
| Listening approval of our prototype | Musical usefulness of that example | Similarity to hardware or complete feature coverage |

In the Relic study, the classic filter profile direction is documented. Our
HP/notch/LP interpolation is an original mathematical design. The modern filter's
extra bandwidth dimension remains separate work. Do not mistake a second knob on
a conventional SVF for independent bandwidth control across every operating point.

Keep a feature-coverage checklist alongside sound quality: oscillators and their
interactions; filter modes; envelope stages and shapes; LFOs and routing; voice
allocation; controller behavior; multitimbrality; presets and state; host behavior.
Mark each as planned, implemented, measured, heard or hardware-compared. One
filter envelope and one sine LFO are useful progress, not four complete envelopes
and four multi-shape LFOs.

## Acquire reference data that can answer the next question

When hardware is available, record known MIDI events and patch settings with dry
audio, sample rate, input gain and calibration tone. Preserve raw captures, peak
headroom and timing. Record effects and normalization separately. For uncertain
subsystems, prioritize isolated oscillators, width/detune sweeps, filter cutoff
and resonance sweeps, envelope transients, velocity, pressure, repeated notes,
sustain and polyphonic overload.

Change one control at a time before studying interactions. Repeat recordings when
free-running phase, drift or voice allocation changes the result. Analog recordings
need statistical/spectral and perceptual comparison, not a demand for bit-identical
samples. A video can suggest a useful experiment without identifying the circuit
responsible for the sound.

## Use PCLA for competing hypotheses and criticism

Use the inspected PCLA-Experimental integration as a bounded research aid:
sensory framing, variants, calibration, refinement, then the relevant computation
or experiment. Supply inspected primary sources and an explicit question. Retain
original and refined candidates, rejected cases, raw responses and run costs.
Generated statements and received companion messages remain context, not automatic
authority to run code or change repositories.

PCLA helped expose two useful issues here. A common DC blocker has memory, so the
drive-history probe must measure before it. Also, the sag formula
`tanh(d*g*x)/d` tends to `g*x` as drive approaches zero; a special zero-drive bypass
to `x` creates a discontinuity. The live/native design now fades memory depth to
zero continuously below drive2, while the frozen original takes remain intact.

Research still needs source checking. PCLA called the XF4 meaning unknown, but
the official specification defines a 50/50 BP4+LP2 blend. The blend is documented;
its full underlying circuit is not. Preserve the incorrect model output and attach
the correction. When the fifth stage only validates plan fields, call it a plan
audit rather than a successful DSP experiment. Useful critique alone does not
establish a general productivity multiplier.

## Preserve a musical baseline and isolate changes

Save an approved source phrase, parameters, sample rate, initial state and raw
audio before changing the engine. Hash reference files and relevant code before
measurement. Use fresh run directories rather than overwriting failed or successful
evidence. Keep the preceding version available for comparison.

Use the same notes, velocities, phase policy and processing chain when isolating
one change. Global RMS matching helps comparisons without flattening note dynamics;
retain raw renders and gain factors. RMS equality is not perceived-loudness equality.
Avoid a hidden limiter or per-note normalization that conceals transient differences.

Study both an isolated source and its driven output. Changing oscillator phase,
width or detune also changes excitation of a nonlinear memory stage. A shared output
drive intentionally couples chord voices; test voice isolation before that shared
stage to distinguish intentional interaction from ownership errors.

The project's original polyBLEP oscillator passed pitch checks but exposed
high-register spectral problems. The Fourier wavetable candidate improved the
same nonharmonic-energy diagnostic and passed direct Fourier-reference checks.
That diagnostic excludes regions around harmonic lines, so it is not total alias
power: leakage can contribute, and folded aliases on harmonic lines can be hidden.
Static and moving-control tests answer different questions; keep both.

## Make the listening experiment understandable

Label what changes, what stays fixed and the exact key/controller sequence.
A held note with three timbre changes can sound like three key presses. The motion
study explicitly identified one note held for two seconds with control targets at
0.5, 1.0 and 1.5 seconds. The later filter and modulation studies instead used the
same seven-note phrase. These labels made Holger's feedback interpretable.

Provide explicit play, replay and stop, prevent overlapping comparison playback,
and load a default or clearly selectable source without autoplay. A grey audio
bar before selection looked broken in the first comparison. Later players use
ordinary controls with an assigned source and metadata preload. Test actual
browser output as well as file integrity; an audio activity icon alone is not
proof that the user heard sound.

Record feedback precisely. Holger called the wavetable tone musical and usable
in productions, and approved labeled filter, modulation and performance examples.
This is valuable informal musical evidence. Attribute it to those examples rather
than turning it into a blind preference result or a hardware-match claim.

## Keep the DSP independent from the plug-in wrapper

The progression here was browser listening prototype, compiled native offline
engine, then JUCE standalone/VST3 wrapper. Keep those milestones explicit. The
earlier presence of C++ files did not mean a VST3 existed, and the browser MIDI
prototype did not automatically acquire the newer native features.

Give the core explicit prepare/reset/render and event contracts. Use fixed voice
storage and caller-owned buffers. Check sample-offset events, partition independence,
reset reproducibility, finite output, note ownership and allocations in tested
render paths. Define same-pitch retriggering, sustain, channel pressure, stealing,
panic and reset-controller behavior before building UI mappings. Distinguish global
panic from channel-scoped MIDI commands.

For filters, compare with an independent response construction, not only copied
implementation equations. Our 180 static cases use a bilinear analog reference;
absolute complex response error handles deep notches better than a relative dB
ratio near zero. Add automation and input-level tests without presenting a bounded
grid as a universal stability proof.

In the JUCE wrapper, normalized float conversion initially shifted default values
enough to fail exact core parity. Canonicalizing defaults within a small fraction
of their displayed increment fixed that mismatch. Test the wrapper against the
core, then scan and instantiate the actual VST3 module. State recall, MIDI timing,
channel isolation, silence after release and reload are separate checks from
offline DSP correctness.

## Treat host installation as a separate milestone

Pin dependency versions and commit hashes. Keep upstream notices and make the
distribution-licence decision explicit before publishing. Separate source,
dependencies, build outputs and installation. A first framework build can take
much longer than a small DSP build; report actual progress without calling an
unfinished binary ready.

FL Studio requires VST3 in standard locations. Our installer copies only the new
named bundle, refuses to overwrite an existing destination and verifies file hashes.
A successful local module-loader check is useful preparation, but it is not proof
of FL Studio loading, UI behavior or hardware MIDI. Confirm those in the target
host and preserve a host project for future regression checks.

## Make controls and automation part of the instrument contract

Preserve host parameter IDs and registration order across feature additions. Add
missing controls to legacy state with explicit original defaults; do not leave a
previously edited value behind when loading an older patch. Test compatibility
against a reused processor instance as well as a fresh instance.

Define when a parameter takes effect. Our envelope stages latch at note-on;
changing a release knob on a held note is therefore different from editing a
live modulation amount. Label that behavior in the editor. A useful envelope
preview should distinguish a fixed illustrative sustain duration from actual
key/pedal-held sustain, and flag routes whose modulation amount is zero.

JUCE parameter attachments provide the automation foundation, but verify both
directions: knob gestures notify the host, and host changes update controls and
curves without generating new user-edit gestures. Include page switches and
hidden controls. In this project, a separate test checked 35 stable automatable
IDs, balanced begin/value/end gestures and value recall; FL Studio then recognized
Cutoff via Last tweaked and drove its knob/value/curve from a Playlist clip.
This did not by itself prove mouse-performance recording, host undo grouping,
fast-ramp audio quality or sample-accurate parameter automation.

For time-based features, use the sample rate supplied during audio preparation,
not an assumption that a host accessor is already initialized. A keyboard test
caught the initial arp clock using an uninitialized host-accessor rate in the
standalone test harness. Test fractional clock accumulation across block sizes,
tempo fallback, pedal-held input chords, release, panic and transport transitions.

## Preserve playable milestones and diagnose host failures separately

Tag the exact committed source for each playable alpha milestone, with release
notes that separate automated checks, user playing feedback and remaining limits.
Keep previous installed bundles and hash records. Do not attach binaries before
settling distribution licensing. A source tag and an installed binary version
can use different prerelease labeling if that relationship is documented.

Work on a separate host-project copy for automation and reload tests. A successful
module-loader test does not cover the full project's other instruments, drivers,
accessibility providers, editor lifetime or host reload path. Extended enjoyable
playing is valuable feedback, but is not a timed soak test or universal stability
proof.

Keep actual crash dumps distinct from snapshots of a healthy running process.
A debugger wake event in a snapshot is not the previous crash's exception.
Check file-access errors explicitly: an access-denied search must not be treated
as proof that no dump exists. Our original dump was later located by Holger.
Analyze the exception subcode and faulting stack before interpreting a generic
error label or blaming an audio driver merely because its DLL is loaded.

Treat accessibility inspection as an active integration condition. The original
FL reload failure occurred in UIAutomationCore provider cleanup after UI
inspection; the exact owner remained under investigation. Avoid further UI
inspection during teardown until isolated. Do not disable accessibility globally
or change audio/DSP behavior on that evidence alone.

Practice debugger cleanup on a disposable target. Interrupting the live capture
debugger through the shell terminated the attached FL process in this session;
that was an agent-caused termination, not another spontaneous crash. Detach
deliberately and verify that the host survives before ending a live session.

C# scripting in WinDbg can make native triage reusable: the user's WinDbgAotExt
hosted CoreCLR, queried typed module data with LINQ and produced a snapshot report.
The debuggee itself need not have a managed heap. Keep raw logs, symbol limits,
failed inspections and unresolved ownership questions alongside conclusions.
Do not upload memory dumps as ordinary source/release artifacts.

## Suggested starting package for the next instrument

Create a separate project containing the product/source index, evidence and
coverage matrix, a small deterministic reference phrase, parameter/event contracts,
an isolated DSP harness, a listening player, raw captures/renders and a result log.
Start with one uncertain, musically meaningful subsystem and compare bounded
alternatives. Expand only when measurements and playing feedback justify the next
step. Preserve the baseline while building toward complete feature coverage.

Relevant local evidence: native/OSCILLATOR-AUDIT.md, WAVETABLE-CANDIDATE.md,
MOTION-AUDIT.md, FILTER-PROFILES.md, MODULATION.md, PERFORMANCE.md and
plugin/BUILD-RESULT.md. The detailed run artifacts remain under outputs/.

Primary references:
- [Relic specifications](https://www.shearelectronics.com/relic/specs/)
- [Relic voice core](https://www.shearelectronics.com/relic/voice-core/)
- [Andrew Simper SVF derivation](https://cytomic.com/files/dsp/SvfLinearTrapezoidalSin.pdf)
- [JUCE CMake API](https://github.com/juce-framework/JUCE/blob/9.0.4/docs/CMake%20API.md)
- [JUCE licensing](https://juce.com/get-juce/)
- [FL Studio plug-in installation](https://www.image-line.com/fl-studio-learning/fl-studio-online-manual/html/basics_externalplugins.htm)
