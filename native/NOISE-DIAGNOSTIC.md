# Noise integration: matched-fixture and drive-context check

Holger reported that integrated 0.7 noise was unclear and seemed to damage the
sound when judged generally. The approved offline examples do not establish
approval of the integrated sound. His actual patch settings were not captured.

The original study bypassed shared drive/memory and per-voice drive. The plugin
defaults instead include shared drive 5 and memory .8. Noise is summed with the
oscillators before the amplitude envelope and filter, so all three sources also
enter the nonlinear shared drive together. Added noise can change the processing
of the oscillator signal itself; lowering final OUTPUT does not change that
upstream interaction. This is a plausible explanation, not a diagnosis of the
uncaptured user patch.

## Matched production comparison

memorylab_noise_diagnostic compiles the production voice path, without the fixed
MEMORYLAB_EXPERIMENTAL_NOISE hook. It settles the live noise/mixer controls for two
seconds without notes, then runs the original study fixture, including the same
notes, envelope, filter sweep, velocities and bypassed drives. It reads the six
approved raw float WAVs from outputs/noise-study-1 and compares every sample.

Run outputs/noise-diagnostic-1/comparison.csv reports maximum absolute difference
and RMS difference of exactly zero for all six cases (percussion/chord ×
oscillators-only/noise-blend/noise-solo). Thus the production engine reproduces
those examples exactly under matched settled conditions. This is an engine
comparison, not a recording of the actual FL patch or a subjective quality verdict.

## Drive-context listening comparison

The same folder's index.html offers nine independently RMS-matched chord clips:
oscillators only, oscillator/noise blend .08, and noise only .35, each with:

- Both drives bypassed, matching the original study's processing.
- Shared drive 5 and memory .8, matching the plugin's drive defaults.
- Voice drive 24 dB plus shared drive 5 and memory .8, exposing the stronger case.

All use the same chord, original study envelope, Q 2.72 and 120 Hz–6 kHz sweep.
These are controlled contexts, not full default-patch reproductions. Raw WAVs,
levels.csv and verification.json are retained. The separate PowerShell/C# verifier
checked 18 WAVs and nine playback clips at −24 dBFS RMS within .001 dB with peak
headroom, player links and source/file hashes. Browser playback is not automated.

Native CTest 5/5 remains passing. Default phrase and installed 0.7 binary hashes
remain unchanged. No DSP correction or installed-plugin replacement was made.
The current routing has a shared filter, not an independent noise filter. Holger
confirmed that explanation made sense; integrated musical approval remains open.
