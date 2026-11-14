"use strict";
const views = {
"lfo-polarity": ["One-sided or centered motion","Each CLOCK page offers independent bipolar/unipolar polarity for the selected part. Old sounds remain bipolar; new listening approval is pending."],
"parts-split": ["Parts and keyboard split","Reserve four voices below the chosen MIDI note and four above it; edit independent sounds and MIDI inputs."],
"parts-layer4": ["Four layered parts","Four sounds share eight voices, with two voices reserved per part. Selection changes which sound the editor controls."],
"osc-mix": ["Two-core oscillator mixer","Mix saw and pulse from each core, with separate base widths. Added sources default off; existing PWM affects oscillator 2."],
"env3": ["Envelope 3 editor","Independent dAHDSR stages with pitch and filter depths."],
"env4": ["Envelope 4 editor","Independent dAHDSR stages with pitch and filter depths."],
"envelope-routes": ["Envelope routing editor","Pitch from amplitude/filter contours and pulse-width movement from ENV 3/4. Zero disables each route."],
"motion3": ["LFO 3 editor","An independent modulation source for pitch, pulse width and filter cutoff. Added depths default to zero."],
"motion3-clock": ["Clock 3 editor","Independent tempo division, note reset/free run and fallback BPM for LFO 3."],
"motion4": ["LFO 4 editor","An independent modulation source for pitch, pulse width and filter cutoff. Added depths default to zero."],
"motion4-clock": ["Clock 4 editor","Independent tempo division, note reset/free run and fallback BPM for LFO 4."],
  "motion2": ["Second LFO editor", "An independent second LFO adds pitch, pulse-width and cutoff movement. All depths default to zero."],
  "motion2-clock": ["Second LFO clock editor", "LFO 2 has its own tempo division, note reset/free run and fallback BPM."],
  "presets": ["Starting sounds browser", "Browse passively or enable optional auto-load on selection. Categories stay passive; loads preserve output and bypass drives. UNDO restores the previous sound."],
  "voice": [
    "Voice sound editor",
    "Shape two oscillators, sweep the classic filter, and explore shared drive, memory and recovery."
  ],
  "expression": [
    "Voice expression editor",
    "Set filter key tracking and independent amplitude/filter-envelope velocity sensitivities. Response curves preview the mappings."
  ],
  "osc": [
    "Oscillator editor",
    "Mix independent saw and pulse sources, tune oscillator two, and blend per-voice white noise before the filter and drive."
  ],
  "motion": [
    "Motion editor",
    "Choose sine, triangle, saw or pulse movement for pitch, pulse width and cutoff."
  ],
  "motion-clock": [
    "Motion clock editor",
    "Sync movement to host tempo or use an internal fallback, with rhythmic divisions and note-retrigger or free-running phase."
  ],
  "amplitude": [
    "Amplitude envelope editor",
    "Set delay, attack, hold, decay, sustain and release. Envelope stages latch on the next note."
  ],
  "filter-envelope": [
    "Filter envelope editor",
    "Shape an independent six-stage filter contour and its contribution to cutoff."
  ],
  "filter-classic": [
    "Filter editor",
    "Explore Classic, LP4, BP4 and Blend families. Classic profile operates in the Classic family."
  ],
  "arp": [
    "Arpeggiator editor",
    "Explore rhythmic rates, up/down/up-down order, octave layers and gate length."
  ],
  "drive": [
    "Drive editor",
    "Intentional distortion: per-voice clipping after the filter, followed by shared memory drive after the voices mix. Zero bypasses each drive."
  ],
  "dice-bypass": [
    "Preset randomizer editor",
    "Roll bounded sounds, lock sections, undo the last roll, or bypass both drives on the next roll. Output and expression settings are preserved."
  ]
};
const image = document.querySelector("#editor-image");
document.querySelectorAll("[data-view]").forEach(button => button.addEventListener("click", () => {
  const key = button.dataset.view;
  image.src = `assets/editor/${key}.png`;
  image.alt = `Memory Lab Study ${views[key][0]}, development build`;
  document.querySelector("#editor-caption").textContent = views[key][1];
  document.querySelectorAll("[data-view]").forEach(item => item.setAttribute("aria-pressed", String(item === button)));
}));
const players = [...document.querySelectorAll("audio")];
players.forEach(player => player.addEventListener("play", () => players.forEach(other => { if (other !== player) other.pause(); })));
document.querySelectorAll("[data-phrase]").forEach(button => button.addEventListener("click", () => {
  const phrase = button.dataset.phrase;
  ["bypass", "8db", "24db"].forEach((mode, index) => {
    const player = players[index]; player.pause();
    player.src = `assets/audio/${phrase}-${mode}.wav`;
    player.setAttribute("aria-label", `${phrase === "single" ? "Single note" : "E-minor chord"}, ${mode === "bypass" ? "extra drive bypassed" : mode === "8db" ? "8 decibels of extra drive" : "24 decibels of extra drive"}`);
    const download = player.querySelector("a"); if (download) download.href = player.src;
    player.load();
  });
  document.querySelector("#phrase-description").textContent = `${phrase === "single" ? "A SINGLE E NOTE" : "AN E-MINOR CHORD"} · 3 SECONDS · 48 KHZ`;
  document.querySelectorAll("[data-phrase]").forEach(item => item.setAttribute("aria-pressed", String(item === button)));
}));
