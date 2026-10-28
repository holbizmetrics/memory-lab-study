# Native editing surface

Version0.2 replaces the generic parameter list with an original JUCE editor.
The published eight-encoder interaction direction motivated a paged control bank;
no hardware artwork, branding or exact panel layout was copied. The surface exposes
implemented features rather than implying full Relic feature coverage.

## Controls and display

Voice page: detune, pulse width, cutoff, resonance, profile, drive, memory depth,
recovery. Motion page: LFO rate, vibrato, pulse motion, filter motion, filter-envelope
amount, pressure routing, memory depth, recovery. Memory and recovery are repeated
across pages for convenient access to the shared output stage. All are attached to
the existing host parameters; sixteen parameter IDs and the plug-in class IDs remain
stable for state/host compatibility.

The central editing display shows the selected control, live parameter value, units
and a short description. Voice page plots the static classic filter target response,
respecting sample-rate cutoff limits. It excludes instantaneous modulation and is
not an audio spectrum analyzer. Motion page shows one second of the per-voice sine
LFO, or the fixed filter-envelope shape when its amount is selected. The envelope's
20/350/.15/400 ms contour is identified as fixed timing, not editable stages.

Master output gain, phase reset, a measured peak indicator and active-voice count
remain available alongside the pages. The output and voice displays read atomics
published by the audio callback. The keyboard covers MIDI36–96 with C4 assigned
to MIDI60, consistent with the listening documentation. Clicks feed the same MIDI
path as hardware notes. Host MIDI notes also highlight the keyboard.

Drag a knob, type its numeric value or double-click it to restore that parameter's
default. The page can be resized from832x592 to1560x1110 with fixed aspect ratio;
the default is1040x740. Editor page/selection are not stored in the sound preset.
Host state still stores parameter values. No new synthesis stages were added.

## Checks

The final build passed the existing native/default parity, MIDI timing, stereo,
channel commands, sustain, bend/pressure, state and block-partition smoke checks.
On-screen keyboard injection produced audio and released to silence. Editor checks
constructed both pages, verified representative cutoff/vibrato attachments and
rendered Voice/Motion/compact PNGs. Those layouts were visually inspected, including
a keyboard-width and octave-label correction. Final images: outputs/editor-preview-2.

The actual0.2 VST3 passed the separate scan/instrument/instantiate/MIDI-audio/state/
release/reload check. These tests do not substitute for mouse interaction, native
display scaling or controller verification inside FL Studio. JUCE's keyboard queue
is now part of the callback boundary; the native engine remains allocation-free in
its earlier bounded tests, but no new whole-framework realtime guarantee is claimed.

## Installed update

The installed0.1 bundle was verified as loaded by FL Studio. Holger confirmed closing
FL Studio before the update. update-installed.ps1 checks the host is closed, verifies
the existing binary against the first-build record, backs it up, copies only the
new bundle's files and verifies every copied hash. The old build is preserved at
outputs/installed-backup-0.1.0. The standard installation path remains:

`C:/Program Files/Common Files/VST3/Memory Lab Study.vst3`

Installed0.2 status and hashes are recorded in outputs/editor-build-0.2.json after
the update. Reopen FL Studio and reload Memory Lab Study; use a verified rescan if
the host still shows cached metadata. Target-host behavior of the new surface still
needs Holger's confirmation. A custom interface does not complete the remaining
oscillator/filter/modulation/multitimbral modeling work.
