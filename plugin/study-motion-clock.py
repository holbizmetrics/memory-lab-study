"""Actual VST3 timing fixtures; run after the 0.9 shape-study fixtures exist."""
from pathlib import Path
import json, struct, subprocess, wave
import xml.etree.ElementTree as ET
ROOT=Path(__file__).resolve().parents[1]
OUT=ROOT/'outputs/motion-clock-study-1';OUT.mkdir(parents=True,exist_ok=True)
EXE=ROOT/'outputs/plugin-build/MemoryLabValidate_artefacts/Release/MemoryLabValidate.exe'
BUNDLE=ROOT/'outputs/plugin-build/MemoryLab_artefacts/Release/VST3/Memory Lab Study.vst3'
rows=[]
for name,bpm,division,retrigger in [('quarter-reset',120,0,1),('eighth-reset',60,1,1),('quarter-free',120,0,0),('eighth-fast',120,1,1)]:
 root=ET.parse(ROOT/'outputs/motion-shapes-study-1/sine.mlpatch').getroot();root.set('schemaVersion','7')
 for key,value in {'lfoSync':1,'lfoDivision':division,'lfoRetrigger':retrigger,'lfoBpm':bpm}.items():ET.SubElement(root,'PARAM',id=key,value=str(value))
 patch=OUT/f'{name}.mlpatch';ET.ElementTree(root).write(patch,encoding='unicode');stem=OUT/name
 run=subprocess.run([str(EXE),'--capture-patch',str(BUNDLE),str(stem),str(patch),'default'],capture_output=True,text=True)
 stem.with_suffix('.log').write_text(run.stdout+run.stderr,encoding='utf-8')
 if run.returncode:raise RuntimeError(run.stdout+run.stderr)
 data=stem.with_suffix('.f32').read_bytes();samples=struct.unpack('<'+'f'*(len(data)//4),data)
 with wave.open(str(stem.with_suffix('.wav')),'wb') as f:
  f.setnchannels(1);f.setsampwidth(2);f.setframerate(48000);f.writeframes(struct.pack('<'+'h'*len(samples),*(round(max(-1,min(1,v))*32767) for v in samples)))
 rows.append({'name':name,'bpm':bpm,'division':division,'retrigger':retrigger})
assert (OUT/'quarter-reset.f32').read_bytes()==(OUT/'eighth-reset.f32').read_bytes(),'Equivalent synced tempos differ'
assert (OUT/'quarter-reset.f32').read_bytes()!=(OUT/'quarter-free.f32').read_bytes(),'Free run did not change starting phase'
(OUT/'measurements.json').write_text(json.dumps(rows,indent=2),encoding='utf-8')
cards=''.join(f'<article><h2>{r["name"]}</h2><audio controls src="{r["name"]}.wav"></audio></article>' for r in rows)
(OUT/'index.html').write_text('''<!doctype html><html lang="en"><meta charset="utf-8"><title>Memory Lab 0.10 clock</title><style>body{background:#101819;color:#e5eee8;font:18px system-ui;max-width:850px;margin:40px auto;padding:20px}article{background:#1a2729;padding:20px;margin:16px 0}audio{width:100%}</style><h1>0.10 LFO clock</h1><p>Same E minor chord and sine filter modulation, both drives off. Standalone validator has no host tempo, so FALLBACK BPM supplies the clock. Quarter at 120 and eighth at 60 are the same 2 Hz motion. Free run joins the clock after silence. Eighth at 120 is twice as fast. Identical output gain; no individual normalization.</p>'''+cards+'</html>',encoding='utf-8')
print('PASS actual VST3: equivalent synced tempos byte-identical; free phase differs')
print(OUT/'index.html')
