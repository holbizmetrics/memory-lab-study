# Optional per-voice drive — 0.5 development

Open DRIVE and raise VOICE DRIVE from zero. This adds gain and hard clipping to
each voice after its filter, before voices mix into the existing shared memory
drive. Zero bypasses the stage exactly. The new parameter is appended after the
original 35 host controls, preserving their IDs and order.

The range is 0–24 dB, with 20 ms target smoothing. The first dB fades the clipping
stage in to avoid suddenly truncating a resonant signal already above its
threshold. From 1 dB upward, the stage is `clamp(signal * 10^(dB/20), -1, 1)`.
This is an original simple algorithm, without oversampling or a hardware circuit
model. Hard clipping can alias. The amplitude envelope still precedes the filter.

Use OUTPUT to compensate for added level. Clipping each voice does not limit the
sum of eight voices. The shared DRIVE, MEMORY and RECOVERY controls retain their
existing behavior. Existing projects and schema-1 patch files load with voice
drive at zero. New patches use schema 2 and contain all 36 controls; older plugin
versions cannot read these new patches.

Validation records and editor previews are kept locally under outputs. The
native default phrase remains byte-identical to the pre-integration reference
(SHA256 B3BB94ED8CB9A7618498FBA2AFC6DB363BFC3BE4F30C24F087FA58105D309AD5).
The wrapper checks exercise enabled/disabled drive, block partitioning, held
eight-note edits at 44.1/48/96/192 kHz, all-sound-off, host state recall and legacy
state migration. Patch and automation tests cover the appended control.

FL Studio recording/playback and reload stability remain separate host checks.
Do not replace the installed VST3 while FL Studio or the standalone app is open.

Verified 2026-10-29: native CTest 3/3, plugin CTest 4/4, actual VST3 validation
including the new control and state recall, and normal/compact editor snapshots.
The 0.5 build is installed and its installed VST3 passed actual-module validation.
The guarded updater preserved and verified the existing 0.4 bundle first.
Holger subsequently approved the drive sound during musical playing in FL Studio.
