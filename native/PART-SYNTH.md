# Four-part native audio engine

PartSynth connects the tested allocator to independently configurable sounds.
The 0.19 source milestone now connects this engine to plugin part controls and
schema-15/state-17 persistence. See [the plugin contract](../plugin/PARTS.md).
Local installation and Holger listening approval are tracked separately.

Each part owns its oscillator/filter/envelope/LFO settings, modulation phase,
sustain/expression state, volume and memory-drive state. Its voice quota comes
from the one global eight-slot reservation map. Storage currently includes four
Synth objects, but allocation activates at most eight voices across them, not
32. This reuses the existing DSP instead of introducing another oscillator path.

The default map sends the full keyboard to part 1 with eight slots. That path
renders directly to the output without summation or added buffering latency.
Layered parts sum in fixed part order. Output is not automatically normalized;
part volume must provide headroom. Memory drive is per part in this prototype,
so four-part drive interaction differs from a single drive after the full mix.
There is no new global drive/master-gain stage or hardware-routing claim.

Notes, splits, MIDI channels, sustain and panic feed the allocator and DSP.
The allocator receives actual amplitude-envelope levels to choose the quietest
released voice. Voice starts reuse the existing phase/noise/envelope/reset and
5 ms steal-transition code. Additional parts use deterministic distinct noise
seeds, preventing identical noise streams in identical layers; part 1 keeps its
original seed behavior. Each part can already use all existing native Param
settings. Parameter events in the legacy event stream address part 1; explicit
part parameter edits can occur between render ranges.

Accepted layout changes silence all DSP voices and clear allocation ownership.
They preserve sound settings but are not a click-free live zone-edit operation.
The eventual UI should make layout application explicit. Invalid layouts and
invalid event-offset blocks are atomic and do not change the engine. Methods
are single-audio-thread operations, not safe for concurrent UI writes. Prepare
resets the underlying sound engines; the future wrapper must reapply settings.

Tests compare one-part audio exactly against Synth at four sample rates, with
active oscillator/modern-filter settings, sustain, bend, pressure and panic.
A four-sound layer equals the sample-wise sum of four separate reference synths;
layered rendering is identical across different block partitions. Additional
checks cover eight-voice/part quotas, split/channel isolation, sustain/panic,
independent noise streams, layout invalidation, atomic invalid blocks and no
heap allocation during audio dispatch/render. The pre-refactor native phrase
is separately compared by SHA256 to guard the shared voice-start extraction.

Remaining: custom allocation, borrowing, per-part arp, separate outputs,
complete realtime checks and musical approval. The implemented wrapper uses
one global arp, per-part LFO timing and selected-part host automation. Source
publication does not imply installation or live FL approval.
