# Filter family listening study

This isolated experiment follows installed 0.7. It keeps the original classic
HP/notch/LP filter unchanged and explores additional colours before any plugin
integration. Only memorylab_filter_family compiles the experimental hook and
filter-family.hpp. No new host parameters, patches or installed files are changed.

## Declared original model

All comparisons use the original saw/pulse mix, +7-cent detune, amplitude envelope
and cutoff sweep, with noise and per-voice drive off. Four fixed settings are:

- classic-lp2: the existing classic filter at its low-pass profile.
- steeper-lp4: two cascaded low-pass SVF sections with identical cutoff and Q.
- bandpass-bp4: two cascaded band-pass sections, each normalized to unity magnitude
  at its centre frequency by multiplying the SVF band output by 1/Q.
- blend-xf4: the sample-wise 50/50 sum of normalized BP4 and the first LP2 section.

This is an original topology choice, not a measured Relic circuit or response.
The XF4 name reflects the source-described BP4/LP2 blend, but normalization and
section design here are ours. Phase is included in that sum; magnitudes are not
simply averaged. Cascaded identical LP sections are not a fourth-order Butterworth
alignment. Q controls both sections; independent bandwidth is not implemented.
Family configuration is fixed before rendering; no live mode-switching contract
or continuous profile extension is implied.

For digital frequency f, define w = tan(pi*f/sr)/tan(pi*cutoff/sr),
D = 1 − w² + j*w/Q, L = 1/D, B = (j*w/Q)/D. The declared transfers are
LP4 = L², BP4 = B², and XF4 = .5*(B² + L).

## Listening

Open outputs/filter-family-study-2/index.html. Twelve 2.7-second playback clips
and twelve raw WAVs compare:

- Single E3, four families, both drives bypassed.
- E3/G3/B3 chord, four families, both drives bypassed.
- The same chord, four families, shared drive 5 and memory .8.

Cutoff sweeps 300 Hz–6 kHz with Q 1.2. The final group uses the plugin's shared
drive defaults, not its complete default patch. Comparing dry and driven contexts
addresses the lesson from the noise review: isolated approval does not establish
approval after nonlinear processing. Each playback file is independently matched
to −24 dBFS RMS; that does not establish equal perceived loudness. Levels and raw
files remain available. No autoplay; starting a player pauses the others.

The earlier study-1 run is retained with only eight dry comparisons. The final
run's verification.json identifies all 24 WAVs and records source/binary hashes.

## Verification

36 fixed impulse cases span 44.1/48/96 kHz, cutoff 600/2200 Hz, Q .70710678/1.2
and the three experimental families. Their 216 complex frequency-response
comparisons agree with the declared equations to a worst absolute complex error
of 7.2173e-15 (acceptance bound 1e-8). DC gains 1/0/.5 and impulse decay pass.
18 additional high-Q edge-cutoff impulse cases remain finite with bounded peak.

36 musical fixture/family/drive/sample-rate cases pass finiteness, release and
exact 512/127 block partition agreement. Invalid family indices are rejected.
All six native CTest checks passed; the filter check was rerun after adding the
driven context and passed again. A separate PowerShell/C# decoder verifies all
24 saved float WAVs, format/length/finiteness, twelve playback files within .001 dB
of the RMS target and peak below .95, audio links and hashes. Browser playback
itself has not been automated.

The native default phrase and installed 0.7 VST3 hashes remain unchanged.
This is ready for listening, not an assertion of musical approval, hardware
matching, full aliasing characterization or realtime performance.
