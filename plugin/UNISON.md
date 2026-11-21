# Per-part unison

Open **Voice → Expression** for the part you want to edit.

- **Unison** stacks 1, 2 or 4 voices on each note. Memory Lab shares eight
  voices across all parts. Each stack fits inside its part's reservation.
  A part with two reserved voices plays a two-voice stack when set to 4.
- **Unison Detune** sets the distance, in cents, from the played pitch to
  each outer voice. The inner voices in a four-voice stack divide that span
  evenly. Detune updates held stacks. The factory value is 12 cents and
  one voice plays at the unchanged pitch.
- **Stereo Spread** can place the stack members across the stereo output.
  Set it to zero to hear the detuned stack centered.

Changing the count stops notes in the edited part. Play a fresh note after
each count change. A part with all eight voices can play up to eight notes at
1, four notes at 2, or two notes at 4. The allocator retriggers, releases and
steals stacks together. Each member is scaled by the reciprocal of the stack
size before mixing into the part's shared drive.

Both settings save with the part in patches and projects. Earlier patches and
host states open at one voice while retaining their saved stereo spread.
