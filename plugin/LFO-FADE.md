# Envelope-controlled LFO 1 depth

Open **MOTION → LFO 1** and set **ENV 3 FADE** for the selected part.
The control scales every LFO 1 destination, including vibrato, oscillator
pulse width, cutoff, filter profile, resonance, and tremolo. The phase
and speed continue while the depth changes.

- **0%** gives the existing LFO 1 depth throughout the note.
- **100%** follows ENV 3, with zero LFO depth at envelope zero and full
  depth at envelope one.
- **50%** starts at half the programmed LFO depth when ENV 3 is zero
  and reaches full depth when ENV 3 reaches one.

For a first listening test, load **Clean Foundation**, open
**ENVELOPES → ENV 3**, and set **Delay = 300 ms**, **Attack = 1,500 ms**,
**Sustain = 1.000**, and **Time Scale = 1.00**. Use the **Legacy** curve
for a linear rise. In **MOTION → LFO 1**, set **LFO Rate = 5 Hz**,
**Vibrato = 40 cents**, and **ENV 3 Fade = 100%**. Hold a fresh note for
three seconds. It should start steady, then gradually develop vibrato.
Set **ENV 3 Fade = 0%** and play the note again to hear immediate vibrato.

Older patches and projects open at 0%. The setting saves with the part.
LFO 2–4 and ENV 3's existing direct destinations keep their behavior.
