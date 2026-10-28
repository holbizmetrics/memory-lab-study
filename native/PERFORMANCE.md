# Native performance events

Holger approved all six previous modulation examples as musically usable,
believable and consistent with their intended behavior. That approval is recorded
for the modulation checkpoint, not automatically applied to these new clips.

## Implemented contracts

Sustain is per MIDI channel, threshold .5 on normalized0..1 values. Note-off clears
physical key ownership. While the pedal is down, the envelope gate remains on;
pedal-up releases only that channel's key-up voices. Duplicate note-offs do not
restart release. Same channel/pitch retriggers a single slot; independent overlap
counts or VST3 note identifiers are not supported yet.

PitchBend accepts normalized -1..1 with a fixed +/-2-semitone range, applied to both
oscillators on the selected channel. ChannelPressure accepts0..1; optional
PressureDepth routes it to cutoff in +/-2 octaves. Default depth0 preserves the
approved patch. Controllers and pressure depth use20 ms smoothing. Channel
pressure intentionally affects all active voices on its channel, including release
tails; controller values persist for later notes until changed or reset. No MPE,
polyphonic pressure, configurable bend range or MIDI-byte decoding is added here.

Eight main voice slots remain. Allocation prefers inactive slots, then the
quietest releasing slot, then the oldest held slot; same channel/pitch reuses its
slot. Replacement applies a5 ms transition from the preceding slot's last output
sample into the new voice. This reduces abrupt steps without extra voices or
allocation. It is a held-value transition, not a continuation of the stolen voice's
waveform or a full release-tail crossfade, and is not certified click-free.
Note-off for a stolen pitch cannot release a different pitch occupying that slot.

AllOff remains the global graceful release command, cancelling all pedal holds.
AllSoundOff immediately clears voices and memory signal state, producing silence
while retaining controller targets. Reset clears controllers and defaults. A future
MIDI wrapper must map channel-mode messages deliberately; these global commands
are not an implementation of every MIDI channel-mode policy.

## Compiled and tested

The project already builds an executable native C++ engine, and its offline
renders generate the listening examples. It is NOT yet a VST3. The browser MIDI
prototype is separate. JUCE wrapper/UI/host integration has not been added.

outputs/performance-audit-1 retains the frozen contract, hashes, native log,
raw renders, report and four-clip player. Semantic tests cover channel-specific
pedal hold/release, key ownership, stolen-note note-offs, exact panic silence and
controller isolation. Four scenarios at44.1/48/96 kHz stay finite and within the
declared headroom and finish with no active/held voices. Block sizes17/127/512
match full renders exactly:36 comparisons over12 mode/rate cases. No C++ new
allocations are detected during the tested renders.

The default phrase remains bit-identical to the approved LP baseline. Existing
native checks pass again; the48,000-sample memory regression remains exact.
Audio frequency estimates for the bent two-oscillator pair are about3.6–3.8 cents
above the undetuned oscillator target, consistent with the intentionally detuned
pair and within the declared25-cent aggregate check. This is not a one-cent
individual-oscillator measurement or device-latency test.

## Listen

outputs/performance-audit-1/index.html has four4-second demonstrations, each
globally matched to -24 dBFS RMS and checked after WAV decoding with peak headroom:

1. Sustain: C4 on, key-up at.75 s, pedal-up at2.5 s.
2. Bend: held C4, +2 semitones at.5 s, -2 at1.5 s, center at2.5 s; key-up3.2 s.
3. Pressure: held C4, +1-octave cutoff at.5 s, back at1.5 s, up at2.5 s; key-up3.2 s.
4. Voices: nine staggered notes fill eight slots; oldest is replaced. G4 repeats
   at1.6 s and releases at2 s. A stale first-note note-off at2.1 s is ignored;
   global release at2.5 s.

Browser playback, perceptual quality and hardware controllers for these new
native features remain unverified. The existing browser MIDI prototype has not
been upgraded by this milestone. No total-aliasing or real-time deadline claim.

```powershell
cmd /c native/build-native.cmd
outputs/native-build/memorylab_native.exe --performance-test
```

performance-audit.py refuses to overwrite the retained run. Future full runs need
a fresh output directory. The next integration work is the native interactive
JUCE/standalone/VST3 boundary, with complete event mapping and state handling.
