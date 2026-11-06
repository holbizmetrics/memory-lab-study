# Starting sounds — 0.13

SOUNDS below DICE opens an internal starting-sound browser. Choose a category
and sound, then LOAD SOUND. Selecting, browsing or reopening the editor does
not load anything. SAVE/LOAD at the top retain their separate native-file role.

Eight original patches: Clean Foundation (Basic), Round Bass, Soft Fifth (Lead),
Slow Air (Pad), Glass Pluck, Noise Tap (Percussion), Breath Swell (Texture),
Clockwork Pulse (Rhythm). These are starting points, not manufacturer presets or
hardware matches. Soft Fifth remains polyphonic; no mono/legato mode is implied.
Clockwork Pulse enables the arp and tempo-synced LFO; hold a chord. Pads/textures
need held notes to hear their slow attack. Noise Tap/Breath Swell use noise only.

LOAD SOUND replaces every sound/arp control from a full factory-based patch,
including expression settings, except output gain, which stays as set. All eight
load both drives at zero. Output preservation avoids an unexpected fader change,
not a loudness guarantee; different sounds still differ in level. A zero output
remains silent. Factory INIT still restores the original default, including its
shared drive at 5; Clean Foundation is a separate cleaner starting sound.

Play fresh notes after loading: existing note-on envelope latching remains.
Stop playback/automation first; host automation can override loaded settings.
SAVE first to keep edits. UNDO LOAD restores the complete sound from before the
last load, including output/drives/arp/expression, while the loaded patch remains
unedited. Other sound edits invalidate that one undo; no multi-step history.
Loading a starting sound clears dice-roll history. Selection and undo are UI
session conveniences, not host parameters or saved patch metadata. DAW/patch
recall stores the actual 49 parameter values, not a preset name or category.

Compatibility: 49 unchanged parameter IDs/order, schema 8, unchanged DSP/default
sound and legacy migrations. Building a preset is pure with respect to processor
parameters; all preset application goes through existing full-patch validation.

Listening recipe: save your current sound; open SOUNDS and browse without loading
(sound must stay unchanged). LOAD Round Bass, try low notes; LOAD Slow Air, hold
a chord; LOAD Noise Tap and Breath Swell, compare short/held notes; LOAD Clockwork
Pulse, hold a chord at the host tempo. Confirm your output fader stays fixed and
both drives read zero. UNDO LOAD restores the complete pre-load sound. SAVE an
edited starting sound and LOAD that file to confirm the ordinary patch workflow.

Technical validation and musical approval are recorded separately in the release
notes. These starting sounds have not yet received Holger's listening approval.
