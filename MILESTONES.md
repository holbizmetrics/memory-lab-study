# Memory Lab: visible progress toward the Relic-inspired instrument

Updated 2026-11-06. An implementation milestone and musical approval are separate.
This is an original instrument inspired by documented Relic features, not yet a
faithful hardware emulation. There is no defensible completion percentage: one
four-part voice architecture can be more work than several UI milestones.

## Current checkpoints

| Area | Implemented here | Still missing / boundary | Holger's testing |
| --- | --- | --- | --- |
| Voices | Eight-voice pool, sustain, stealing, MIDI bend and channel pressure | Four-part allocation/layers/splits; MPE not implemented | Played in FL; no full multi-part test possible yet |
| Oscillators | Saw source + pulse source, levels, interval/detune, pulse width/PWM, phase reset, per-voice noise | Not two fully equipped hardware VCO models; oscillator sync/wider capabilities remain | OSC/noise listening and general instrument playing |
| Filters | Classic HP/notch/LP profile, LP4, BP4, BP4/LP2 Blend | Full documented family/profile/bandwidth behaviour and measured hardware matching remain | Family/drive listening; Profile clarified as Classic-only |
| Envelopes | Two independent dAHDSR contours: amplitude and filter | Four envelopes and broader routing; stage values latch on note-on | Existing contours played; filter velocity confirmed |
| LFOs | Four per-voice LFOs in 0.15, each with four shapes, independent timing and pitch/PWM/cutoff depths | Extra shapes/ranges and broader destinations remain | LFO 1/2 confirmed; LFO 3/4 await their own test |
| Expression | Independent amp/filter-env velocity response, note-to-cutoff tracking, channel pressure | Full performance/modulation architecture and MPE remain | Three 0.12 expression controls confirmed with keyboard |
| Drive | Per-voice clipping and original shared memory drive, independent bypass at zero | Original design; no hardware distortion match established | Purpose clarified; clean/bypass comparisons and dice bypass confirmed |
| Patch workflow | File save/load, INIT, host state, Dice/locks/undo | Full FL recording/undo/project-reload checks remain | Save/load used; Dice, bypass and Undo confirmed |
| Starting sounds | Eight original sounds, category browser and explicit Load/Undo | User-file library management remains outside this bank | All eight approved; Round Bass confirmed around C3 |

## Proposed next checkpoints — scope can split when needed

| Version | Concrete next goal | What it advances |
| --- | --- | --- |
| 0.13 | Starting presets, category browser, explicit load and undo | Workflow; no major synthesis architecture gap closed |
| 0.14 | Second independent LFO and recall tested/approved by Holger | Two-source independent simultaneous modulation |
| 0.15 | LFO 3/4 technically verified and installed; musical test pending | Closes LFO-count gap; new-source musical test next |
| 0.16 | Third/fourth envelopes and useful modulation destinations | Documented envelope-count gap |
| 0.17 | Broader explicit modulation routing | Source/destination flexibility |
| 0.18 | Oscillator/filter expansion against a refreshed documented gap list | Source and filter architecture; split if needed |
| 0.19 | Four-part allocation, layers and splits | Part/voice architecture; likely multiple milestones |
| 0.20 | Cross-feature, compatibility, CPU and DAW integration checks | Evaluate the combined instrument, not individual modules alone |

These are proposed checkpoints, not a frozen schedule or a guarantee of hardware
completion by 0.20. Scope each against manufacturer evidence before implementation.
The next synthesis milestones take priority over additional convenience features
after the already-started 0.13. Keep the approved sound, stable parameter IDs and
old-patch defaults throughout. Matching hardware tone/control behaviour requires
reference measurements; feature count alone cannot establish it.

Holger completed the combined 0.13/0.14 listening round and explicitly approved
continuing to four LFOs. Stop after 0.15 delivery for the LFO 3/4 listening round
before proceeding to the next synthesis milestone. No new-source approval is
inferred from his enjoyment of the first two sources or from automated checks.

Evidence: [recorded voice architecture](VOICE-EVIDENCE.md),
[interview evidence](RELIC-INTERVIEW-EVIDENCE.md), [roadmap/history](ROADMAP.md).
