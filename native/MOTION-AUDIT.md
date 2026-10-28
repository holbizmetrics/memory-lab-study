# Native oscillator motion checkpoint

Holger's listening feedback on the wavetable comparison: the tone sounds different
and clear, remains musical, and is something he would use in a production. This
is direct informal feedback on the comparison, not a blind preference test or
hardware similarity judgment. The baseline clips and browser instrument remain.

## Measured motion checks

outputs/motion-audit-1 contains the frozen contract, source/executable hashes,
native check log, raw fixtures, report and three matched listening clips.
No sound-engine code was changed for this milestone; only measurement harnesses
were added. The approved memory stage is unchanged.

- Nine full-voice combinations: pulse-width steps, detune steps, and combined
  steps at 44.1/48/96 kHz. Each uses MIDI60 at velocity .85, note-off at two seconds,
  and one second for release. Target changes occur at .5, 1 and 1.5 seconds.
- All output is finite and within the declared headroom. Notes finish releasing.
  For all cases, block sizes 17/127/512 match the single-block render exactly:
  27 successful partition comparisons. The tested renders allocate no C++ new
  memory according to the harness counter.
- At fixed phase, two-sided perturbations around all nine table-bank boundaries
  differ by at most 1.06e-9 for a 1e-9 relative frequency perturbation. A 2e-9
  pulse-width perturbation differs by at most 1.73e-6. These are local mathematical
  continuity checks, not guarantees for arbitrary modulation or hard retriggers.
- An isolated bank fixture deliberately crosses a harmonic-bank boundary while
  pitch and width targets change with 20 ms smoothing. An independent direct
  Fourier sum follows the intended time-varying harmonic weights. Largest sample
  error across the three rates: 1.05e-5, below the frozen 1e-4 tolerance. Exported
  control state also matches independently calculated recurrences within 1e-10.
- Existing native core checks pass again. The 48,000-sample native/browser memory
  comparison remains exact.

These results support continuity and correctness in the declared cases. They do
not certify perceptual clicklessness, dynamic alias power, CPU deadlines, voice
stealing or retrigger transients. A time-varying Fourier reference can itself have
modulation sidebands, so agreement with it is not an alias-free proof. The bank
fixture isolates lookup and controls; event/block tests exercise the actual Synth.

## Listening

Open outputs/motion-audit-1/index.html. It contains three full native-voice clips,
with the filter and approved memory-drive settings. All are globally matched to
-24 dBFS RMS. Selection/playback uses ordinary audio controls with metadata preload;
no sound starts automatically. Browser playback of this new page has not been
automated. Listen for ticks or abrupt unwanted jumps at .5, 1 and 1.5 seconds.
Perceived quality of these new transitions is still unassessed.

## Next work

With static and motion oscillator evidence retained, the next sound-building
milestone is the original classic HP/notch/LP filter profile. It requires external
response targets, cutoff/resonance and automation tests, and level-matched musical
comparison before adding more filter modes. It is not a reconstruction of Relic's
undocumented circuitry. JUCE integration and full synth features remain pending.

To reproduce the native motion checks after a build:

```powershell
outputs/native-build/memorylab_native.exe --motion-test
```

motion-audit.py refuses to overwrite its output directory. Choose a new run path
in the script for another full measurement/render run, preserving earlier evidence.
