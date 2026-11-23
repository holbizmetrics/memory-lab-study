# MPE pressure to filter cutoff

Enable **MPE PITCH** on **PARTS** and use **OMNI** for a part that receives
the full lower zone. Open **VOICE → EXPRESSION** and set **MPE PRESSURE**
for the selected part. The depth runs from −2 to +2 octaves. Positive depth
opens the filter, negative closes it, and zero bypasses the route.

Member-channel aftertouch affects held notes on the same MIDI channel and
all their unison voices. A key-up retains its last pressure through sustain
and release. A fresh note on an idle member channel starts at zero pressure.
It can start at another value when pressure arrives before note-on. Manager
channel pressure is outside the dedicated route.

The existing **PRESSURE** depth keeps its channel-pressure behavior. The
two cutoff contributions add if both depths are set. Set **PRESSURE** to
zero to audition the dedicated route on its own. Older patches and projects
open with zero MPE pressure depth and retain their prior settings.

## FL Studio listening check

Use the existing two-note pattern routed through MIDI Out on internal port
20. On channel 2, move the **Chan.Aft** knob while both notes sound. Start
with **MPE PRESSURE** at **+2.00** and **PRESSURE** at **0.00**. The note on
channel 2 should grow brighter while the note on channel 3 holds its tone.
Try **−2.00** to hear the opposite movement, then save and recall the patch
or FL project.
