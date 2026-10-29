# White noise — 0.7 development

On OSC, raise WHITE NOISE to mix noise with the saw and pulse sources. For noise
alone, set OSC 1 / SAW and OSC 2 / PULSE to zero. The level ranges 0–1 and smooths
over approximately 20 ms. Zero disables the source exactly after smoothing settles.
Noise enters the existing amplitude envelope and filter, then the voice and shared
drive stages. Raising source levels can increase output; use OUTPUT to compensate.
VOICE DRIVE remains on the DRIVE page.

For a swell, lengthen the amplitude attack and hold notes. For a riser, sweep
filter cutoff upward while holding noise. Resonance adds tonal colour; noise alone
does not track oscillator pitch or the OSC 2 interval. Each active voice filters
its own deterministic xorshift32 pseudorandom stream, seeded by note-on order,
note and MIDI channel. No hardware noise-circuit model or measured spectrum is
claimed. Host state saves settings, not active voices or the noise generator phase.

Holger approved the noise-study-1 percussive and chord examples as clean and
musically useful, particularly noise solo, and requested plugin integration.

The new noiseLevel parameter appends after the existing 39 host parameters.
Old projects and schema-1/2/3 patch files load with noise off. New patches use
schema 4 and contain 40 controls, requiring this build or later. Existing
oscillator/drive values are retained when their patch schema supports them.
The approved default sound is preserved. The v0.5.0-alpha.1 tag is unchanged.

Verified on 2026-10-29: native CTest 5/5, plugin CTest 4/4 and actual VST3 module
checks passed, including new noise state recall. Integration checks cover held
eight-note noise edits at 44.1/48/96/192 kHz, default silence with oscillators muted,
mute/re-enable/all-sound-off, exact block partition parity, host state migration,
stable parameter order and schema-1/2/3 patch migration. Normal/compact editor
snapshots were inspected. The default native audio remains byte-identical.
Full FL automation recording/undo/project recall remains unverified separately.

Listening status: Holger subsequently reported uncertainty about integrated 0.7
noise and an impression that it damaged the sound. Integrated musical approval
is open. The [diagnostic comparison](../native/NOISE-DIAGNOSTIC.md) reproduces all
six approved offline raw examples exactly using the production engine with
matched settings. Separate drive-context clips investigate the shared nonlinear
path; they do not diagnose the user's uncaptured patch. No DSP change was made.
