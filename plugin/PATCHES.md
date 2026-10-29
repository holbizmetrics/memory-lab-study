# Patch files (development build)

SAVE and LOAD in the top right of the editor open native file dialogs for
`.mlpatch` files. Save a sound, choose another sound or change controls, then load
the file to restore all 41 synthesis, envelope, output and arpeggiator controls.
Keep the `.mlpatch` extension when naming files. The last selected path is used
for subsequent dialogs in that editor session.

Stop playback and release held notes before loading. Existing notes retain their
latched envelope stages; the patch does not restart notes or capture active voices.
DAW automation can override loaded parameter values when playback resumes. Loading
notifies the host of parameter changes, but host undo grouping is not verified.
FL project state remains the way to preserve the complete arrangement and its
automation clips; patch files store instrument settings only.

The 0.8 format is readable XML, with a MemoryLabPatch root and schemaVersion 5.
Older schema-1 files with their original 35 controls load with voice drive at zero.
Schema-1 and schema-2 files restore the original oscillator balance and zero
interval. Schema-2 preserves its saved voice-drive setting. New schema-3 files
from 0.6 remain readable. Schema-1/2/3 patches load with noise off; new schema-4
files from 0.7 remain readable. Schema-1/2/3/4 files restore Classic filter family;
new schema-5 files require the 0.8 build or later.
Every current parameter must appear exactly once with a finite, in-range value.
Malformed files, unsupported schemas, unknown IDs and incomplete patches are
rejected before any controls change. Saving writes a temporary file and replaces
the destination only after that write succeeds. No factory bank is included.
Host project/preset state uses the existing migration path and is unchanged.

Verification on 2026-10-29: all four CTest checks passed, including patch roundtrip
for all controls, invalid-file rejection without partial edits and file save/load.
The actual VST3 passed scan/load/audio/state/release/reload checks. Editor snapshots
were inspected at the compact size. Native dialog operation inside FL Studio is
not yet verified, and this development build has not replaced the installed 0.4.
