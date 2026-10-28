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

After restarting FL Studio, the user reopened the saved copy successfully and
Memory Lab's editor appeared. This confirms loading in a fresh process; it does
not resolve the earlier exit during same-session reload. Playback after restart
has not yet been reverified by the agent.

No synth runtime changes or new installed build were required for this initial
automation check. The installed binary remains the verified 0.4 build.

## WinDbg / C# extension follow-up

The original Windows report retained Report.wer but its temporary crash dump was
gone. At the user's request, cdb captured a separate full-memory snapshot of the
current FL process at 22:43:30 using a non-invasive attach, then detached. FL
remained running. This is NOT the 22:30 crash dump.

A fresh offline cdb session loaded the user's WinDbgAotExt csfile-preview bundle
with bridge/. Version reported 1.3; the hosted CoreCLR test returned 4242.
The extension's report enumerated 199 modules and 118 threads, and identified a
native target without a managed heap. A C# LINQ query over debugger.Modules
returned ilwasapi2asio_x64 and Memory_Lab_Study. No module name containing "focus"
matched. Module presence or absence does not establish the earlier crash's cause
or identify a hardware fault.

The snapshot event is debugger wake 0x80000007, not the FL crash 0xc0000409.
The extension's generic "crash dump" label and unclassified-break verdict must
be read in that context. No culprit was established. The user reports unusual
Focusrite behavior; that remains a hypothesis, not a diagnosis.

Local artifacts are in ignored outputs/debug/: capture/analyze/query command
files, logs, fl-current-triage.md and the approximately 1.5 GB current-state dump.
The dump was not uploaded or attached to GitHub. A dump captured at an actual
future failure is needed for crash-stack analysis.

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
