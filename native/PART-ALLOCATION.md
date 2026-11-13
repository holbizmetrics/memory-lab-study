# Four-part allocation foundation

Policy code only: part-allocator.hpp is not connected to the installed plugin.
No new sound, host parameter, patch schema or numbered synth version is delivered.

Four parts reserve slots from one eight-slot budget. The default reserves all
eight for part 1; parts 2–4 reserve zero. A split can reserve four below middle
C and four above it. A four-layer setup can reserve two per part, so each
keyboard note can consume four slots and only two such notes fit without stealing.
This avoids accidentally making four independent eight-voice instruments.

Each part has an inclusive low/high note range and an omni or specific MIDI
channel (zero-based internally). Overlapping ranges form layers. Reserved slots
are not borrowed between parts. This is our initial policy, not a claim that
the hardware uses exactly this stealing or reservation algorithm.

Note-on routing prefers an existing same-note/channel voice, then a free slot,
then the quietest released voice, then the oldest held voice within that part.
The DSP must provide envelope levels and mark amplitude releases finished.
Replaced voice output needs a DSP crossfade; the allocator itself makes no audio.
Repeated same-note behavior follows the current synth's retrigger policy.

Sustain, note-off and channel panic stay channel-specific across layers.
Invalid events and invalid/overbooked layouts do not change state. Accepted
layout changes clear the allocation state; the eventual wrapper must silence
the corresponding DSP voices too, rather than leaving old voices stranded.
Layout changes and audio processing must not race.

The dedicated test covers default eight-voice allocation, quiet-release and
oldest stealing, stolen-note-off ownership, exhaustive note/channel routing,
splits, four layers, sustain, same-note channel isolation, velocity-zero note
off, channel/all-part panic, atomic layout rejection and 100,000 events with
no heap allocations. It is policy evidence, not a full audio test.

The [native audio integration](PART-SYNTH.md) now connects independent sounds,
voice ownership, per-part drive/volume and sample-accurate note rendering.
Next: backward-compatible host/patch state, part controls, arp/clock policy,
transpose and musical listening.
The approved single-part path must stay byte-identical before a new build is
offered for testing. Existing installation stays at 0.18 alpha.3.
