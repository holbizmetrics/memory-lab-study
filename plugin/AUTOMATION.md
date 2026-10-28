# DAW automation verification

## Current result, 2026-10-28

The installed 0.4 VST3 already supports the parameter connection needed for DAW
automation. In FL Studio 2026, a mouse drag on Cutoff was recognized as
Memory Lab Study / Filter cutoff (Hz) in Tools > Last tweaked. An automation clip
was created, enabled on its Playlist track, and edited with low/high points.
In Song mode, playback visibly moved the knob, numeric value and filter curve.
Observed playback values included approximately 271, 839 and 1457 Hz without
additional knob input. Playback was stopped afterwards.

Work took place in the user-created Memory Lab Study Copy.flp, not the original
musical project. The test copy was saved before reloading it from Recent projects.
The cutoff clip and Memory Lab editor reappeared on reload. FL also displayed a
missing Diva plug-in notice for another project instrument, then exited
unexpectedly. The user confirmed they had not closed FL Studio.

Windows Application events 1000/1001 at 22:30 on 2026-10-28 report FL64.exe
26.1.7.5653, exception 0xc0000409, faulting module unknown, report ID
cb492ca4-07f1-495e-a002-46493768f395. This does not identify a responsible plug-in.
Project reload/playback is NOT a passed acceptance check. Investigate the exit
and repeat under controlled host conditions before declaring recall reliable.

No synth runtime changes or new installed build were required for this initial
automation check. The installed binary remains the verified 0.4 build.

## Source-side checks

Run MemoryLabSmoke --automation-test after building. The checks cover:

- All 35 existing parameter IDs, registration order and automatable status.
- One begin gesture, two value notifications and one end gesture for a simulated
  slider drag using JUCE's ScopedDragNotification, on representative controls
  across Voice, Motion, Amplitude, Filter and ARP pages.
- Parameter-to-editor updates without additional edit gestures/value echoes.
- Host-side edits to a hidden control followed by page switching/rebinding.
- Parameter value serialization/recall.

These are JUCE parameter/attachment checks. They do not verify real mouse
recording, host undo grouping, FL event-data recording, project automation storage,
audio smoothness under fast ramps or sample-accurate parameter automation.

## FL Studio clip workflow

1. Turn a Memory Lab knob using the mouse.
2. Use Tools > Last tweaked > Create automation clip. Confirm the target name.
3. Enable the clip's Playlist track and edit its control points.
4. Use Song mode for Playlist automation playback.

The alternative is Browser > Current project > Generators/Effects > plug-in
parameter list, then right-click the parameter and create an automation clip.
This alternative was documented but not tested here.

Image-Line's reference:
https://www.image-line.com/fl-studio-learning/fl-studio-online-manual/html/playlist_automationclip.htm

## Remaining work

Investigate the unexpected FL exit; repeat project save/reload/playback. Then
verify recorded knob performances, undo gestures, automation for the remaining
control families, page/display following, and bounded/smooth fast parameter ramps.
Envelope-stage automation retains the existing next-note latch behavior.
