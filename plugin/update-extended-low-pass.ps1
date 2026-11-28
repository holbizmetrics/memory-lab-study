$ErrorActionPreference='Stop'
$studyRoot=Split-Path -Parent $PSScriptRoot
$build=Get-Content -LiteralPath (Join-Path $studyRoot 'outputs/extended-low-pass-build-0.29.json') -Raw | ConvertFrom-Json
$source=Join-Path $studyRoot 'outputs/plugin-build/MemoryLab_artefacts/Release/VST3/Memory Lab Study.vst3'
$installed='C:\Program Files\Common Files\VST3\Memory Lab Study.vst3'
$relative='Contents/x86_64-win/Memory Lab Study.vst3'
$backup=Join-Path $studyRoot 'outputs/installed-backup-0.28.0-alpha.1'

function Assert-HostClosed {
 if(Get-Process -Name FL64,FL,'Memory Lab Study' -ErrorAction SilentlyContinue){
  throw 'Close FL Studio and Memory Lab standalone before updating the plug-in.'
 }
}

function Get-BundleHashes([string]$root) {
 $hashes=@{}
 foreach($file in Get-ChildItem -LiteralPath $root -File -Recurse){
  $name=$file.FullName.Substring($root.Length+1)
  $hashes[$name]=(Get-FileHash -LiteralPath $file.FullName).Hash
 }
 return $hashes
}

function Assert-Hashes($expected,$actual) {
 if($expected.Count -ne $actual.Count){throw 'Bundle file count differs.'}
 foreach($name in $expected.Keys){
  if($actual[$name] -ne $expected[$name]){throw "Bundle file differs: $name"}
 }
}

Assert-HostClosed
if((Get-FileHash -LiteralPath (Join-Path $source $relative)).Hash -ne $build.binary_sha256){
 throw 'Build changed after validation.'
}
if((Get-FileHash -LiteralPath (Join-Path $installed $relative)).Hash -ne $build.previous_installed_sha256){
 throw 'Installed plug-in differs from the verified 0.28 bundle.'
}
$sourceHashes=Get-BundleHashes $source
$previousHashes=Get-BundleHashes $installed
if($sourceHashes.Count -ne $previousHashes.Count){throw 'Installed bundle layout differs from the build.'}
foreach($name in $sourceHashes.Keys){
 if(!$previousHashes.ContainsKey($name)){throw "Installed bundle is missing $name"}
}
if(Test-Path -LiteralPath $backup){
 Assert-Hashes $previousHashes (Get-BundleHashes $backup)
}else{
 Copy-Item -LiteralPath $installed -Destination $backup -Recurse
 Assert-Hashes $previousHashes (Get-BundleHashes $backup)
}
Assert-HostClosed
try{
 foreach($name in $sourceHashes.Keys){
  Copy-Item -LiteralPath (Join-Path $source $name) -Destination (Join-Path $installed $name) -Force
 }
 Assert-Hashes $sourceHashes (Get-BundleHashes $installed)
}catch{
 Assert-HostClosed
 foreach($name in $previousHashes.Keys){
  Copy-Item -LiteralPath (Join-Path $backup $name) -Destination (Join-Path $installed $name) -Force
 }
 Assert-Hashes $previousHashes (Get-BundleHashes $installed)
 throw
}
Write-Output "Installed verified 0.29. Previous bundle backed up at $backup"
