# Memory Lab Study project site

Buildless HTML/CSS/JavaScript site prepared for GitHub Pages. Serve this directory
as the site root. Relative asset paths support the repository project URL
`https://holbizmetrics.github.io/memory-lab-study/` without a build step.

The site showcases the development editor, six listening-study clips, research
context and the current source prerelease. It includes no plugin binaries,
external fonts, third-party scripts, tracking or manufacturer artwork. Screenshots
come from our editor; the audio comes from our original offline drive study.

## Hosting status

The first Pages request on 2026-10-29 returned HTTP 422 because the repository
was private and the account plan did not support that configuration. A public
site-only fallback repository was created, but before its hosting was configured,
Holger independently made the original repository public. Publication now uses
the original `holbizmetrics/memory-lab-study` repository, main branch, `/docs`.
The fallback `holbizmetrics/memory-lab-study-site` repository is unused and has
no Pages site configured. Do not use it as a second deployment destination.

The intended URL is `https://holbizmetrics.github.io/memory-lab-study/`.
Publication verified on 2026-10-29: GitHub Pages reported build `built` for
commit 9060a94, and the live root returned HTTP 200 with the expected title and
updated public repository wording. This verifies delivery, not browser UI layout.
`.nojekyll` bypasses Jekyll.
No custom domain or build workflow is necessary. Confirm the returned Pages URL
and successful build before reporting the site as published.

## Behavior and verification

- Five editor buttons switch real screenshots with descriptive captions.
- Single-note/chord selection switches all three audio examples and resets playback.
- Only one example plays at a time; changing the phrase does not autoplay.
- Native audio controls and keyboard-operable buttons; visible focus styles,
  descriptive image/audio names, a skip link and reduced-motion support.
- Single-note audio and chord download links remain available without JavaScript.
- Local HTTP response and JS syntax checked; asset references and WAV hashes checked.
- Browser UI preview was attempted but the browser access host was unavailable.
  Rendered desktop/mobile layout and actual browser media interaction remain unverified.

## Assets

Editor PNGs: outputs/patch-editor-preview (development build, including SAVE/LOAD).
Matched WAVs: outputs/voice-drive-study-2, verified against verification.json.
Three-second mono 48 kHz float WAVs, matched to -24 dBFS RMS. The experimental
drive stage is not compiled into the installed plugin. Raw measurement artifacts
remain ignored under outputs/ and are not published by this site.
