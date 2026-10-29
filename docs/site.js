"use strict";
const views = {
  voice: ["Voice editor", "Shape two oscillators, sweep the classic filter, and explore shared drive, memory and recovery."],
  motion: ["Motion editor", "Bring in vibrato, pulse-width movement, cutoff motion and filter-envelope depth."],
  amplitude: ["Amplitude envelope editor", "Set the amplitude contour from delay and attack through sustain and release. Stages latch on the next note."],
  "filter-envelope": ["Filter envelope editor", "Shape a separate filter contour and its contribution to cutoff."],
  arp: ["Arpeggiator editor", "Hold notes to explore rhythm, direction, octave range and gate. The first note starts the clock."]
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
