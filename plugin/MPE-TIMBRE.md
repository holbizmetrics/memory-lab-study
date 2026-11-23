# MPE timbre to filter cutoff

Enable **MPE PITCH** on **PARTS**, then go to **FILTER → MOD ROUTES** and
set **MPE TIMBRE** for the selected part. The depth ranges from −2 to +2
octaves. Incoming **CC 74** on a member channel is centered at 64. With
positive depth, sweeping up from 64 opens the filter, and sweeping down
closes it. Negative depth reverses the direction.

Timbre follows the held note and its unison voices. After key-up, the
sounding tail keeps its last timbre value. An idle member channel starts
the next note at center unless it receives CC 74 before note-on. Channel
1 is the lower-zone manager. Its CC 74 has no effect on this member route.

Timbre depth defaults to zero and saves with each part. MPE pressure and
timbre sum with the existing cutoff modulation when their depths are active.

## FL Studio listening check

Use the saved two-note MIDI Out project with port 20. Set **MPE TIMBRE**
to **+2.00**. Set both pressure depths to **0.00** for this comparison.
With the two notes held, sweep the CC 74 knob on MIDI channel 2 through
its range. C4 should go from darker to brighter while G4 stays steady.
Then save and reopen the project and repeat the sweep.
