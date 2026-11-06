"""Actual VST3 isolation and combined-source fixtures for the second LFO."""
from pathlib import Path
import json, math, struct, subprocess, wave, xml.etree.ElementTree as ET

ROOT=Path(__file__).resolve().parents[1]
OUT=ROOT/'outputs/lfo2-study-0.14'; OUT.mkdir(parents=True,exist_ok=True)
SMOKE=ROOT/'outputs/plugin-build/MemoryLabSmoke_artefacts/Release/MemoryLabSmoke.exe'
EXE=ROOT/'outputs/plugin-build/MemoryLabValidate_artefacts/Release/MemoryLabValidate.exe'
BUNDLE=ROOT/'outputs/plugin-build/MemoryLab_artefacts/Release/VST3/Memory Lab Study.vst3'
subprocess.run([str(SMOKE),'--factory-patches',str(OUT/'bank')],check=True)
base=ET.parse(OUT/'bank/1.mlpatch').getroot()
conditions=[('clean',{},'default'),('inactive-clock',{'lfo2Shape':3,'lfo2Sync':1,'lfo2Retrigger':0,'lfo2Division':5},'default'),
 ('lfo1-pitch',{'vibrato':8,'lfoRate':4},'default'),
 ('lfo2-filter',{'lfo2Cutoff':1,'lfo2Rate':.7},'default'),
 ('both',{'vibrato':8,'lfoRate':4,'lfo2Cutoff':1,'lfo2Rate':.7},'default'),
 ('both-eight-notes',{'vibrato':8,'lfoRate':4,'lfo2Cutoff':1,'lfo2Rate':.7},'stress'),
 ('lfo2-pwm',{'lfo2Pwm':.12,'lfo2Rate':1.3,'lfo2Shape':1},'default'),
 ('lfo2-pitch',{'lfo2Pitch':8,'lfo2Rate':4},'default'),
 ('lfo2-clock',{'lfo2Cutoff':1,'lfo2Sync':1,'lfo2Division':1,'lfo2Bpm':120},'default')]
rows=[]
for name,settings,mode in conditions:
    root=ET.fromstring(ET.tostring(base))
    values={'cutoff':1000,'resonance':.8,**settings}
    for entry in root:
        if entry.get('id') in values:entry.set('value',str(values[entry.get('id')]))
    stem=OUT/name;ET.ElementTree(root).write(stem.with_suffix('.mlpatch'),encoding='unicode')
    result=subprocess.run([str(EXE),'--capture-patch',str(BUNDLE),str(stem),str(stem.with_suffix('.mlpatch')),mode],capture_output=True,text=True,check=True)
    stem.with_suffix('.log').write_text(result.stdout+result.stderr,encoding='utf-8')
    raw=stem.with_suffix('.f32').read_bytes();samples=struct.unpack('<'+'f'*(len(raw)//4),raw)
    assert all(math.isfinite(v) and abs(v)<1 for v in samples),name
    rms=math.sqrt(sum(v*v for v in samples)/len(samples));assert rms>1e-6,name
    with wave.open(str(stem.with_suffix('.wav')),'wb') as f:
        f.setnchannels(1);f.setsampwidth(2);f.setframerate(48000);f.writeframes(struct.pack('<'+'h'*len(samples),*(round(v*32767) for v in samples)))
    rows.append(dict(name=name,peak=max(map(abs,samples)),rms=rms))
assert (OUT/'clean.f32').read_bytes()==(OUT/'inactive-clock.f32').read_bytes(),'Inactive LFO 2 changed audio'
for name in ['lfo1-pitch','lfo2-filter','both','lfo2-pwm','lfo2-pitch','lfo2-clock']:
    assert (OUT/f'{name}.f32').read_bytes()!=(OUT/'clean.f32').read_bytes(),f'{name} had no effect'
for name in ['lfo1-pitch','lfo2-filter']:
    assert (OUT/'both.f32').read_bytes()!=(OUT/f'{name}.f32').read_bytes(),'Combined source disappeared'
(OUT/'measurements.json').write_text(json.dumps(rows,indent=2),encoding='utf-8')
cards=''.join(f'<article><h2>{r["name"]}</h2><audio controls preload="none" src="{r["name"]}.wav"></audio><p><a href="{r["name"]}.mlpatch">Patch</a></p></article>' for r in rows)
(OUT/'index.html').write_text('''<!doctype html><html lang="en"><meta charset="utf-8"><meta name="viewport" content="width=device-width,initial-scale=1"><title>0.14 Two independent LFOs</title><style>body{background:#101819;color:#e5eee8;font:18px system-ui;max-width:850px;margin:40px auto;padding:20px}article{background:#1a2729;padding:20px;margin:16px 0}audio{width:100%}a{color:#b9e7cb}</style><h1>Two independent LFOs</h1><p>Actual VST3 captures. E-minor chord except the labelled eight-note stress clip. Both drives zero, output 0.25; no loudness normalization. Compare fast LFO 1 pitch, slow LFO 2 cutoff and both together. Technical fixtures; musical approval pending.</p>'''+cards+'</html>',encoding='utf-8')
print(f'PASS: nine actual VST3 fixtures; inactive parity, three active routes and combined sources; max peak={max(r["peak"] for r in rows):.6f}')
