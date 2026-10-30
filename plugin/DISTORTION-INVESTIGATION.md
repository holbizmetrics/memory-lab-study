# Possible 0.8 distortion regression — investigation open

On 2026-10-30 Holger reported that the instrument seemed almost always distorted.
He compared listening with Dune 3, which sounded clean to him, and requested that
this be flagged as a possible regression. He also suggested the current filter
settings might be involved. This report is not dismissed as listening fatigue.
Do not treat technical checks as his approval of the current sound.

No DSP change, installed-plugin replacement or 0.9 feature development was made
during this investigation. The installed 0.8 hash remains
311BA1E72A3F84CA9546523B2BB4584891D4CF5DD6EE14C77E0313F66E1BCB5F.

## What was checked

The editor-opening/tab-switch test monitors all 41 parameters across all four
families. Opening/closing the editor and 128 page switches produce zero parameter
writes and preserve every value. Merely opening FILTER therefore did not change
the patch in this isolated check. Changing FILTER FAMILY or shared controls does
affect audio globally, regardless of the visible page. This did not automate FL.

The actual preserved VST3 modules for 0.4, 0.6, 0.7 and installed 0.8 were hash
verified and copied into correctly named audit bundles. Each render runs in a
separate process and captures the real hosted plugin, not the native reference
engine. No editor is opened. Each receives the same 48 kHz, 512-frame MIDI fixture:

- default: fresh factory parameters, E3/G3/B3, velocity 100.
- clean: same chord, shared drive/depth zero, optional voice drive/noise zero and
  Classic selected; original cutoff/Q and output retained. The name means
  drive-bypassed, not a subjective guarantee of cleanliness.
- stress: factory parameters, eight held notes.

Parameters settle without notes before each three-second capture. All twelve
raw captures are byte-identical across versions within their respective setting.
Their SHA256 groups are:

- default: D628E59F85BA2C92641BE1FC800C40BE819A7A30DB28AE5A18C2C78B7C3D8564
- drive-bypassed: BB42285D6B6892F56CD3E3A3D8DDE56083C9639B5BC627DB54F2A159B1837F7A
- eight-voice: 3D045486D3429D0654D34157D03614F90CE4EA64E18358BD0F68B28A1B238987

No captured sample is nonfinite or at/above full scale. Peaks are .0505873,
.393945 and .051196 respectively. This rules out full-scale overs in these
fixtures, not intentional nonlinear saturation or clipping elsewhere in FL.
The default still includes the original shared drive 5 and memory .8.

Measured 0.8 worst block-budget ratios were .0991 default, .0677 drive-bypassed
and .1660 eight-voice (1.0 would exhaust that block's audio duration). One run of
these fixed 48 kHz/512-frame fixtures cannot rule out underruns at other buffer
sizes/sample rates or in Holger's FL project with other processing.

## Artifacts and next evidence

outputs/distortion-audit-0.8-1 contains hash-verified plugin copies, twelve raw
float captures, parameter text snapshots, timing logs and verification.json.
The independent PowerShell/C# verifier decodes all captures, checks exact hashes
and creates four level-matched listening WAVs for 0.6/0.8 default/drive-bypassed.
index.html presents these without autoplay and pauses other players. Listening
WAVs are within .001 dB of -24 dBFS RMS with peak headroom; browser playback itself
is not automated.

No general factory-default change between these versions was reproduced. The
user's exact affected patch, MIDI and FL effect/output chain are not yet captured.
Save the affected sound as a .mlpatch before changing settings, then reproduce it
offline and compare with FL. Keep this possible regression and musical review
open; the controlled default results do not establish that his patch sounds right.
