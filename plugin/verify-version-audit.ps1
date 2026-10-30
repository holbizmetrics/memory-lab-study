param([string]$Folder='outputs/distortion-audit-0.8-1')
$ErrorActionPreference='Stop'
$auditRoot=Split-Path -Parent $PSScriptRoot
$auditFolder=Join-Path $auditRoot $Folder
if(Test-Path -LiteralPath (Join-Path $auditFolder 'verification.json')){throw 'Verification report exists; preserve the run.'}
Add-Type -TypeDefinition @'
using System;
using System.IO;
public static class VersionAudioAudit {
 public static double[] Read(string path){byte[] b=File.ReadAllBytes(path);if(b.Length!=144000*4)throw new Exception("Wrong capture size");double sum=0,peak=0,over=0;for(int i=0;i<b.Length;i+=4){float x=BitConverter.ToSingle(b,i);if(float.IsNaN(x)||float.IsInfinity(x))throw new Exception("Nonfinite capture");sum+=(double)x*x;peak=Math.Max(peak,Math.Abs(x));if(Math.Abs(x)>=1)over++;}return new double[]{Math.Sqrt(sum/144000),peak,over};}
 public static void Wave(string input,string output,double gain){byte[] b=File.ReadAllBytes(input);using(var w=new BinaryWriter(File.Create(output))){w.Write(System.Text.Encoding.ASCII.GetBytes("RIFF"));w.Write(36+b.Length);w.Write(System.Text.Encoding.ASCII.GetBytes("WAVEfmt "));w.Write(16);w.Write((short)3);w.Write((short)1);w.Write(48000);w.Write(192000);w.Write((short)4);w.Write((short)32);w.Write(System.Text.Encoding.ASCII.GetBytes("data"));w.Write(b.Length);double sum=0,peak=0;for(int i=0;i<b.Length;i+=4){float x=(float)(BitConverter.ToSingle(b,i)*gain);sum+=(double)x*x;peak=Math.Max(peak,Math.Abs(x));w.Write(x);}if(Math.Abs(20*Math.Log10(Math.Sqrt(sum/144000))+24)>.001||peak>=.95)throw new Exception("Matched WAV level failed");}}
}
'@
$auditRows=@(foreach($auditMode in @('default','clean','stress')){
 $auditBaseline=(Get-FileHash -LiteralPath (Join-Path $auditFolder ('0.4/'+$auditMode+'.f32'))).Hash
 foreach($auditVersion in @('0.4','0.6','0.7','0.8')){
  $auditPath=Join-Path $auditFolder ($auditVersion+'/'+$auditMode+'.f32')
  $auditHash=(Get-FileHash -LiteralPath $auditPath).Hash
  if($auditHash -ne $auditBaseline){throw "Version audio mismatch: $auditVersion / $auditMode"}
  $auditStats=[VersionAudioAudit]::Read($auditPath)
  $auditLog=Get-Content -LiteralPath (Join-Path $auditFolder ($auditVersion+'/'+$auditMode+'.log')) -Raw
  [ordered]@{version=$auditVersion;mode=$auditMode;sha256=$auditHash;matches_04_exactly=$true;rms=$auditStats[0];peak=$auditStats[1];samples_above_full_scale=$auditStats[2];processing_ms=[double]([regex]::Match($auditLog,'processing_ms=([\d.]+)').Groups[1].Value);worst_block_budget_ratio=[double]([regex]::Match($auditLog,'worst_block_budget_ratio=([\d.]+)').Groups[1].Value)}
 }
})
$auditHtml="<!doctype html><meta charset='utf-8'><title>Version distortion check</title><style>body{font:18px system-ui;background:#101819;color:#e5eee8;max-width:850px;margin:40px auto;padding:16px}section{background:#1a2729;padding:16px;margin:16px 0}audio{width:100%}</style><h1>Controlled version comparison</h1><p>Actual preserved VST3 plugins, same MIDI chord. Within each setting, 0.4/0.6/0.7/0.8 produced byte-identical audio. These are fresh instances, not your current FL patch.</p><p>Default uses the original shared drive 5 and memory .8. Drive-bypassed uses Classic, both drives 0, memory 0 and noise 0. Filter cutoff and Q retain the original defaults. Clips matched to -24 dBFS RMS for listening; original raw captures and settings retained.</p>"
foreach($auditMode in @('default','clean')){foreach($auditVersion in @('0.6','0.8')){
 $auditPath=Join-Path $auditFolder ($auditVersion+'/'+$auditMode+'.f32');$auditStats=[VersionAudioAudit]::Read($auditPath)
 $auditName=$auditVersion+'-'+$auditMode+'.wav';$auditGain=[Math]::Pow(10,-24/20)/$auditStats[0]
 [VersionAudioAudit]::Wave($auditPath,(Join-Path $auditFolder $auditName),$auditGain)
 $auditLabel=if($auditMode -eq 'clean'){'Drive bypassed'}else{'Original default drive'}
 $auditHtml+="<section><h2>$auditVersion / $auditLabel</h2><audio controls preload='none' src='$auditName'></audio></section>"
}}
$auditHtml+="<script>document.addEventListener('play',e=>{document.querySelectorAll('audio').forEach(a=>{if(a!==e.target)a.pause()})},true)</script>"
$auditHtml | Set-Content -LiteralPath (Join-Path $auditFolder 'index.html')
$auditRows | ConvertTo-Json -Depth 5 | Set-Content -LiteralPath (Join-Path $auditFolder 'verification.json')
Write-Output 'PASS: all 12 captures finite; every version byte-identical within default/drive-bypassed/eight-voice groups; four listening WAVs level-checked.'
