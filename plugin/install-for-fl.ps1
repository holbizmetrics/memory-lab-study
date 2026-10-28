$ErrorActionPreference = 'Stop'
$studyRoot = Split-Path -Parent $PSScriptRoot
$studyBundle = Join-Path $studyRoot 'outputs/plugin-build/MemoryLab_artefacts/Release/VST3/Memory Lab Study.vst3'
$studyDestination = 'C:/Program Files/Common Files/VST3/Memory Lab Study.vst3'
if (-not (Test-Path -LiteralPath $studyBundle -PathType Container)) { throw 'Build and validate the VST3 bundle first.' }
if (Test-Path -LiteralPath $studyDestination) { throw 'A destination already exists. Refusing to overwrite a possibly loaded plug-in.' }
$studyStandardFolder = Split-Path -Parent $studyDestination
New-Item -ItemType Directory -Path $studyStandardFolder -Force | Out-Null
Copy-Item -LiteralPath $studyBundle -Destination $studyDestination -Recurse
$studySources = Get-ChildItem -LiteralPath $studyBundle -File -Recurse
foreach ($studyFile in $studySources) {
    $studyRelative = $studyFile.FullName.Substring($studyBundle.Length).TrimStart([char[]]'\/')
    $studyCopied = Join-Path $studyDestination $studyRelative
    if ((Get-FileHash -LiteralPath $studyFile.FullName).Hash -ne (Get-FileHash -LiteralPath $studyCopied).Hash) { throw "Copy verification failed: $studyRelative" }
}
Write-Output "Installed and verified: $studyDestination"
