# Filter families — 0.8 development

FILTER offers CLASSIC, LP4, BP4 and BLEND. Classic is the original HP/notch/LP
filter and remains the default. CLASSIC PROFILE only affects that family; the
selected display says INACTIVE for that control in other families. Cutoff,
resonance/Q, filter-envelope amount and filter-LFO amount affect all families.
Noise and oscillators share the selected voice filter; this is not separate
noise routing. Both drive stages retain their existing placement and behavior.

LP4 cascades two identical low-pass sections. BP4 cascades two normalized
band-pass sections. BLEND sums .5*BP4 + .5*LP2, including their relative phase.
These are the original models in native/FILTER-FAMILY-STUDY.md, not measured
hardware responses. Q applies to both sections and affects bandwidth; there is
no independent bandwidth control. The static response display shows the target
family/parameters, not the brief transition between families or nonlinear drive.

All filter paths run continuously while a voice is active, retaining their state
when the mode changes. Output weights crossfade with approximately 20 ms target
smoothing rather than resetting filter state or switching output abruptly.
The exact original signal path remains selected when Classic weights settle.
The extra paths add processing work; no universal realtime deadline is claimed.

The filterFamily host parameter appends after the original 40 controls. Older
projects and schema-1/2/3/4 patches restore Classic. New patches use schema 5 and
contain all 41 controls. Existing profile/noise/drive values remain preserved as
supported by their schema. The v0.5.0-alpha.1 tag remains unchanged.

The musical approval of the new live families remains open. Holger requested
continued development after the study was delivered; this is not recorded as
an explicit listening approval of every filter example or live mode.

The production engine was compared with all twelve raw study clips, including
the shared-drive chords, after settling mode weights before notes. The worst
absolute float-sample difference was 3.72529e-09 (bound 1e-7), recorded locally in
outputs/filter-family-production-0.8.csv. The original native default phrase is
byte-identical. This is technical agreement under matched settings, not a
subjective sound-quality verdict or a recording of a user's actual FL patch.

Validation on 2026-10-29: native CTest 6/6 and plugin CTest 4/4 passed. The actual
VST3 passed scan/load/audio, family/noise/drive state recall, arp/all-off/release
and second-instantiation checks. Integration tests cover held eight-note mode
switches and high-Q cutoff edges at 44.1/48/96/192 kHz, exact block partition
agreement, legacy host state, all four older patch schemas and stable host IDs.
Normal LP4 and compact Blend editor snapshots were inspected. Full FL automation
recording/undo/project reload remains unverified separately. Installation is
pending FL closing; the guarded updater backs up and verifies installed 0.7 first.
