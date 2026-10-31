# Possible 0.8 distortion regression — investigation open

## LP4 compensation listening study (2026-10-31)

`plugin/study-lp4-headroom.py` creates isolated derivative patches and captures
the preserved actual 0.8 VST3 at Q 3.72 and 8, noise MIN/MAX. It compares Classic,
original LP4 and a candidate 1/Q source scaling, each drive enabled/bypassed.
Because the voice filters are linear at fixed settings, scaling every source
approximates scaling LP4 output before drive; source-control quantization remains.
Four comparisons against scaled original bypassed captures bound that difference
to .004 of the original peak. This is an approximation study, not a new DSP mode.

All 24 captures completed and all 24 listening WAVs were read back and verified
within .001 dB of -24 dBFS RMS, with peak headroom. Artifacts and measurements:
`outputs/lp4-headroom-study-2/index.html` and `measurements.json`. A first run
stopped on an artifact filename handling error; it is retained separately and
not counted as a successful study. The 1/Q candidate changes gain across the
whole spectrum, including the passband; equal gain at cutoff does not make the
filters equivalent or establish that this is the desired musical correction.

With Q 3.72/noise MIN, original LP4 drive-on crest was 8.51 dB versus 11.09 dB
for the candidate (Classic 9.10 dB). Noise MAX: 7.91 versus 9.38 dB. With Q 8,
MIN: 7.50 versus 10.33 dB; MAX: 7.43 versus 8.84 dB. These are whole-clip crest
measurements, not an objective distortion rating or isolated compression meter.
No installed plugin, host parameters, source patches or DSP sound defaults changed.

The independent equation checks now include Q 2.4, 3.72 and 8 and exact-cutoff
response evaluation, covering the previously missing resonant gain cases. This
strengthens response verification; it does not by itself impose a gain policy.
Native CTest passed 6/6; the expanded test passed 90 impulse cases / 630 complex
response comparisons (worst absolute complex error 7.67583e-13), plus the existing
high-Q edges. The installed 0.8 binary hash remains 311BA1E7...E1BCB5F.

## Holger's FL exports, 2026-10-30

Eight schema-5 patches and eight stereo 44.1 kHz/32-bit float WAVs are now in
`outputs/Holger's Test Output - mlpatches`. Holger reports removing the master
Emphasizer and all other plugins, assigning the synth its own mixer channel,
and re-rendering Classic MIN. He loaded patches before exporting. This is useful
manual recall evidence, not an automated proof that every FL export used the
intended state/MIDI. He noted that late-night repeated file navigation made the
process error-prone; retain that uncertainty without assuming a mistake.

All patches agree except noiseLevel (0/1) and filterFamily (0/1/2/3): cutoff
2062 Hz, resonance 3.72, shared drive 5, memory .8, voice drive 0. All eight
WAVs are finite and below full scale. Whole-file RMS/crest measurements are in
`outputs/holger-renders-analysis.json`. LP4 is about .61 dB (noise MIN) or .69 dB
(MAX) higher RMS than Classic, with nearly unchanged peaks. This is consistent
with stronger saturation/compression, not proof of causation or an evaluation
of audible quality. Durations differ by about 4.8 ms, so whole-file comparisons
are approximate and are not sample-aligned null tests.

Independent PCLA review identified two coverage gaps: equation response checks
use low resonance; integration checks bypass drive. Its Python-model gain
measurements require confirmation in actual-plugin captures. Exact affected MIDI
and host state are still unavailable. Next compare the saved settings through the
real plugin with a fixed phrase, drive enabled/bypassed, preserving filter/noise.

Usability follow-up: remember the last successful patch folder across editor and
application restarts. The current editor only remembers patchFile in its lifetime.
This preference must not become a sound parameter or change saved-patch behavior.

## Actual-plugin saved-setting captures

### Live FL patch recall verified via FPSDK (2026-10-31)

Holger manually loaded Classic, LP4, BP4 and Blend, each noise MIN then MAX, into
the sole remaining Memory Lab instance (#2, global channel 4). The FPSDK console
queried FL's exposed parameters through its MIDI scripting readback, without
Windows UI Automation or parameter writes. All eight snapshots matched all 41
saved patch values in stable host order, using normalized-value tolerance 2e-6
and each parameter's declared range/skew: 328/328 comparisons passed. The first
LP4 MIN attempt used an outdated filename and did not perform a comparison;
the corrected filename passed. That failed attempt is not counted as a match.

Evidence: `outputs/fpsdk-*-0*.json` (eight successful reports), accompanying logs
and `outputs/fpsdk-memorylab-readback.log`. Observations are sequential pages,
with identity consistency checked; they are not an atomic host snapshot or an
audio/curve verification. FL reports 4240 exposed slots including wrapper/reserved
entries; only the synth's first 41 controls were compared.

Earlier live inspection showed two synth instances, but Holger clarified that
only the song-pattern instance was used for export, with no manual note playing.
Their presence alone is not evidence of layering. He subsequently removed the
unused instruments and saved a separate FL project named
"Memory Study Lab - Probable Noise Problem" in FL's default Projects folder.
The FPSDK bridge remains reachable after removing its helper instrument.

This gives positive evidence that these eight loads restore the intended controls.
It does not establish that no intermittent recall bug exists or prove the exact
state during the earlier exports. No sound fix, installation or version change
was made during these checks.

The diagnostic host now supports `--capture-patch <bundle> <stem> <patch> <mode>`
for complete schema-5 patches. It maps the 41 IDs into the VST3 component state,
then reads every value back, rejecting differences above .001. This validates
host-state application, not the editor's patch-file parser or Holger's manual recall.
`default` preserves the patch; `clean` zeros only drive/depth/voiceDrive, retaining
noise and filter family (unlike the older factory `clean` mode).

All 16 runs against the preserved actual 0.8 module completed: fixed E3/G3/B3,
velocity 100, 48 kHz/512 frames, three seconds. No nonfinite values. The legacy
factory capture remains byte-identical to its earlier SHA256 D628E59F...8564,
confirming that extending the host did not alter that fixture. Installed plugin
and DSP source were not replaced or changed.

For noise MIN, drive-bypassed Classic RMS was .0727517 versus LP4 .118630
(about 4.25 dB higher); drive-on RMS was .0178215 versus .0191994. For noise MAX,
bypassed Classic RMS was .0995668 versus LP4 .220498 (about 6.91 dB higher);
drive-on RMS was .0188468 versus .0205894. Whole-clip drive-on/bypassed ratios
are not compressor gain-reduction meter readings: the nonlinear stage changes
both amplitude and spectrum. These observations substantiate stronger drive
interaction, not the cause of every distortion Holger heard.

LP4 MAX with drive bypassed reached 1.37847 and had 108 samples at/above full
scale. Float capture preserves those values; playback/export into a bounded
stage could clip. This is a reproducible output-headroom case with this patch
and controlled MIDI, not a claim that his drive-on FL exports clipped.

`outputs/holger-plugin-captures-2` retains raw float files, parameter readbacks,
logs, measurements.json and sixteen listening WAVs. `index.html` compares drive
on/bypassed at matched -24 dBFS RMS with peak headroom. The phrase/sample rate
are deliberately controlled and not asserted identical to Holger's FL MIDI.
His own eight exports also have level-matched copies and an index under
`outputs/holger-filter-listening-1`. Originals remain untouched. Browser playback
has not been verified. No gain compensation or factory-default change is approved
by these measurements alone; musical comparison remains open.

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

No general factory-default change between these versions was reproduced. Saved
settings and FL exports are now available as described above, but exact MIDI and
host state remain unavailable. Keep this possible regression and musical review
open; controlled results do not establish that his patch sounds right.
