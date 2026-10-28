# First modulation slice

Holger approved all four previous classic filter examples as good sounding and
consistent with their labels (HP, notch, LP and sweep). That is informal listening
approval of those examples, not hardware correspondence. Preserve their files.

Added one dedicated filter ADSR and one sine LFO per voice. They are distinct from
the existing amplitude envelope and shared output-stage memory follower. This is
not yet the complete four-envelope/four-LFO feature inventory or a modulation matrix.

## Controls

- Filter envelope: attack20 ms, decay350 ms, sustain.15, release400 ms; linear
  contours, reset on note-on. Cutoff contribution is depth-in-octaves multiplied
  by envelope and normalized note velocity.
- LFO: sine, note-retriggered phase0 for each voice, rate .05–20 Hz. First study
  uses 4.5 Hz. Both oscillators receive the same pitch modulation within a voice.
- Pitch depth: +/-50 cents, PWM depth: +/- .35 width units, cutoff LFO depth:
  +/-2 octaves, filter-envelope depth: +/-4 octaves. These are parameter limits,
  not a claim that every combination has been musically or spectrally validated.
- Routing depths and LFO rate use 20 ms parameter smoothing. Effective pulse width
  is clamped .1–.9; effective cutoff is clamped 30 Hz to min(18 kHz,.4*sampleRate).
  Filter profiles, oscillator bank and approved drive code are unchanged.
- Defaults set all depths to zero. LFO phases and filter-envelope state reset
  with note ownership. Voice stealing remains abrupt, as previously documented.

The new native Param entries are LfoRate, PitchLfo, PwmLfo, CutoffLfo and
FilterEnvelope. They do not add controls to the old browser prototype; JUCE/native
interactive controls remain future work. No third-party framework was installed.

## Tests and evidence

outputs/modulation-audit-1 retains the frozen contract, code/executable hashes,
raw renders, native test output, report and player. At44.1/48/96 kHz, six modes
(off, four isolated routes, combined) stay finite and within the test's headroom.
Their output matches exactly at block sizes17/127/512:54 partition comparisons
over18 mode/rate cases. The tested renders show no C++ new allocations and release
all notes. The dedicated envelope's contour and release match independent time
expressions within1e-9 at all three rates.

The modulation-disabled 48 kHz phrase is bit-identical to the previously approved
filter-profiles-1 LP raw render. Each enabled route differs from that baseline
above the preregistered RMS threshold1e-4. These establish route activity and
regression behavior, not perceived benefit. Existing native core checks pass and
the 48,000-sample native/browser memory-port regression remains exact.

No universal stability, worst-case deadline, total-aliasing, perceptual-click or
full-architecture claim follows from this bounded test set. One shared LFO within
a voice intentionally correlates its routes. Notes have independent retriggered
LFOs; shared output drive can couple their audible results. CPU and spectrally
demanding combinations still need additional measurement before production use.

## Listening

Open outputs/modulation-audit-1/index.html. The six clips use the same seven notes,
velocities and approved baseline; each complete6.8-second clip is matched to
-24 dBFS RMS. Decoded WAVs were checked within .001 dB with peak headroom.
Pre-roll100 ms lets depth smoothing settle before the identical phrase, and is
trimmed from the listening files. No sound autoplays; each clip has loaded-source
audio controls. Browser playback and approval of these new clips are unverified.

Isolated depths: filter envelope1.5 octaves (velocity scaled), vibrato8 cents,
PWM .15 width units, cutoff LFO .5 octaves. Combined uses the same settings, not
an automatically selected winner. Further movement within held notes is modulation,
not additional note attacks.

## Reproduce

```powershell
cmd /c native/build-native.cmd
outputs/native-build/memorylab_native.exe --mod-test
```

modulation-audit.py refuses to overwrite its evidence directory. Use a fresh run
path for any later full audit. Next evaluate listening and voice/performance
ownership before expanding modulation coverage or adding the JUCE wrapper.
