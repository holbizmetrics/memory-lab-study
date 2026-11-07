# 0.16 implementation contract: four envelopes

Status: implemented and technically verified in v0.16.0-alpha.1. Holger's
listening approval of these new contours remains pending. Installation status
is recorded separately in the local build manifest.

Keep the existing amplitude and filter dAHDSR contours unchanged. Add two
independent per-voice modulation contours, each with Delay, Attack, Hold, Decay,
Sustain and Release. Stage settings latch on note-on, matching the existing
contours; depth controls are smoothed. Note-off, sustain release, stealing,
all-notes-off and all-sound-off must handle the new contours consistently.
Their tails must not silently extend voice lifetime beyond the amplitude
envelope's existing lifetime contract.

Initial explicit routes for each new source: bipolar pitch depth in cents and
bipolar filter-cutoff depth in octaves. These are original software routing
choices, not a claim to have reproduced undocumented hardware routing. Broader
destinations remain the 0.17 milestone. Both new depths default to zero, and
inactive sources must preserve the exact existing sample arithmetic.

Each new envelope gets an ENVELOPES subpage with six stage controls, pitch and
filter depths, plus a contour preview. The existing amplitude/filter subpages
and their bindings remain unchanged. Do not insert new pages into the existing
numeric page range used by the four-LFO editor tests.

Append 16 unique parameters after the existing 76; preserve all previous IDs,
ranges and order. Update host state and patch schemas, validate complete new
patches before application and migrate older files with both new routes off.
Keep new sources protected from the current Dice policy and inactive in existing
starter sounds. New-format patches need not load in older plugins.

Verification gates before delivery:

- Independently exercise delay/attack/hold/decay/sustain/release, note-off during
  attack, zero-duration stages, stage latching, sustain and voice stealing.
- Isolate each source and destination, combine both, and remove each depth to
  establish independent contributions across sample rates and block sizes.
- Require exact old-default and saved 0.15 sound render parity with new depths
  zero; test old patch migration, host state and all 92 parameter bindings.
- Check isolated editor rendering, page changes without parameter writes, actual
  VST3 parameter export, state reload, CPU and output headroom.
- Repair the stale Windows version-resource dependency separately from audio;
  verify PE and VST3 versions agree in the delivered build. Do not assume this
  repairs FL's existing-instance table discrepancy.
- Install only with FL and standalone closed, with verified backup. Publish an
  annotated source-only prerelease of the exact verified source commit.

## Verified results

All 13 plugin CTest checks passed (104.36 s); all six native checks passed
(44.98 s). Nine actual VST3 audio fixtures passed with peak 0.606819 at the
fixture's output setting. Default, schema-7 clocked and Holger's local saved
schema-9 sound render byte-identically to the preserved earlier captures.
Actual VST3 scan/load, MIDI audio, state roundtrip, release and reload passed.
The normal and compact ENV 3/4 editor snapshots were inspected. All 92
automatable IDs/order and bindings pass; the first 76 IDs are unchanged.
Windows PE and VST3 build versions now report 0.16.0. One eight-note capture
reported a held median block time of 1006.1 us at 48 kHz / 512 frames; this is
an offline observation, not a realtime dropout guarantee.

The new controls are ENVELOPES / ENV 3 and ENV 4. Each has six stage knobs,
PITCH DEPTH (-1200..1200 cents) and FILTER DEPTH (-4..4 octaves). The contour
is unipolar; negative route depths reverse its effect. Release is audible only
while the amplitude envelope keeps the voice alive. INIT and starter sounds
set both new routes to zero; Dice preserves these new settings as it does LFOs
2-4. Schema 11 stores all 92 controls; schemas 1-10 migrate to inactive routes.
Host state version is 13. New files require a compatible newer plugin.

Holger confirmed the ENV 3 pitch contour and ENV 4 filter swell. He identified
the brightness movement clearly with ENV 3 pitch depth zeroed, then confirmed
restoring the saved sound after Clean Foundation and restoring the FL project
after a complete restart. These are musical and recall approvals of the tested
0.16 configuration; broader 0.17 routing requires its own check.
