param([string]$Folder='outputs/distortion-audit-0.8-1')
$ErrorActionPreference='Stop'
$auditRoot=Split-Path -Parent $PSScriptRoot
$auditFolder=Join-Path $auditRoot $Folder
if(Test-Path -LiteralPath $auditFolder){throw 'Audit folder already exists; preserve the run.'}
New-Item -ItemType Directory -Path $auditFolder | Out-Null
$auditExe=Join-Path $auditRoot 'outputs/plugin-build/MemoryLabValidate_artefacts/Release/MemoryLabValidate.exe'
$auditVersions=@(
 @{version='0.4';source=(Join-Path $auditRoot 'outputs/installed-backup-0.4.0');hash='3956789C66349A455CAFFB440ADEFC886A54883F590D07BB8434091E50139888'},
 @{version='0.6';source=(Join-Path $auditRoot 'outputs/installed-backup-0.6.0');hash='771008429B9463B747529312BC98207753E4AA38A13DB75FE46246825F906342'},
 @{version='0.7';source=(Join-Path $auditRoot 'outputs/installed-backup-0.7.0');hash='480F6F44DEF566AC415207B4E62F46F61C0624BA12092255AA40C7ECAD94680C'},
 @{version='0.8';source='C:/Program Files/Common Files/VST3/Memory Lab Study.vst3';hash='311BA1E72A3F84CA9546523B2BB4584891D4CF5DD6EE14C77E0313F66E1BCB5F'}
)
foreach($auditVersion in $auditVersions){
 $auditRelative='Contents/x86_64-win/Memory Lab Study.vst3'
 if((Get-FileHash -LiteralPath (Join-Path $auditVersion.source $auditRelative)).Hash -ne $auditVersion.hash){throw 'Unexpected version binary.'}
 $auditVersionFolder=Join-Path $auditFolder $auditVersion.version
 New-Item -ItemType Directory -Path $auditVersionFolder | Out-Null
 $auditBundle=Join-Path $auditVersionFolder 'Memory Lab Study.vst3'
 Copy-Item -LiteralPath $auditVersion.source -Destination $auditBundle -Recurse
 if((Get-FileHash -LiteralPath (Join-Path $auditBundle $auditRelative)).Hash -ne $auditVersion.hash){throw 'Audit copy hash mismatch.'}
 foreach($auditMode in @('default','clean','stress')){
  $auditStem=Join-Path $auditVersionFolder $auditMode
  $auditOutput=& $auditExe --capture $auditBundle $auditStem $auditMode
  if($LASTEXITCODE -ne 0){throw "Capture failed: $($auditVersion.version) / $auditMode"}
  $auditOutput | Set-Content -LiteralPath ($auditStem+'.log')
  Write-Output "$($auditVersion.version): $auditOutput"
 }
}
$auditVersions | ConvertTo-Json | Set-Content -LiteralPath (Join-Path $auditFolder 'versions.json')
Write-Output "Captured preserved plugin copies at $auditFolder. Installed plugin untouched."
