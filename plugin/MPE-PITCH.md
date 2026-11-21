# Lower-zone pitch expression

Open **PARTS** and enable **MPE PITCH**. Leave **MIDI INPUT** at **OMNI**
for a part that should receive the full zone. The mode applies to the
instance and starts off in INIT and in patches or projects from earlier
versions. It saves in patches and projects. Toggling it clears held notes.

This first stage uses a fixed lower zone. MIDI channel 1 is the manager
with a ±2-semitone wheel. Channels 2–16 are members with ±48 semitones
each. Set the controller to the same zone and ranges. Send the notes on
separate member channels to bend them independently. Channel 1 also accepts
regular note input, including the on-screen keyboard.

The manager pedal and panic messages apply across the zone. Member bend
follows each held note and all its unison voices. When a key lifts, its
sounding tail keeps the member offset while the manager wheel can still move
the zone. A fresh note on an idle member channel starts centered unless a
wheel message arrived there before its note-on. Notes held on the same
member channel share the wheel.

This milestone covers pitch. Dynamic zone and bend-range negotiation,
upper-zone input, and dedicated per-note pressure and timbre routing are
later work. The host must pass the channel and wheel events to the plug-in.

[MIDI Association MPE overview](https://midi.org/midi-polyphonic-expression-mpe-specification-adopted)
