# 0.18 first oscillator step: complementary waveforms

OSC now has TUNING (the existing controls) and MIX. MIX provides OSC 1 SAW,
OSC 1 PULSE, OSC 2 SAW, OSC 2 PULSE, OSC 1 WIDTH, OSC 2 WIDTH, noise and output.
Five source-level bars show the four waveform contributions and noise.

The previous oscillator-1 saw and oscillator-2 pulse remain unchanged. The new
oscillator-1 pulse shares oscillator 1's phase/pitch core; the new oscillator-2
saw shares oscillator 2's phase/pitch core and follows its existing interval and
detune. These are complementary waveforms from two cores, not four oscillators.
Both waveforms from a core may be mixed. More sources can increase level; no
automatic normalisation changes existing sounds. Use output to compensate.

OSC 1 WIDTH is independently adjustable from 0.1 to 0.9 and defaults to 0.5.
It is static in this step. Existing LFO/envelope pulse-width routes continue to
affect oscillator 2 only. Hard sync, independently targeted pitch modulation
and measured SEM circuit matching are not implemented here. Both waveforms
reuse the existing band-limited wavetable bank and existing pitch clamps.

Three host parameters append after the existing 100 IDs: osc1PulseLevel,
osc2SawLevel and osc1Width (103 total). Added levels default zero; inactive
sources skip their sample arithmetic. Controls smooth over 20 ms. Existing
muted sources should settle before a fresh-note isolation test, rather than
interpreting their short fade-out as a failed mute.

Patch schema 13 stores all controls; schemas 1-12 migrate with new levels off
and oscillator-1 width at 0.5. Host state version is 15. New-format patches need
a compatible newer plugin. Starter sounds/INIT reset the new sources off;
Dice protects the new fields and retains its approved scope. Existing IDs,
host parameter names/order and original source levels are unchanged.

## Evidence and scope boundary

Reviewed 2026-11-08: the manufacturer's [specifications](https://www.shearelectronics.com/relic/specs/)
and [voice-core page](https://www.shearelectronics.com/relic/voice-core/) describe
two SEM-topology VCOs per voice. The supplied 2017 creator interview describes
saw/pulse and oscillator sync; see [generation-specific evidence](../RELIC-INTERVIEW-EVIDENCE.md).
The current official text does not specify this exact four-level software mixer
or its gain calibration. This expands our software's oscillator capabilities,
not a claim to have reproduced the hardware circuit or every control.

Modern filter gradient/profile/bandwidth work remains a separate follow-up in
the 0.18 series. Recipe copy/paste remains a later software extra, as recorded
in the roadmap. Neither is silently counted as completed by this oscillator step.

## Listening guide

Load Clean Foundation. OSC / MIX: set OSC 1 SAW and OSC 2 PULSE to zero, then
turn up OSC 1 PULSE. Compare OSC 1 WIDTH at 0.2 and 0.5 on fresh held notes.
Mute it and try OSC 2 SAW alone. OSC / TUNING / OSC 2 INTERVAL should transpose
that saw; it should not transpose oscillator 1's pulse. Then blend the four
sources at modest levels. Save/reload and verify project recall separately.
Holger confirmed oscillator-1 width tone differences and oscillator-2 saw
octave-up/back tuning. Blend patch recall failed in alpha.1: a valid 0.1 width
was rejected after decimal formatting. Alpha.2 fixes endpoint validation;
Holger then confirmed both original-file recall with the correct sound and FL
project close/reopen recall in alpha.2. These are the tested listening/recall
checks; no hardware equivalence is inferred.

## Validation

All 15 plugin suites pass (100.05 s), including the corrected settled-source
isolation fixture; all six native suites pass (41.97 s). Tests cover new solo
and combined waveforms, width/tuning independence, four sample rates, block
parity, mute/release, held eight-note edits/all-sound-off, schema-12 migration,
host recall, 103 stable parameter bindings and 384 passive page changes.
Eight actual VST3 fixtures pass, maximum fixture peak 0.366584. Default,
clocked, active-envelope, active-routing and local saved two-LFO recordings
remain byte-identical. Actual VST3 scan/load/audio/state/release/reload passed;
PE/plugin versions report 0.18.0. Normal/compact mixer captures were inspected.
These are technical checks, not hardware equivalence or a realtime xrun test.
