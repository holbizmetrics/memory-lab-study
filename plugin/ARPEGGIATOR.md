# Arpeggiator development

## Version 0.4 controls and behavior

ARP is off by default, including when recalling a 0.3 or older patch. Existing
parameter and plug-in IDs remain unchanged. The seven new host parameters are
arpOn, arpSync, arpBpm, arpRate, arpOrder, arpOctaves and arpGate. All are saved
with the patch; held notes, pedal state and clock phase are transient.

The ARP page has eight controls: on/off, host tempo, internal BPM, rate, order,
octaves, gate and the existing base cutoff. The display shows a sixteen-step
rhythm/gate preview, not a pitch sequencer. The step highlight advances during
audio processing. Internal tempo is used when host sync is off or valid host
tempo is unavailable. Rates are 1/4, 1/8, 1/16, 1/32, 1/8 triplet and 1/16 triplet.
Orders are Up, Down and Up/Down without repeated endpoints, over 1–3 octave
layers. Within a layer, notes are ordered by pitch then MIDI channel. Notes above
MIDI 127 are omitted. Original channel and velocity are preserved.

The first held note starts immediately. Additional notes enter the next step;
releasing the last held note releases the current output immediately. CC64 holds
the input chord, without sustaining every generated output note. Channel all-off
commands clear only that channel. Switching ARP on/off clears sounding notes and
pedals; re-press notes after switching. Transport start restarts the pattern;
transport stop clears held arp notes. Playing while the host is stopped remains
available for auditioning. Host sync follows BPM; this first implementation does
not quantize the first note to the host PPQ grid or track timeline seek/loop
boundaries. Tempo, rate and gate edits affect subsequent steps.

Gate is 5–95% of step length; envelope release is additional audible time.
Pitch bend and channel pressure still reach the voice engine. The arp generates
internal voice events, not MIDI output. Latch, swing, random order and a step
sequencer are not included in this first version.

Try: turn ARP on, choose 1/16, Up and 2 octaves, gate 40%, then hold an E-minor
chord. For your existing piano-roll pattern, leave ARP off. Save the FL project
before installing updates. Version 0.3 is retained as a backup during installation.

This is original behavior. The supplied panel references show an arpeggiator
section, but the manufacturer's public specs do not establish its detailed modes:
https://www.shearelectronics.com/relic/specs/

Historical clarification (2026-10-29): the creator's NAMM 2017 interview describes
a 64-step polyphonic sequencer and distinguishes it from an arpeggiator. That
interview does not confirm a native hardware arp. See
[interview evidence by generation](../RELIC-INTERVIEW-EVIDENCE.md).

## Verification

The native tests cover order and octave range, fractional sample timing, gate
length, exact block partition agreement across 54 mode/rate/sample-rate cases,
sustain, channel isolation, high-note limits and tested render allocation counts.
Wrapper tests cover audio partition parity, host/internal/fallback BPM,
transport stop, disabling the arp, state recall and legacy-preset bypass.
Editor and actual-module checks are required before installation.

## User listening reference, 2026-10-28

Holger confirmed enjoying the installed 0.3 instrument in FL Studio. Screenshots
show the following patch values; this is a partial transcription, not a complete
host preset (the filter envelope page and other state were not shown).

- Voice: detune 0 cents, pulse width .698, cutoff 381 Hz, Q 2.72,
  profile .736, drive 1.4, memory .58, recovery 145 ms, output .250,
  oscillator phase reset on.
- Motion: rate 7.13 Hz, vibrato -18 cents, pulse motion .072,
  filter motion .74 octaves, filter envelope amount 1.28 octaves,
  pressure depth -2 octaves.
- Amplitude: delay 0 ms, attack 0 ms, hold 2 ms, decay 57 ms,
  sustain .394, release 23 ms.

The Voice screenshot preceded the Motion screenshot and shows memory .80 /
recovery 250 ms; the later Motion and Amplitude screenshots show .58 / 145 ms.
Save the FL project to retain the complete patch.
