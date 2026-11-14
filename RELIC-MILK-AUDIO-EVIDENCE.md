# Milk Audio Store Relic demonstration — supplied transcript

Video: https://www.youtube.com/watch?v=NeCRJ6rGjK4

Holger supplied the title and untimestamped transcript on 2026-11-14:
“Shear Electronics RELIC bi-timbral 8-voice polyphonic analog synthesizer
inspired by Oberheim OB-X.” He reports the upload as about four months old.
Exact upload date, recording date and transcript-to-audio accuracy have not
been independently verified. A relative upload age does not date the demo.
This is one supplied transcript, not independent corroboration of every claim.

## Developer statements extracted from the supplied text

| Statement | Implementation relevance | Boundary |
| --- | --- | --- |
| Eight voices, two VCOs per voice, one continuously variable VCF per voice | Supports the later architecture target | One filter with different responses does not mean multiple independent serial filters selected by the player. Internal circuit sections remain possible. |
| Four-pole BP blends through an X midpoint to two-pole LP | Agrees with the modern profile direction | No interpolation equation or measured response supplied. |
| Profile is voltage-controllable and a modulation destination | Supports proposed profile modulation | “Everything is a mod destination” is broad speech, not a complete source/destination matrix. |
| BP can be wide or narrow; a preferred setting is about 4.4 | Confirms a separate bandwidth adjustment | Units and range are not given; do not interpret 4.4 as octaves, Q or Hz. No LP bandwidth effect is established by this passage. |
| Gain setting 7 is +8 dB; additional settings reach +12/+16/+20 dB; setting 11 is +24 dB | Gives useful drive-control anchor points | Does not establish every setting, taper, reference level or full gain law. |
| At the high setting there is substantial hard clipping at the voice-core output VCA; profile sweep is repeated with higher gain | Strong support for per-voice post-filter clipping and a useful comparison fixture | Does not establish threshold, transfer curve, oversampling requirement or shared memory behavior. Our 0–24 dB voice-drive control is conceptually aligned; not proven calibrated to hardware. |
| Four-way multitimbral; layers and split demonstrated | Supports implemented part architecture | Title says bi-timbral but spoken account says four-way. Do not reduce the target from the title. Exact quotas/stealing rules absent. |
| Speaker calls the displayed machines production units one and two and says shipping began two days earlier | Identifies the speaker's stated development stage | Recording date and exact hardware/firmware IDs are unknown; cannot date shipping from upload age. |

## Transcription and interpretation cautions

The opening says the classic SEM goes from two-pole low-pass to four-pole
band-pass, then immediately describes the added four-pole BP/LP sweep. That
opening conflicts with the current written Classic HP2/NF2/LP2 specification.
It may be a spoken slip or transcription error; do not use it to redefine
Classic or claim a new filter revision. Later modern-profile discussion is
clearer and agrees with the current written overview.

The transcript renders Shear as “Shire,” including its email domain. Do not
copy that as contact information. Use the manufacturer's independently
published site for any contact details.

No timestamps or exact parameter settings for the complete sound were supplied.
The transcript describes musical examples but does not substitute for hearing
or measuring them. Gain anchors and the explicit clipping attribution are the
main additions to our evidence; they do not justify changing the approved DSP.
