# Continuous modern filter — 0.18 alpha.3

FILTER has FAMILIES and MODERN pages. The original four choices retain their
parameter range, processing and stored values. MODERN ON replaces that path
with the new filter. Off restores it; navigation never changes the active mode.

Three appended controls (106 total): modernEnabled (Off/On, default Off),
modernProfile (0–1, default 0.5), modernBandwidth (0.25–4 octaves, default 1).

Profile anchors: 0 BP4, 0.25 XF4, 0.5 LP2, 0.75 LP3, 1 LP4. Adjacent outputs
blend continuously. XF4 is half BP4 plus half LP2. Bandwidth separates BP4
corner frequencies; it affects BP4/XF4 and neighboring blends, but is inactive
from LP2 onward. Existing cutoff, resonance, tracking and envelope/LFO cutoff
routes apply. No profile/BW modulation route is added in this checkpoint.
Existing saw/pulse/noise all pass through it.

On/off crossfades and profile/BW changes smooth over 20 ms. New filter states
reset on voice allocation; the modern path does no filter work when fully
bypassed. Existing families keep running. The display shows the selected static
response, not transient crossfades, modulation or drive. Inactive controls dim.

The low-pass cascade avoids applying full resonance to both sections, so LP4
does not square Q. This is not automatic loudness normalization: resonance,
blends, source levels, voice count and intentional drives affect level.
No new limiter, compressor or noise routing changes older sounds.

Compatibility: prior 103 IDs/names/order retained. Schema 14 / host state 16
store additions; schemas 1–13 and old host states migrate with Modern off,
profile 0.5 and bandwidth 1. INIT/starter sounds use those defaults. DICE
preserves these three fields. Old plugin versions do not support schema 14.
The alpha.2 patch endpoint fix remains.

Validation: 16/16 plugin and 7/7 native suites. Independent complex reference
matches impulse/display at four rates and Q/BW extremes. Integration covers
five anchors, BW extremes, Q8/eight voices with drive off/on, block parity
through edits/bypass, all-off, migration, recall, bindings and passive navigation.
Eight-voice fixture peak 0.343848 at output 0.03 is not universal headroom.
Ten actual VST3 profile/drive fixtures peak 0.163812 at their settings.
Sixteen legacy family/drive/noise captures match installed alpha.2 byte-for-byte;
eight waveform fixtures and five prior default/motion/envelope/routing/saved
recordings remain unchanged. Original Four Wave Blend passes file-loader recall.
Actual module scan/load/audio/state/release/reload and normal/compact UI pass.

Holger confirmed Modern on/off, the profile sweep, audible/visible bandwidth
change and inactive controls. He saved Modern LP2 Test with cutoff 300 Hz,
then confirmed patch reload and FL project close/reopen restore its sound.
These checks do not establish every setting or a hardware match. This is a behavioral
approximation, not circuit-exact matching. [Derivation and limits](../native/MODERN-FILTER-RESEARCH.md).
Full realtime CPU/dropout, aliasing and FL automation/undo coverage remain open.

## First listening check

Load Clean Foundation. FILTER → MODERN: MODERN ON = ON, BASE CUTOFF = 1800 Hz,
RESONANCE = 0.9, OUTPUT = 0.15. Hold a low note and sweep MODERN PROFILE from
0 to 1. Separately, return profile to 0, then compare bandwidth 0.25 and 4.
Both drives stay off for the first check. Save/reload and project recall follow.
