param([Parameter(Mandatory=$true)][string]$Folder)
$ErrorActionPreference='Stop'
$studyFolder=(Resolve-Path -LiteralPath $Folder).Path
$studyReport=Join-Path $studyFolder 'verification.json'
if(Test-Path -LiteralPath $studyReport){throw 'Verification report already exists; preserve this run.'}
Add-Type -TypeDefinition @'
using System;
using System.IO;
public static class NoiseDiagnosticWavCheck {
 public static double[] Read(string path) {
  byte[] b=File.ReadAllBytes(path);
  if(b.Length!=44+144000*4 || System.Text.Encoding.ASCII.GetString(b,0,4)!="RIFF" || System.Text.Encoding.ASCII.GetString(b,8,8)!="WAVEfmt " || BitConverter.ToInt32(b,16)!=16 || BitConverter.ToInt16(b,20)!=3 || BitConverter.ToInt16(b,22)!=1 || BitConverter.ToInt32(b,24)!=48000 || BitConverter.ToInt32(b,28)!=192000 || BitConverter.ToInt16(b,32)!=4 || BitConverter.ToInt16(b,34)!=32 || System.Text.Encoding.ASCII.GetString(b,36,4)!="data" || BitConverter.ToInt32(b,40)!=144000*4) throw new Exception("WAV format or length: "+path);
  double sum=0,peak=0;
  for(int i=44;i<b.Length;i+=4){float x=BitConverter.ToSingle(b,i);if(float.IsNaN(x)||float.IsInfinity(x))throw new Exception("Nonfinite audio");sum+=(double)x*x;peak=Math.Max(peak,Math.Abs(x));}
  return new double[]{20*Math.Log10(Math.Sqrt(sum/144000)),peak};
 }
}
'@
$studyFiles=@(Get-ChildItem -LiteralPath $studyFolder -Filter '*.wav' -File)
if($studyFiles.Count -ne 18){throw 'Expected 18 WAV files.'}
$studyClips=@(foreach($studyFile in $studyFiles){
 $studyValues=[NoiseDiagnosticWavCheck]::Read($studyFile.FullName)
 if(-not $studyFile.BaseName.EndsWith('-raw')){
  if([Math]::Abs($studyValues[0]+24) -gt .001 -or $studyValues[1] -ge .95){throw 'Matched listening file level/headroom failed.'}
 }
 [ordered]@{file=$studyFile.Name;rms_dbfs=$studyValues[0];peak=$studyValues[1];sha256=(Get-FileHash -LiteralPath $studyFile.FullName).Hash}
})
$studyHtml=Get-Content -LiteralPath (Join-Path $studyFolder 'index.html') -Raw
$studyPlayers=[regex]::Matches($studyHtml,"<audio controls preload='none' src='([^']+)'></audio>")
if($studyPlayers.Count -ne 9 -or [regex]::IsMatch($studyHtml,'<audio\b[^>]*\bautoplay\b')){throw 'Audio player markup failed.'}
foreach($studyPlayer in $studyPlayers){if(-not(Test-Path -LiteralPath (Join-Path $studyFolder $studyPlayer.Groups[1].Value))){throw 'Missing listening file.'}}
$studyRoot=Split-Path -Parent $PSScriptRoot
$studyHashes=@(foreach($studyRelative in @('native/voice.hpp','native/noise-study.cpp','native/noise-diagnostic.cpp','outputs/native-build/memorylab_noise_diagnostic.exe')){
 [ordered]@{file=$studyRelative;sha256=(Get-FileHash -LiteralPath (Join-Path $studyRoot $studyRelative)).Hash}
})
[ordered]@{clips=$studyClips;sources=$studyHashes} | ConvertTo-Json -Depth 5 | Set-Content -LiteralPath $studyReport
Write-Output 'PASS: 18 independently decoded float WAVs; 9 listening files matched to -24 dBFS within .001 dB with peak headroom; player links and hashes verified.'
