# Optional per-voice post-filter drive: first listening study

The creator interviews describe gain and clipping at each voice's output VCA.
This study asks whether adding a simple post-filter stage is musically useful.
It is not a reconstruction of that VCA circuit or the complete hardware path.

The experiment compiles only in the separate memorylab_voice_drive executable,
with MEMORYLAB_EXPERIMENTAL_VOICE_DRIVE=1. The plugin and standard native target
do not compile the extra stage. No host parameters, patches or installed files
are changed. The alpha.2 release tag remains the preserved baseline.

## Declared model and listening conditions

The enabled stage multiplies each filtered voice by 10^(dB/20), then hard clips
to [-1,1] before voices are summed. A disabled stage bypasses this operation.
The amplitude envelope remains in its existing pre-filter position; this is not
a complete post-filter envelope/VCA rearrangement. Clipping is at the render
sample rate, without oversampling. Do not claim hardware similarity or controlled
aliasing from these clips.

Two fixtures use a single E3 note or an E3/G3/B3 chord. They share velocity,
amplitude settings and a 120 Hz to 6 kHz classic low-pass sweep, Q 2.72. This
does not implement the modern BP4/X filter. Each fixture compares bypass, +8 dB
and +24 dB. Shared drive and memory depth are zero to isolate the experiment;
the existing output DC filter and output gain remain active.

The six three-second listening clips are independently matched over their full
duration to -24 dBFS RMS. Raw renders and applied gains are retained. RMS matching
does not establish equal perceived loudness. Both enabled settings reach the
clip threshold in the 48 kHz fixtures, with counts in measurements.csv.

## Results and artifacts

Run outputs/voice-drive-study-2 contains index.html, six matched WAVs, six raw WAVs,
measurements.csv and verification.json with audio/source/executable hashes.
The earlier study-1 run is retained; study-2 adds explicit clip counts.

All 18 fixture/gain/sample-rate cases at 44.1/48/96 kHz were finite, released their
voices and matched exactly between 512- and 127-sample blocks. Enabled modes
differed from bypass and +24 dB reached clipping in every fixture/rate case.
Native core and arp checks also passed. These are bounded checks, not allocation,
realtime deadline, aliasing or listening approval for the new stage.

Independent Python decoding verified the 12 saved float WAVs, 48 kHz mono format,
length, finite samples and six matched clips within .001 dB of the RMS target,
with peak headroom. The player has six explicit audio controls and no autoplay.
The first HTML check mistakenly matched the explanatory text about autoplay;
it was corrected to parse actual audio element attributes, and passed.

## Listening feedback, 2026-10-29

Holger listened to the comparison and reported that all settings sounded clear
and good, with no perceived flattening or distortion where it should not be.
No preferred gain setting or separate note/chord ranking was specified. This is
positive subjective feedback on the supplied clips, not a rejection of +24 dB.
It supports retaining optional per-voice drive as a candidate for integration,
with the existing sound available at the default/bypass setting.

Next: define the host control, bypass and parameter/patch compatibility contracts
before integrating the experimental stage into the plugin.
Keep the approved shared memory drive available regardless of this study's outcome.

## Reproduce

```powershell
cmd /c native\build-native.cmd
outputs/native-build/memorylab_voice_drive.exe outputs/voice-drive-study-NEW
py -3 native/voice-drive-verify.py outputs/voice-drive-study-NEW
```

Choose a new folder: rendering and verification refuse to overwrite prior runs.
