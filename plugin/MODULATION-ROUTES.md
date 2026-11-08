# 0.17: additional modulation routes and optional preset auditioning

Eight new depths append after the existing 92 host parameters (100 total).
All default to zero; the existing IDs/order, default sound, source clocks and
envelope stage behaviour remain unchanged. Patch schema 12 stores the depths;
schemas 1-11 and legacy host state reset missing routes to zero. Host state is
version 14. New patch files require a compatible newer plugin.

## Controls and sound

- MOTION / LFO 1-4: AMP DEPTH adds per-voice tremolo after the filter and before
  voice drive. It affects filtered oscillators/noise together. Depth 0 is exact
  bypass; absolute depth 1 sweeps gain from zero to one. Negative depth reverses
  modulation phase. This is attenuation only, not gain boost or pitch vibrato.
  Multiple active amplitude routes multiply their gains, so combining them can
  become quieter. Existing rate/shape/sync/retrigger settings drive this route.
- ENVELOPES / ROUTES: AMP ENV PITCH and FILTER ENV PITCH route those existing
  contours to both oscillators, up to +/-1200 cents. These extra pitch routes
  use the source contour itself, without the existing filter-velocity scaling.
- ENVELOPES / ROUTES: ENV 3 PULSE and ENV 4 PULSE add their contours to pulse
  width, up to +/-0.35, before the existing 0.1-0.9 width clamp. An audible pulse
  oscillator level is needed. The same page repeats ENV 3/4 pitch/filter depths
  for convenience; those repeated controls are not extra host parameters.

The new depths smooth over 20 ms. Old zero routes skip their arithmetic; source
stage settings still latch on note-on. Existing voice lifetime remains governed
by the amplitude contour. DICE protects the added depths and keeps its approved
scope; INIT and starting sounds reset them to zero. This is an explicit routing
expansion, not a general matrix or a claim to reproduce all hardware routing.

## SOUNDS: Auto-load on selection

The checkbox is off by default. Off keeps passive browsing plus LOAD SOUND.
On loads a newly selected sound through the same existing load function:
output stays as set, both drives load bypassed, and UNDO LOAD restores the full
sound before the latest load. The existing undo invalidation rules still apply.

Opening SOUNDS, enabling the checkbox and changing categories never load a
sound. Category changes can highlight a new first item without applying it;
LOAD SOUND remains available for that highlighted item. Choosing another item
with auto-load enabled applies it immediately. Save edits before auditioning.

The checkbox is a local browsing preference in the same settings file as the
patch folder, not a DAW sound parameter or patch/project field. It is restored
when a panel opens; other already-open instances need not follow the checkbox
live. Tests use a private settings file and do not alter Holger's preferences.

## Musical status

Holger confirmed 0.16's ENV 3 pitch contour and ENV 4 cutoff contour, identified
the brightness movement with the pitch route zeroed, and confirmed patch recall
after Clean Foundation plus FL-project recall after a complete restart. New
0.17 destinations and auto-load interaction still need their own listening/UI
confirmation. Preserve that distinction when reporting validation.

## Holger's completed 0.17 checks

Holger confirmed auto-load on sound selection and restored manual loading when
disabled. LFO 1 AMP DEPTH at 2 Hz produced two volume pulses per second; zero
removed them. ENV 3 pulse-width movement was audible with a steady pitch and
disappeared at zero depth. Filter-envelope pitch and amplitude-envelope pitch
routes produced their expected rises, with zero removing each pitch movement;
the amplitude-envelope pitch route also fell during audible release as expected.
He saved the active amplitude-envelope pitch route, loaded Clean Foundation and
restored it from `Envelope Pitch Sweep.mlpatch`. After saving the FL project,
closing FL completely and reopening it, he confirmed the sound and remembered
auto-load checkbox setting. These approve the tested routes and recall paths,
not every source, polarity, combination or automation scenario.

## Automated validation results

Windows VST3 and standalone builds pass. All 14 plugin CTest checks pass
(100.99 s), including 100 stable IDs/order/bindings and gestures, 352 passive
page changes, all old patch migrations, Dice protection and isolated editor
lifecycle checks. All six native suites pass (36.81 s). Routing tests isolate
all eight new destinations with both signs at 44.1/48/96/192 kHz, test combined
sources, exact block partitioning, release and old/new patch/host recall.
Auto-load tests cover default off, no load on enable/category/panel open,
selection loading, output/drive policy, most-recent-load Undo, disabling and
preference persistence with the patch-folder preference retained.

Nineteen actual VST3 route fixtures pass, peak 0.371115 at their fixture output
setting. Default, clocked, active 0.16 envelopes and the local saved two-LFO
sound render byte-identically to preserved recordings. Actual VST3 scan,
classification, MIDI/arp audio, state/release/reload pass at 44.1 kHz. PE and
plugin versions report 0.17.0. New normal/compact editor captures were inspected.
These checks do not certify every routing extreme, realtime dropout behaviour,
full live FL automation/undo, or measured hardware equivalence.
