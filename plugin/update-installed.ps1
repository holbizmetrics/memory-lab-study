$ErrorActionPreference='Stop'
$studyRoot=Split-Path -Parent $PSScriptRoot
$studySource=Join-Path $studyRoot 'outputs/plugin-build/MemoryLab_artefacts/Release/VST3/Memory Lab Study.vst3'
$studyInstalled='C:/Program Files/Common Files/VST3/Memory Lab Study.vst3'
if (Get-Process -Name FL64,FL,'Memory Lab Study' -ErrorAction SilentlyContinue) { throw 'Close FL Studio and the standalone app before updating the loaded plug-in.' }
if (-not (Test-Path -LiteralPath $studyInstalled -PathType Container)) { throw 'The expected first installation is missing.' }
$studyPrior=Get-Content -LiteralPath (Join-Path $studyRoot 'outputs/first-plugin-build.json') -Raw | ConvertFrom-Json
$studyBinaryRelative='Contents/x86_64-win/Memory Lab Study.vst3'
if ((Get-FileHash -LiteralPath (Join-Path $studyInstalled $studyBinaryRelative)).Hash -ne $studyPrior.installed_sha256) { throw 'Installed binary differs from the verified first build. Review before overwriting.' }
$studyBackup=Join-Path $studyRoot 'outputs/installed-backup-0.1.0'
if (Test-Path -LiteralPath $studyBackup) { throw 'Backup already exists. Refusing to overwrite history.' }
Copy-Item -LiteralPath $studyInstalled -Destination $studyBackup -Recurse
foreach ($studyFile in Get-ChildItem -LiteralPath $studyInstalled -File -Recurse) {
 $studyRelative=$studyFile.FullName.Substring($studyInstalled.Length).TrimStart([char[]]'\/')
 if ((Get-FileHash -LiteralPath $studyFile.FullName).Hash -ne (Get-FileHash -LiteralPath (Join-Path $studyBackup $studyRelative)).Hash) { throw 'Backup verification failed.' }
}
foreach ($studyFile in Get-ChildItem -LiteralPath $studySource -File -Recurse) {
 $studyRelative=$studyFile.FullName.Substring($studySource.Length).TrimStart([char[]]'\/')
 $studyTarget=Join-Path $studyInstalled $studyRelative
 New-Item -ItemType Directory -Path (Split-Path -Parent $studyTarget) -Force | Out-Null
 Copy-Item -LiteralPath $studyFile.FullName -Destination $studyTarget -Force
 if ((Get-FileHash -LiteralPath $studyFile.FullName).Hash -ne (Get-FileHash -LiteralPath $studyTarget).Hash) { throw "Update verification failed: $studyRelative" }
}
Write-Output "Updated and verified: $studyInstalled; prior build retained at $studyBackup"
