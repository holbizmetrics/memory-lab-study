# Four independent LFOs — 0.15

MOTION now offers LFO 1 / CLOCK 1 through LFO 4 / CLOCK 4. Each source has its
own free rate, sine/triangle/saw/pulse shape, pitch/PWM/cutoff depths, sync,
division, note retrigger/free run and fallback BPM. Existing first/second-source
controls retain their IDs, ranges and behaviour. LFO 3 defaults to 0.35 Hz and
LFO 4 to 1.1 Hz; all their modulation depths default to zero.

This closes the independent-LFO-count gap, not the complete hardware LFO spec:
four shapes per source, fixed pitch/PWM/cutoff destinations and 0.05-20 Hz free
rate remain the current scope. Further shapes/ranges/routing need their own
evidence and verification. No claim of faithful Relic waveform behaviour.

Pitch contributions sum in cents, width contributions sum before the existing
0.1-0.9 clamp, and filter contributions sum in octaves before cutoff clamps.
Continuous rates/depths use existing 20 ms smoothing; zero depths settle to exact
bypass. Shape/clock changes are discrete and can intentionally change phase or
modulation abruptly. Independent note phases restart on note-on when selected;
free-running phases advance during processing, including silence. Prepare resets
them; transport seek does not align them to song position. Phases aren't saved.

With valid host BPM, synced sources share host tempo but have independent
divisions. With no valid host BPM they each use their own fallback. Changing
ARP sync does not change any LFO's timing policy. Output and existing amplitude
envelope/voice allocation are unchanged.

## Compatibility and workflow

76 total host parameters: existing 58 IDs/order remain stable. LFO 3's nine
fields append at 58-66, LFO 4 at 67-75, in Rate/Pitch/Pwm/Cutoff/Shape/Sync/
Division/Retrigger/Bpm order. Patch schema 10 stores all controls; new files
require 0.15 or later. Schemas 1-9 and legacy host states restore inactive
LFO 3/4 defaults, including after edits in the same instance. A schema-9 patch
keeps its active LFO 1/2 settings. Host-state version 12.

INIT and starting sounds reset added sources inactive. DICE retains its approved
scope: oscillator/filter/envelope sections plus LFO 1 motion, with output/ARP
and LFO 2-4 settings preserved. No new randomisation options. The ARP continues
its existing pattern when you roll a new sound. Existing drive-bypass and Undo
behaviour is unchanged.

Holger approved all eight starting sounds and LFO 2's three routes, combined
first/second-source movement, sync, retrigger/free run and patch recall, including
after a Dice roll. Round Bass was confirmed around C3; the high-register concern
was resolved by playing lower. `Slow Wah, Fast Heart.mlpatch` was saved by Holger
and a read-only copy is used as a 0.14-versus-0.15 actual-module preservation
fixture. His original file is not changed or published.

## Suggested next listening round

1. SOUNDS / Clean Foundation / LOAD SOUND, output about 0.25. On LFO 3 set
   FILTER to 1 octave and RATE to 0.35 Hz; hold a note. Set FILTER to zero to
   isolate/bypass it. Repeat pitch and pulse routes separately as with LFO 2.
2. Repeat on LFO 4 at a visibly different rate. Confirm changing LFO 4's rate
   doesn't change the values or timing of LFO 3.
3. Combine LFO 1 fast pitch, LFO 2 slow filter, LFO 3 pulse movement and LFO 4
   another gentle filter rate. Zero each source's depth in turn to isolate it.
4. CLOCK 3/4: sync/divisions and retrigger/free run should behave like CLOCK 2,
   independently. Synced mode dims that source's free-rate knob.
5. Save/reload an active four-source patch. Load Slow Wah, Fast Heart: the first
   two LFOs should restore, while LFO 3/4 depths return to zero. Save under a new
   filename when keeping an old-version copy readable by 0.14.
6. Dice should leave ARP and all LFO 2-4 controls untouched.

Technical validation and Holger's musical approval of LFO 3/4 remain separate.
