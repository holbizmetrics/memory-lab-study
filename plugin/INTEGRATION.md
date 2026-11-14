# 0.20 integration checkpoint

This checkpoint combines existing features; it adds no sound controls or DSP
changes. There are still 409 parameters, patch schema 15 and host state 17.
The approved default, parameter IDs/order and older patch migration are retained.

`integration-checks.hpp` exercises three setups at 44.1, 48, 96 and 192 kHz:
four parts with separate MIDI channels and drives, an omni two-way split,
and four layers fed by the global arp. Oscillator/noise mixing, classic/modern
filters, all four LFOs and envelopes, expression and parameter edits are active.
Host tempo loss and transport stop, sustain, pitch bend, pressure and panic
are included. Patch and host-state clones rendered with different block sizes
must produce exactly equal audio, remain finite and respect eight voices.
The 12-context fixture peak was 0.038395; this is fixture headroom, not a
guarantee for arbitrary sounds or output settings.

`study-integration.py` loads the actual VST3 module: 34 existing recordings
remain byte-identical, and three combined setups produce finite audio with
peak 0.040846. File-loader validation precedes each patch capture.
Actual module scan/load/MIDI/arp/state/release/reload also passes. The full
plugin CTest suite passes 18/18 (143.30 seconds). Native DSP sources are
unchanged; their preceding 0.19 result remains 9/9, not a new 0.20 run.

Holger approved 0.19 layers, split, complete patch/project recall, four-part
voice reservations and MIDI routing. This is separate from automated 0.20
checks. Parameter edits here emulate host updates; they do not establish FL
automation recording, undo or realtime dropout behavior. CPU profiling,
aliasing measurements, long live sessions and hardware sound equivalence
remain open. Fixed reservations and one global arp remain deliberate limits.

The checkpoint does not declare the Relic conversion complete. Remaining
hardware coverage includes modulation forms/routing and oscillator behavior,
as well as reference measurements for filter and drive characteristics.

Holger's 0.20 live regression approval: fresh instance/version identification,
Bass and Air Split zoning, Slow Wah Fast Heart movement, and normal note
release without stuck notes all confirmed. This completes the planned short
listening round; it does not close the broader live-performance checks above.
