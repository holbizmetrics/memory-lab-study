# Parts — 0.19 alpha.1

PARTS, beside RESET OSC PHASE, opens allocation and part selection. Four parts
can hold independent sound settings while sharing eight active voices total.

| Allocation | Active parts | Voice reservation |
|---|---|---|
| Single | Part 1 | 8 |
| 2-layer | Parts 1–2, overlapping keyboard | 4 each |
| 2-way split | Part 1 below split note, Part 2 at/above it | 4 each |
| 4-layer | Parts 1–4, overlapping keyboard | 2 each |

SPLIT is a MIDI note number, not an octave-name convention. Each part receives
OMNI or one of MIDI channels 1–16. EDIT PART opens its sound pages; the header
shows the selection. Inactive parts can be edited in advance. Selection alone
does not change routing or sound. COPY P1 HERE copies Part 1's 99 sound controls
to that row without changing routing or global arp settings.

VOICE, OSC, MOTION, ENVELOPES, FILTER, DRIVE, the output fader and phase-reset
switch address the selected part. SOUNDS and DICE also address it; loading a
starter sound preserves the selected part's output level and bypasses its drives.
Part-1 starter loads retain the previous global-arp preset behavior. Loads into
Parts 2–4 leave the global arp unchanged. Undo restores the most recent full
pre-action snapshot and is invalidated by intervening sound changes.

ARP is one global pattern feeding the part routing; there are not four arps.
Per-part LFO rates, divisions and fallback tempos are independent, with shared
host BPM when available. Part selection is local editor-session state, not a
host parameter or a saved sound choice. SAVE/LOAD store the whole setup, not
just the selected part. INIT resets every part, routing and the editor selection.

The first 106 host IDs/names/order remain unchanged. Each of Parts 2–4 appends
99 sound parameters, followed by allocation, split and four channel parameters:
409 total = 396 part controls + 7 global arp controls + 6 routing controls.
Added sound IDs use part2__, part3__ and part4__ prefixes. Patch schema 15 and
host state 17 save the setup. Old patches and states migrate to Single/Part 1,
split note 60, omni channels and inactive additional parts with compatible
defaults. Older plugins cannot load new schema-15 files. The endpoint fix stays.

Routing changes silence held notes and reset the global arp to avoid stranded
voices. This first version uses fixed reservations, with no borrowing, custom
quotas, four-way split, velocity zones, per-part transpose or separate outputs.
Each part owns its memory drive and volume; there is no new master drive or
master fader. The meter shows the summed output. Layer levels add, so keep part
outputs modest. Newly activated parts prime their stored controls before new
notes; existing sound changes continue to smooth normally. Extra-part noise
streams use deterministic distinct seeds. Parts 2–4 default with drives off.

Validation: 17/17 plugin and 9/9 native suites. Tests cover 409 stable IDs,
old/new patch and project-state migration, selected-part preset/dice/undo/copy
and edit isolation, passive navigation, complete INIT, four-rate layer/split
channel routing, panic and reprepare behavior. Fixture peak is 0.122648 at
its test settings, not universal headroom. Eight actual VST3 single/layer/split
fixtures pass (peak 0.069244 at their settings). Twenty-one legacy and modern
filter captures match installed 0.18 alpha.3 byte-for-byte. Eight oscillator
fixtures and five earlier default/clock/envelope/routing/saved recordings also
match. Actual scan/load/audio/state/release/reload and original Four Wave Blend
file loading pass. Normal/compact Parts and selected-Part-2 editor inspected.
Holger confirmed the two-sound layer, keyboard split, INIT/patch reload, FL
project recall, four-layer 4/8 voice use and MIDI-channel isolation in both
directions. Complete live FL automation/undo and realtime dropout checks
remain open. This implements documented architectural ideas, not measured
Relic hardware equivalence. Local installation is recorded separately.

## First listening round

Use a fresh instance for the expanded host parameter table. Keep Single while
preparing sounds: EDIT PART 1, load Round Bass and set its OUTPUT to 0.10.
EDIT PART 2, load Slow Air and set OUTPUT to 0.10. Choose 2-layer, play a held
note/chord and hear both the immediate bass and slower pad. Then try 2-way split
at MIDI 60, comparing notes below and above the boundary. Check other parts do
not change while editing one. Save/reload the complete setup and separately
check FL project recall. Four-layer and channel routing follow as distinct tests.
