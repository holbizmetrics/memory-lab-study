"""Actual-module LFO 3/4 isolation, four-source combination and legacy preservation."""
from pathlib import Path
import json, math, struct, subprocess, wave, xml.etree.ElementTree as ET

ROOT=Path(__file__).resolve().parents[1]
OUT=ROOT/'outputs/four-lfo-study-0.15';OUT.mkdir(parents=True,exist_ok=True)
SMOKE=ROOT/'outputs/plugin-build/MemoryLabSmoke_artefacts/Release/MemoryLabSmoke.exe'
EXE=ROOT/'outputs/plugin-build/MemoryLabValidate_artefacts/Release/MemoryLabValidate.exe'
BUNDLE=ROOT/'outputs/plugin-build/MemoryLab_artefacts/Release/VST3/Memory Lab Study.vst3'
subprocess.run([str(SMOKE),'--factory-patches',str(OUT/'bank')],check=True)
base=ET.parse(OUT/'bank/1.mlpatch').getroot()
conditions=[('clean',{},'default'),('inactive-clocks',{'lfo3Shape':3,'lfo3Rate':17,'lfo3Retrigger':0,'lfo4Shape':2,'lfo4Sync':1,'lfo4Division':5},'default')]
for source in [3,4]:
    for route,amount in [('Pitch',8),('Pwm',.12),('Cutoff',1)]:
        conditions.append((f'lfo{source}-{route.lower()}',{f'lfo{source}{route}':amount,f'lfo{source}Rate':.7 if route=='Cutoff' else 3},'default'))
both={'vibrato':8,'lfoRate':4,'lfo2Cutoff':.5,'lfo2Rate':.7,'lfo3Pwm':.06,'lfo3Rate':.35,'lfo4Cutoff':.3,'lfo4Rate':1.1,'lfo4Shape':1}
conditions += [('all-four',both,'default'),('all-four-eight-notes',both,'stress')]
for source in [3,4]:conditions.append((f'lfo{source}-clock',{f'lfo{source}Cutoff':1,f'lfo{source}Sync':1,f'lfo{source}Division':1,f'lfo{source}Bpm':120},'default'))
rows=[]
def capture(stem,patch,mode):
    result=subprocess.run([str(EXE),'--capture-patch',str(BUNDLE),str(stem),str(patch),mode],capture_output=True,text=True,check=True)
    stem.with_suffix('.log').write_text(result.stdout+result.stderr,encoding='utf-8')
    raw=stem.with_suffix('.f32').read_bytes();samples=struct.unpack('<'+'f'*(len(raw)//4),raw)
    assert all(math.isfinite(v) for v in samples),stem.name
    return raw,samples
for name,settings,mode in conditions:
    root=ET.fromstring(ET.tostring(base));values={'cutoff':1000,'resonance':.8,**settings}
    for entry in root:
        if entry.get('id') in values:entry.set('value',str(values[entry.get('id')]))
    stem=OUT/name;ET.ElementTree(root).write(stem.with_suffix('.mlpatch'),encoding='unicode')
    raw,samples=capture(stem,stem.with_suffix('.mlpatch'),mode)
    assert max(map(abs,samples))<1,name
    rms=math.sqrt(sum(v*v for v in samples)/len(samples));assert rms>1e-6,name
    with wave.open(str(stem.with_suffix('.wav')),'wb') as f:
        f.setnchannels(1);f.setsampwidth(2);f.setframerate(48000);f.writeframes(struct.pack('<'+'h'*len(samples),*(round(v*32767) for v in samples)))
    rows.append(dict(name=name,peak=max(map(abs,samples)),rms=rms))
assert (OUT/'clean.f32').read_bytes()==(OUT/'inactive-clocks.f32').read_bytes(),'Inactive LFO 3/4 changed audio'
for name,_,_ in conditions[2:]:assert (OUT/f'{name}.f32').read_bytes()!=(OUT/'clean.f32').read_bytes(),f'{name} had no effect'
legacy=ROOT/'outputs/four-lfo-0.15-baseline/slow-wah.mlpatch'
legacy_status='not available; user fixture not included in public source'
if legacy.exists():
    raw,_=capture(OUT/'slow-wah-migrated',legacy,'default')
    assert raw==legacy.with_suffix('.f32').read_bytes(),'Saved two-LFO sound changed'
    legacy_status='byte-identical to preserved 0.14 user-patch render'
(OUT/'measurements.json').write_text(json.dumps(dict(fixtures=rows,slow_wah=legacy_status),indent=2),encoding='utf-8')
cards=''.join(f'<article><h2>{r["name"]}</h2><audio controls preload="none" src="{r["name"]}.wav"></audio><p><a href="{r["name"]}.mlpatch">Patch</a></p></article>' for r in rows)
(OUT/'index.html').write_text('''<!doctype html><html lang="en"><meta charset="utf-8"><meta name="viewport" content="width=device-width,initial-scale=1"><title>0.15 Four independent LFOs</title><style>body{background:#101819;color:#e5eee8;font:18px system-ui;max-width:850px;margin:40px auto;padding:20px}article{background:#1a2729;padding:20px;margin:16px 0}audio{width:100%}a{color:#b9e7cb}</style><h1>Four independent LFOs</h1><p>Actual VST3 captures. E-minor chord except the labelled eight-note stress clip. Both drives zero, output 0.25; no loudness normalization. Isolate LFO 3/4, then combine four distinct source rates. Technical fixtures; new-source musical approval pending.</p>'''+cards+'</html>',encoding='utf-8')
print(f'PASS: {len(rows)} actual VST3 fixtures, three routes per new LFO and four-source combination; max peak={max(r["peak"] for r in rows):.6f}')
print('Slow Wah, Fast Heart: '+legacy_status)
