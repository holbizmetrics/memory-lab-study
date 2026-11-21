# Independent pulse-width modulation

Open **OSC → OSC 1 PWM** and raise **OSC 1 PULSE** to hear the new routes.
Each part has six signed depths for LFO 1–4 and ENV 3–4. The page also shows
OSC 1's base width and pulse level. LFO timing, shape, polarity and retrigger
come from the existing MOTION pages. Envelope timing comes from ENVELOPES.

The routes add to OSC 1's base width, with the final width limited to 0.1–0.9.
Depths ramp over the existing smoothing interval. The old LFO and envelope
pulse-width controls still address OSC 2. For an isolated test, enable just
one pulse source at a time on the MIX page.

The 24 new parameters follow the existing 473 in the host table. Older
patches and project states load with the new routes at zero. INIT and factory
sounds reset them, copying a part includes them, and DICE preserves them.
Existing controls retain their IDs, order, and ranges.

This stage implements independent PWM. Hardware sync, cross modulation and
the production Relic's exact modulation scales require separate investigation.
