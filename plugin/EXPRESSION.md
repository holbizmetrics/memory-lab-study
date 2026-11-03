# Voice expression — 0.12

VOICE now has SOUND and EXPRESSION subpages. EXPRESSION adds three controls:

- KEY TRACK (0–100%): scales cutoff with note number around MIDI 60 / C4.
  At 100%, C5 doubles the base cutoff and C3 halves it. At 50%, moving two
  octaves changes cutoff by one octave. At 0%, the base cutoff stays fixed.
  Pitch bend does not change this note-number reference. Existing cutoff limits
  still apply; envelopes, LFO and channel pressure add their modulation normally.
- AMP VELOCITY (0–100%): source gain is `(1 - amount) + amount * velocity`.
  100% retains the original linear MIDI-velocity gain; 0% uses full source gain
  regardless of note velocity. This applies to both oscillators and noise before
  the filter/drives. MIDI note-on velocity zero still acts as note-off in the host.
- FILTER ENV VEL (0–100%): the same response formula controls how velocity scales
  the filter envelope's amount. 100% retains the old response. 0% uses the full
  filter-envelope amount at any note velocity; it does not disable that envelope.
  ENV AMOUNT must be nonzero to hear this control. This is separate from amp gain.

These are smoothed live controls, unlike the envelope stages latched on note-on.
They default to 0%, 100%, 100% respectively, preserving prior behaviour. Existing
channel pressure, envelope amount, base cutoff, resonance and output controls
also appear on the expression page. The preview shows normalized velocity-response
curves; selecting KEY TRACK shows its static cutoff-vs-note curve. These are not
output loudness measurements, particularly with drive enabled.

Host parameters append as `keyTrack`, `ampVelocity`, `filterVelocity` at indices
46–48; the previous 46 IDs/order remain intact. Current files use schema 8, with
49 controls. Schemas 1–7 and old host states restore the neutral expression
defaults even after expression settings were edited in the current instance.
New files require 0.12 or later. INIT restores neutral expression defaults.
Dice rolls preserve all three expression settings regardless of section locks;
its drive-bypass option remains available.

## Suggested combined listening check

Holger explicitly chose to combine the pending 0.11 alpha-2 checkbox check with
0.12's new listening test rather than approve the checkbox before further work.

1. For a clean comparison, use both drives at zero. On EXPRESSION set ENV AMOUNT
   to zero and KEY TRACK to zero. Compare soft/hard notes with AMP VELOCITY at
   100%, then 0%. At 0%, source level should no longer follow note velocity.
2. Keep AMP VELOCITY at 0% to isolate brightness. Set ENV AMOUNT around 1.5
   octaves and base cutoff near 1000 Hz. Compare soft/hard notes with FILTER ENV
   VEL at 100%, then 0%. At 0%, the envelope amount should be independent of velocity.
3. Set ENV AMOUNT back to zero. Compare C3/C4/C5 with KEY TRACK at 0%, then 100%.
   At 100%, cutoff halves/doubles around C4 instead of staying fixed.
4. On DICE, check BYPASS DRIVES and roll. Both drive controls should read zero;
   expression settings should remain unchanged. Undo should restore the prior
   sound, including any previous drive values. Checking alone is passive.
5. Save/load a new patch and check all expression values return. Load an older
   clocked patch: new expression values should reset to 0/100/100, preserving
   the old response. Save under a new filename if keeping an old-version copy.

Musical approval of 0.12 and live confirmation of the bypass checkbox remain
pending. A full hardware Relic modulation/MPE architecture is not claimed.
