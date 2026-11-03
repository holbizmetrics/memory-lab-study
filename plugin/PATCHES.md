# Patch files (development build)

INIT restores all sound/arp controls to the factory defaults, including output,
drive and LFO clock settings. It replaces unsaved edits: SAVE first to keep them.
It does not change remembered folders, load a file, retrigger held notes or promise
host undo grouping. Release held notes and play again to hear the default envelope;
active host automation can override initialized controls. A saved Default.mlpatch
remains useful for your own preferred starting sound.

SAVE and LOAD in the top right of the editor open native file dialogs for
`.mlpatch` files. Save a sound, choose another sound or change controls, then load
the file to restore all 49 synthesis, envelope, output and arpeggiator controls.
Keep the `.mlpatch` extension when naming files. Successful saves and loads
remember their folder for both dialogs, including after reopening the editor
or application. Cancellation and rejected patches do not update it. Missing
folders fall back to Documents. This is a local UI preference, not part of a
patch or host sound state; other DAWs' own export dialogs remain independent.

Stop playback and release held notes before loading. Existing notes retain their
latched envelope stages; the patch does not restart notes or capture active voices.
DAW automation can override loaded parameter values when playback resumes. Loading
notifies the host of parameter changes, but host undo grouping is not verified.
FL project state remains the way to preserve the complete arrangement and its
automation clips; patch files store instrument settings only.

The 0.12 format is readable XML, with a MemoryLabPatch root and schemaVersion 8.
Older schema-1 files with their original 35 controls load with voice drive at zero.
Schema-1 and schema-2 files restore the original oscillator balance and zero
interval. Schema-2 preserves its saved voice-drive setting. New schema-3 files
from 0.6 remain readable. Schema-1/2/3 patches load with noise off; new schema-4
files from 0.7 remain readable. Schema-1/2/3/4 files restore Classic filter family;
new schema-5 files require the 0.8 build or later. Schemas 1–5 restore the sine
LFO shape; schema-6 files retain the selected shape and require 0.9 or later.
Schemas 1–6 restore the original unsynced, note-retriggered LFO clock defaults.
Schema 7 requires 0.10 or later and retains the clock controls as well.
Schemas 1–7 restore KEY TRACK to 0%, AMP VELOCITY and FILTER ENV VEL to 100%.
Schema 8 saves these expression controls and requires 0.12 or later.
Every current parameter must appear exactly once with a finite, in-range value.
Malformed files, unsupported schemas, unknown IDs and incomplete patches are
rejected before any controls change. Saving writes a temporary file and replaces
the destination only after that write succeeds. No factory bank is included.
Host project/preset state uses the existing migration path and is unchanged.

Historical verification on 2026-10-29: all four CTest checks passed, including patch roundtrip
for all controls, invalid-file rejection without partial edits and file save/load.
The actual VST3 passed scan/load/audio/state/release/reload checks. Editor snapshots
were inspected at the compact size. Native dialog operation inside FL Studio is
was not yet verified in that historical check. On 2026-10-31 Holger manually
loaded eight patches in FL; FPSDK readback independently matched all 41 controls
in each (328/328 comparisons). Persistent folder behavior is a later UI update;
native-dialog persistence in FL still needs its own live check after installation.
