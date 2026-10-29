$ErrorActionPreference='Stop'
$studyRoot=Split-Path -Parent $PSScriptRoot
$studySource=Join-Path $studyRoot 'outputs/plugin-build/MemoryLab_artefacts/Release/VST3/Memory Lab Study.vst3'
$studyInstalled='C:/Program Files/Common Files/VST3/Memory Lab Study.vst3'
if (Get-Process -Name FL64,FL,'Memory Lab Study' -ErrorAction SilentlyContinue) { throw 'Close FL Studio and the standalone app before updating the loaded plug-in.' }
if (-not (Test-Path -LiteralPath $studyInstalled -PathType Container)) { throw 'The expected first installation is missing.' }
$studyBinaryRelative='Contents/x86_64-win/Memory Lab Study.vst3'
if (-not (Test-Path -LiteralPath (Join-Path $studySource $studyBinaryRelative) -PathType Leaf)) { throw 'Build and validate the new VST3 bundle first.' }
$studyBuild=Get-Content -LiteralPath (Join-Path $studyRoot 'outputs/voice-drive-build-0.5.json') -Raw | ConvertFrom-Json
if ((Get-FileHash -LiteralPath (Join-Path $studySource $studyBinaryRelative)).Hash -ne $studyBuild.binary_sha256) { throw 'Source binary differs from the verified 0.5 build.' }
if ((Get-FileHash -LiteralPath (Join-Path $studyInstalled $studyBinaryRelative)).Hash -ne '3956789C66349A455CAFFB440ADEFC886A54883F590D07BB8434091E50139888') { throw 'Installed binary differs from the verified 0.4 build. Review before overwriting.' }
$studyBackup=Join-Path $studyRoot 'outputs/installed-backup-0.4.0'
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
