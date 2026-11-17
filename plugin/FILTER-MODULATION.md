# Filter modulation — 0.23

FILTER / MOD ROUTES adds four signed depths per part:

| Control | Source | Destination and units |
| --- | --- | --- |
| ENV 3 PROFILE | ENV 3, including its curve/timing | Offset in normalized profile units, -1…+1 |
| LFO 1 PROFILE | LFO 1, including polarity/shape/clock | Offset in normalized profile units, -1…+1 |
| ENV 3 RES | ENV 3 | Additive Q offset, -7.5…+7.5 |
| LFO 1 RES | LFO 1 | Additive Q offset, -7.5…+7.5 |

Each voice sums the two contributions onto its base profile or resonance.
Resulting profile clamps to 0…1 and Q to 0.5…8. Positive ENV depth increases
the value as the envelope rises; negative depth decreases it. Bipolar LFO
motion goes both ways, while a unipolar source moves one way for a signed
depth. At a bound the motion clips, so a positive profile depth at base 1
will not rise further. These are explicit software laws, not documented
hardware scaling or compensation curves.

Profile routing uses the active Classic or Modern profile. Legacy LP4, BP4
and Blend are not continuous-profile modes: with Modern off, their profile
routes are inactive and greyed out. Resonance routes operate in every family.
The new page exposes both base profiles, base Q and base cutoff. Existing
cutoff modulation is independent; no old source/destination is repurposed.

LFO 1 settings remain under MOTION / LFO 1 and CLOCK 1; ENV 3 settings remain
under ENVELOPES / ENV 3. ENV 3 has no implicit primary-filter-envelope velocity
scaling. It shares the existing voice lifetime and sustain/release behavior.
This milestone adds explicit routes from two sources, not a general matrix
or modulation-of-modulators feature.

## Sound and display behavior

Depth controls use the existing 20 ms smoothing time constant. Intentional
steps in pulse/saw sources remain steps; no extra waveform smoothing is added.
Changing these depths affects held notes. ENV 3 stage/curve/timing edits still
latch on new notes. Filter states are not reset by the new modulation.

The graph shows the BASE response, not an animated or selected-voice response.
It is explicitly labelled that way on MOD ROUTES. The audible filters can move
away from that curve independently on every voice.

Q bounds are not automatic loudness compensation. High resonance can strongly
boost signal, particularly through existing legacy cascaded filters, and can
drive intentional saturation. Use moderate Q/depth and lower output when needed.
Nothing silently changes the old filter gain law or inserts a limiter.

## Compatibility and persistence

All depths default to zero. Zero amounts bypass new arithmetic; off-target
amounts settle to exact zero after smoothing. First 457 host IDs/order/ranges
are unchanged. Sixteen fields append in part-major order: `filterModEnvProfile`,
`filterModLfoProfile`, `filterModEnvRes`, `filterModLfoRes`, with existing
`partN__` prefixes for additional parts. Total 473 parameters.

Patch schema 18 / host state 20. Older sounds restore inactive routes even in
an edited instance, preserving prior envelope and polarity choices. INIT and
selected-part starter loading reset routes. Part copying includes them. Dice
preserves route depths and keeps its previous randomization scope; undo stores
the full setup. New-format files require a newer plugin.

## Evidence and checks

The manufacturer documents voltage-controllable profile, and the supplied
later interview/research includes ENV 3-to-profile and resonance-destination
statements. See [hardware audit](../RELIC-COVERAGE-AUDIT.md) for provenance and
unverified timestamp leads. No complete hardware matrix or depth law is claimed.

Checks cover independent sign/sum/clamp references, default bypass, appended
IDs, old/new state and patch recall, part copy/starter/Dice isolation, isolated
Classic/Modern routes, legacy-family bypass and Q modulation, four-rate held
extreme depth edits, multipart block parity and UI enabled/binding states.
Actual-module captures separately check old sounds, new route examples and
combined parts. Musical approval, realtime/aliasing review and hardware
sound equivalence remain separate from those checks.
