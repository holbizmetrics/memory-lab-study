$ErrorActionPreference='Stop'
$studyRoot=Split-Path -Parent $PSScriptRoot
if(Get-Process -Name FL64,FL,'Memory Lab Study' -ErrorAction SilentlyContinue){throw 'Close FL Studio and the standalone before updating the plugin.'}
$build=Get-Content -LiteralPath (Join-Path $studyRoot 'outputs/vco-build-0.18.json') -Raw | ConvertFrom-Json
$source=Join-Path $studyRoot 'outputs/plugin-build/MemoryLab_artefacts/Release/VST3/Memory Lab Study.vst3'
$installed='C:/Program Files/Common Files/VST3/Memory Lab Study.vst3'
$relative='Contents/x86_64-win/Memory Lab Study.vst3'
if((Get-FileHash -LiteralPath (Join-Path $source $relative)).Hash -ne $build.binary_sha256){throw 'Build changed after validation.'}
if((Get-FileHash -LiteralPath (Join-Path $installed $relative)).Hash -ne $build.previous_installed_sha256){throw 'Installed plugin differs from the verified previous bundle.'}
$backup=Join-Path $studyRoot 'outputs/installed-backup-0.17.0-alpha.1'
if(Test-Path -LiteralPath $backup){throw 'Backup exists; refusing to overwrite history.'}
Copy-Item -LiteralPath $installed -Destination $backup -Recurse
foreach($file in Get-ChildItem -LiteralPath $installed -File -Recurse){
 $name=$file.FullName.Substring($installed.Length).TrimStart([char[]]'\/')
 if((Get-FileHash -LiteralPath $file.FullName).Hash -ne (Get-FileHash -LiteralPath (Join-Path $backup $name)).Hash){throw "Backup mismatch: $name"}
}
foreach($file in Get-ChildItem -LiteralPath $source -File -Recurse){
 $name=$file.FullName.Substring($source.Length).TrimStart([char[]]'\/');$target=Join-Path $installed $name
 New-Item -ItemType Directory -Path (Split-Path -Parent $target) -Force | Out-Null
 Copy-Item -LiteralPath $file.FullName -Destination $target -Force
 if((Get-FileHash -LiteralPath $file.FullName).Hash -ne (Get-FileHash -LiteralPath $target).Hash){throw "Update mismatch: $name"}
}
Write-Output "Installed verified 0.18 oscillator mixer build; 0.17 bundle retained at $backup"
