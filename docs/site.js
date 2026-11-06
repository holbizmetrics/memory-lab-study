"use strict";
const views = {
  "motion2": ["Second LFO editor", "An independent second LFO adds pitch, pulse-width and cutoff movement. All depths default to zero."],
  "motion2-clock": ["Second LFO clock editor", "LFO 2 has its own tempo division, note reset/free run and fallback BPM."],
  "presets": ["Starting sounds browser", "Browse eight original sounds without changing audio. LOAD preserves output and bypasses both drives; UNDO restores the previous sound."],
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
