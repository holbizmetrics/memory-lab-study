# Stereo spread

Open **VOICE → EXPRESSION** and use **STEREO SPREAD**, the last knob.
Each part has its own setting. At 0% all its voices are centered. At 100%
the notes from a chord occupy different positions across the stereo output.
Changing spread also works on held notes.

The voice's position follows its allocated slot through sustain and release.
The two sides apply shared drive separately when spread is enabled. With
shared drive off, averaging the stereo channels recovers the centered signal.
With shared drive on, the stereo fold may sound different because saturation
acts on different mixes. A mono host bus always uses the original centered
mix. The plugin still has its original main output and eight-voice budget.

For a first test, load Clean Foundation, use a stereo mixer channel and
hold a simple chord as you compare 0% and 100% on headphones.

The new four depth parameters append after 501 existing controls. Older
projects and patches load at 0%. INIT and starter sounds reset spread,
Copy Part includes it, and Dice preserves it.
