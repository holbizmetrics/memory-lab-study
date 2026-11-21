# Amp position

**DRIVE → AMP POSITION** sets the voice path for the selected part.

| Setting | Order within one voice |
| --- | --- |
| Legacy | Oscillator/noise mix → amp envelope and velocity → filter → voice drive |
| Post-filter | Oscillator/noise mix → filter → amp envelope and velocity → voice drive |

Each note takes the selected path when triggered and keeps it through release.
The existing filter-envelope and modulation routes still apply. The per-voice
drive remains after the filter and the shared drive remains after voices sum.
Adjust the right-hand **OUTPUT** fader to compare at similar loudness.

Try a single saw, a low cutoff with moderate resonance, and a short amp decay
with low sustain. Play a fresh note in each mode. Compare how the attack and
filter ring develop. Start with both drives at zero, then raise **VOICE DRIVE**
and listen again. A sustained portion can sound very similar after filter
state has settled.

Older patches load in Legacy. The four mode parameters append after the 497
existing controls. INIT and starter sounds reset the mode, copying a part
includes it, and Dice preserves it.
