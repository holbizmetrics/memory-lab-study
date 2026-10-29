# Project working instructions

- Preserve the approved default sound, existing host parameter IDs/order, and
  compatibility with older patches when adding synth features.
- Every numbered synth version that is verified and delivered to Holger must
  receive an annotated tag and a published GitHub prerelease as part of that
  milestone. This is a standing user instruction; do not wait for another reminder.
- Use `vX.Y.0-alpha.N`, tag the exact verified source commit, and never move an
  existing tag. Documentation-only commits do not require a new version release.
- Write release notes with features, compatibility, validation and remaining
  limits. Distinguish technical checks from Holger's musical listening approval.
- Releases remain source-only, without binary assets, until distribution
  licensing is selected. Verify the published prerelease flag, tag and assets.
- Do not replace the installed plugin while FL Studio or the standalone is open.
  Back up and verify the previous bundle before an authorized local update.
- Active FL crash investigation remains parked until recurrence. Do not automate
  FL through UI Automation or attach a debugger as routine feature verification.
