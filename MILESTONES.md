# Memory Lab: visible progress toward the Relic-inspired instrument

Updated 2026-11-08. An implementation milestone and musical approval are separate.
This is an original instrument inspired by documented Relic features, not yet a
faithful hardware emulation. There is no defensible completion percentage: one
four-part voice architecture can be more work than several UI milestones.

## Current checkpoints

| Area | Implemented here | Still missing / boundary | Holger's testing |
| --- | --- | --- | --- |
| Voices | Eight-voice pool, sustain, stealing, MIDI bend and channel pressure | Four-part allocation/layers/splits; MPE not implemented | Played in FL; no full multi-part test possible yet |
| Oscillators | Two saw/pulse cores, four waveform levels, separate widths, OSC 2 tuning/PWM, phase reset, noise | Hard sync, OSC 1 PWM and full independent routing/SEM matching remain | Earlier OSC/noise confirmed; new complementary waveforms await listening |
| Filters | Classic HP/notch/LP profile, LP4, BP4, BP4/LP2 Blend | Full documented family/profile/bandwidth behaviour and measured hardware matching remain | Family/drive listening; Profile clarified as Classic-only |
| Envelopes | Four independent dAHDSR contours: amplitude, filter and two pitch/cutoff modulators | Pitch routes from all four contours, PWM from ENV 3/4; a general matrix remains | Existing contours played; filter velocity confirmed; ENV 3 pitch and ENV 4 cutoff, patch and full FL restart recall confirmed |
| LFOs | Four per-voice LFOs in 0.15, each with four shapes, independent timing and pitch/PWM/cutoff depths | Extra shapes/ranges and broader destinations remain | LFO 1/2 confirmed; LFO 3 filter and LFO 4 pitch solo/combined, patch and FL restart recall confirmed |
| Expression | Independent amp/filter-env velocity response, note-to-cutoff tracking, channel pressure | Full performance/modulation architecture and MPE remain | Three 0.12 expression controls confirmed with keyboard |
| Drive | Per-voice clipping and original shared memory drive, independent bypass at zero | Original design; no hardware distortion match established | Purpose clarified; clean/bypass comparisons and dice bypass confirmed |
| Patch workflow | File save/load, INIT, host state, Dice/locks/undo | Full FL recording/undo/project-reload checks remain | Save/load used; Dice, bypass and Undo confirmed |
| Starting sounds | Eight original sounds, category browser and explicit Load/Undo | User-file library management remains outside this bank | All eight approved; Round Bass confirmed around C3 |

## Proposed next checkpoints — scope can split when needed

| Version | Concrete next goal | What it advances |
| --- | --- | --- |
| 0.13 | Starting presets, category browser, explicit load and undo | Workflow; no major synthesis architecture gap closed |
| 0.14 | Second independent LFO and recall tested/approved by Holger | Two-source independent simultaneous modulation |
| 0.15 | LFO 3/4 solo/combined and patch/project recall confirmed | Closes LFO-count gap; new-source musical test next |
| 0.16 | ENV 3/4 listening and patch/project recall confirmed | Documented envelope-count gap |
| 0.17 | Tested routes, auto-load and patch/project recall confirmed | Source/destination flexibility |
| 0.18 | First oscillator waveform step technically verified; modern filter step remains | Source and filter architecture; split if needed |
| 0.19 | Four-part allocation, layers and splits | Part/voice architecture; likely multiple milestones |
| 0.20 | Cross-feature, compatibility, CPU and DAW integration checks | Evaluate the combined instrument, not individual modules alone |

These are proposed checkpoints, not a frozen schedule or a guarantee of hardware
completion by 0.20. Scope each against manufacturer evidence before implementation.
The next synthesis milestones take priority over additional convenience features
after the already-started 0.13. Keep the approved sound, stable parameter IDs and
old-patch defaults throughout. Matching hardware tone/control behaviour requires
reference measurements; feature count alone cannot establish it.

Holger completed the combined 0.13/0.14 listening round and explicitly approved
continuing to four LFOs. The 0.15 listening round confirmed LFO 3 filter and LFO 4 pitch, both together,
patch-file recall and FL-project recall after restart. Stop after 0.16 delivery
for the new-envelope listening round before proceeding to broader routing.
Holger has now confirmed ENV 3 pitch, ENV 4 cutoff and patch/FL restart recall.
0.17 technical checks pass; its new routes and auto-load interaction await their
own listening/UI check. Installation status is separate from source publication. No new-source approval is
inferred from his enjoyment of the first two sources or from automated checks.

Evidence: [recorded voice architecture](VOICE-EVIDENCE.md),
[interview evidence](RELIC-INTERVIEW-EVIDENCE.md), [roadmap/history](ROADMAP.md).

0.17 listening update: Holger confirmed LFO 1 tremolo/bypass, ENV 3 PWM,
filter-envelope and amplitude-envelope pitch/bypass, patch recall, FL restart
recall and auto-load preference. No blanket approval of every new route implied.
0.18 complementary-waveform listening is next.

0.18 alpha.2: patch endpoint recall fix and VOICE hint spacing; oscillator width and OSC 2 octave listening confirmed. Original blend patch now passes read-only loader checks; live recall pending. No DSP or parameter-layout changes.
