# Independent second LFO — 0.14

MOTION now has LFO 1 / CLOCK 1 / LFO 2 / CLOCK 2 subpages. The first two keep
the existing controls and IDs. LFO 2 is a second per-voice source with independent
rate (0.05-20 Hz), sine/triangle/saw/pulse shape, pitch (-50..50 cents), pulse-width
depth (-0.35..0.35) and cutoff depth (-2..2 octaves). All three depths default zero.

Sources combine at their destinations: pitch offsets add in cents, pulse-width
offsets add before the existing 0.1-0.9 width clamp, filter offsets add in octaves
before the existing cutoff clamp. This is not amplitude modulation or a full
modulation matrix. Intentional saw/pulse discontinuities remain. Both sources
share the voice's note/channel/velocity but have independent phase accumulators.

CLOCK 2 offers tempo sync, six rhythmic divisions, note retrigger and independent
fallback BPM. With valid host BPM both synced sources use that host tempo; their
divisions remain independent. Otherwise each uses its own fallback. Sync controls
speed, not bar/song-position phase. Retrigger ON starts the source at phase zero
per note; OFF joins its own global phase, which advances while audio processing
continues through silence. Prepare resets phases; phases are not serialized or
reset on transport seek. Oscillator phase reset is a separate control.

New controls append at host indices 49-57: lfo2Rate, lfo2Pitch, lfo2Pwm,
lfo2Cutoff, lfo2Shape, lfo2Sync, lfo2Division, lfo2Retrigger, lfo2Bpm. All earlier
49 IDs/order remain stable. Patch schema 9 stores 58 controls. Schemas 1-8 and
old host states restore defaults for all nine fields, including after LFO 2 edits
in the same instance. New patches require 0.14 or later. Continuous rate/depths
use 20 ms smoothing; shape/sync/retrigger choices are discrete. Depths returning
to zero settle to exact bypass. Default and legacy sound preservation is checked
with real VST3 captures, not inferred from inactive defaults alone.

INIT resets both LFOs. Starting sounds initialize LFO 2 inactive. Dice retains
every LFO 2 control regardless of LOCK MOTION; its motion roll still varies LFO 1.
This deliberate boundary keeps LFO 2 under your control during this first test.
Current rate/shape can change a preview while zero depths leave sound unchanged.

## Holger's next combined check — stop here before four LFOs

Save any sound you want to retain. In SOUNDS load Clean Foundation, which bypasses
both drives; set output to a comfortable level. Play fresh notes after loading.

1. MOTION / LFO 2: set LFO 2 FILTER to about 1 octave and LFO 2 RATE to 0.7 Hz.
   Hold a note/chord: the filter should open/close slowly. Try 3 Hz, then return
   the depth to zero: the motion should disappear. Try pitch and PWM separately
   (pulse level must be nonzero for PWM).
2. Put LFO 2 FILTER back at 1 octave and rate at 0.7 Hz. On LFO 1 set VIBRATO
   around 8 cents and LFO RATE around 4 Hz. Hold a note: slow filter movement
   and quicker pitch movement should coexist. Turn either depth to zero to
   hear the other source alone. This is the independent-source check.
3. CLOCK 2: turn sync on and compare divisions; the free-rate knob dims. Note
   retrigger versus free run should restart versus continue the second source's
   phase across fresh notes. LFO 1 timing should remain as you set it.
4. Save a .mlpatch with active LFO 2, change its depths, then reload and compare.
   Load an older patch: LFO 2 should return inactive. Keep old files under their
   old names if you want versions readable by older builds.
5. Test 0.13's SOUNDS browsing/load/Undo and a few starting sounds as described
   in PRESETS.md. Both preset and LFO 2 musical approval remain pending until
   Holger reports them; automated checks do not substitute for that report.

The next four-LFO expansion is explicitly held until Holger tests this milestone.
