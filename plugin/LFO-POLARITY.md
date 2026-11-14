# LFO polarity — 0.21

Each part now has independent BIPOLAR/UNIPOLAR controls for LFOs 1–4 on
MOTION / CLOCK 1–4. Shape controls retain their existing four values and host
ranges. Four shapes times two polarities provide eight waveform choices.
The right output fader remains visible; polarity replaces the duplicate
OUTPUT knob on the clock pages.

Bipolar sources retain their original -1…+1 math. Unipolar maps the waveform
to 0…1: `(bipolar + 1) / 2`. Positive pitch/width/cutoff depths therefore move
only above the base setting; negative depths move only below. This changes
the mean and halves peak-to-peak excursion for the same depth; it is intentional.
Rate, note-reset/free-running phase and clock behavior are unchanged.
Live polarity changes crossfade over the existing 20 ms smoothing time constant;
they do not reset oscillator or LFO phase. The preview shows the target waveform,
not the momentary smoothing transition. Clamps on destination controls still apply.

AMP DEPTH is an attenuation-only route. It normalizes either source polarity
over its full cycle, preserving the existing `1 - abs(depth) * (0.5 + 0.5*wave)`
law (with phase reversal for negative depths). Polarity alone consequently
does not change tremolo. This explicit software policy avoids unexpected
half-depth/DC attenuation; no hardware amplitude-law equivalence is claimed.

## Compatibility

Append 16 host controls after the original 409, in part-major order:
`lfoUnipolar`, `lfo2Unipolar`, `lfo3Unipolar`, `lfo4Unipolar`, then the same
IDs with `part2__`, `part3__`, `part4__` prefixes. All default to bipolar (0).
There are 425 controls. None of the first 409 IDs/order/ranges changes.
Patch schema 16 / host state 18 store the full setup. Older patches and host
states restore bipolar for every source, including in an already edited instance.
New-format files require the newer plugin. INIT and starter sounds reset
polarity; selected-part starter loading and Copy Part 1 include these settings.
DICE preserves polarity, retaining its prior randomization scope; undo preserves
the complete patch. No hardware modulation matrix or extra source added here.

## Verification scope

`--polarity-test` checks all shape/polarity mappings, appended IDs, migrations,
patch/host recall, copy/starter isolation, four-rate multipart held-note polarity
edits and block parity, four independent cutoff-route effects, exact tremolo
invariance and CLOCK page bindings. Existing native/plugin tests and actual
VST3 preservation captures remain separate gates. Holger's musical approval
is pending until the new version is delivered and played.

Manufacturer basis: [Relic specs](https://www.shearelectronics.com/relic/specs/)
list four LFOs, with unipolar and bipolar triangle/sine/pulse/saw forms.
Their exact starting phase, ranges and destination laws are not published here.
