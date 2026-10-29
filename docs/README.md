# Memory Lab Study project site

Buildless HTML/CSS/JavaScript site prepared for GitHub Pages. Serve this directory
as the site root. Relative asset paths support the repository project URL
`https://holbizmetrics.github.io/memory-lab-study/` without a build step.

The site showcases the development editor, six listening-study clips, research
context and the current source prerelease. It includes no plugin binaries,
external fonts, third-party scripts, tracking or manufacturer artwork. Screenshots
come from our editor; the audio comes from our original offline drive study.

## Hosting status

Not published. The repository is private and the Pages endpoint returned 404 when
queried on 2026-10-29. The account API did not expose a plan name, so eligibility
for private-repository Pages remains unconfirmed. GitHub requires an appropriate
paid plan for Pages from a private repository; see
https://docs.github.com/en/pages/getting-started-with-github-pages/what-is-github-pages .

Publishing a normally public Pages site is distinct from publishing the repository.
Do not change repository visibility to work around eligibility. The site explicitly
marks repository/notebook/release links as requiring repository access.

After approval to publish this site publicly, configure Pages to deploy from the
main branch's /docs folder if supported by the account. `.nojekyll` bypasses Jekyll.
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
