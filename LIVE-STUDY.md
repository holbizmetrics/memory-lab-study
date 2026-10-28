# Playable memory study

Holger listened to the frozen envelope-memory medium take and described it as
musical and potentially useful in his productions. This is initial qualitative
feedback, not a blind preference test or a statement about hardware accuracy.
The original outputs/ideation-1 files and memory_dsp.py are preserved unchanged.

## Launch

From this project directory:

```powershell
py -3 -m http.server 8766 --bind 127.0.0.1
```

Open http://127.0.0.1:8766/live.html in Chrome. Enable audio, play the phrase,
use the numbered keys, or play A W S E D F T G Y H U J K on the computer keyboard.
Keyboard notes need audio enabled first. Connect MIDI explicitly requests
non-SysEx input permission; choose your input in the adjacent selector. No MIDI
output is sent. This prototype treats the selected input as one instrument,
without separate channel parts, sustain pedal, pitch bend or MPE.
Stop cancels pending demo notes and releases sounding notes over 500 ms.
The local server is a foreground development server, not a message watcher.
Stop it with Ctrl+C when finished.

## Sound and scope

Default drive 5, depth .8 and recovery 250 ms retain the selected memory settings.
The live voice uses Web Audio saw/square oscillators, seven-cent detuning,
velocity-scaled amplitude envelopes and a 1450 Hz low-pass filter with Q 2.4.
Its oscillator phase, pulse width, filter, and resampling differ from the frozen
source; it is a new playable approximation, not an exact reproduction of the take.
Live volume is manual and initially conservative. No whole-clip RMS matching is
possible for arbitrary live playing. Eight held notes are supported.

The worklet linearly interpolates four substeps and averages their outputs.
This is a modest exploratory resampling scheme, not the offline polyphase
resampler and not an alias-free guarantee. Further spectral and latency
measurement is needed before production use or a real-time C++/VST3 version.

PCLA calibration identified that the original sag formula tends to gain*x as
drive approaches zero, whereas its exact-zero branch bypasses. The live version
multiplies depth by smoothstep(min(1, drive/2)); it reaches the frozen depth at
drive 2 and above and tends smoothly to zero below it. Twenty-millisecond
parameter smoothing reduces abrupt control changes. The original fixed-strength
renders remain untouched. A common 8 Hz DC blocker follows the nonlinear stage.

## Validation

`node test-live.cjs` executes the actual worklet in a stub audio environment.
Passed: 18 finite bounded renders across drives and depths, silent startup,
zero-drive continuity and conditioned-history response. JavaScript syntax checks
passed. Chrome loaded the page and worklet, advanced through phrase note events,
showed audio activity, and handled Stop. These observations do not establish
perceived audio quality. Hardware MIDI and end-to-end latency remain untested;
no MIDI permission was accepted on Holger's behalf.

Browser requirements:
https://www.w3.org/TR/webmidi/
https://developer.mozilla.org/en-US/docs/Web/API/AudioWorklet
