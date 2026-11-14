"""0.20 actual module combined fixtures and unchanged 0.19 sound captures."""
from pathlib import Path
import hashlib,json,math,struct,subprocess,wave
R=Path(__file__).resolve().parents[1];O=R/'outputs/integration-study-0.20';O.mkdir(exist_ok=True,parents=True)
S=R/'outputs/plugin-build/MemoryLabSmoke_artefacts/Release/MemoryLabSmoke.exe'
V=R/'outputs/plugin-build/MemoryLabValidate_artefacts/Release/MemoryLabValidate.exe'
B=R/'outputs/plugin-build/MemoryLab_artefacts/Release/VST3/Memory Lab Study.vst3'
previous=R/'outputs/parts-study-0.19'
meta=json.loads((previous/'measurements.json').read_text(encoding='utf-8'))
def capture(stem,patch=None,mode='default'):
 if patch:
  subprocess.run([str(S),'--check-patch-file',str(patch)],capture_output=True,check=True)
  args=[str(V),'--capture-patch',str(B),str(stem),str(patch),mode]
 else:args=[str(V),'--capture',str(B),str(stem),mode]
 r=subprocess.run(args,capture_output=True,text=True,check=True)
 stem.with_suffix('.log').write_text(r.stdout+r.stderr,encoding='utf-8')
 raw=stem.with_suffix('.f32').read_bytes();samples=struct.unpack('<'+'f'*(len(raw)//4),raw)
 assert all(math.isfinite(x) for x in samples)
 return raw,samples
preserved=[]
for row in meta['fixtures']:
 name=row['name']+'-drive'+str(row['drive']);raw,_=capture(O/name,previous/(name+'.mlpatch'),'stress')
 assert raw==(previous/(name+'.f32')).read_bytes(),name
 preserved.append(name)
for row in meta['preservation']:
 name=row['name']+'-candidate';raw,_=capture(O/name,previous/(name+'.mlpatch'))
 assert raw==(previous/(name+'.f32')).read_bytes(),name
 preserved.append(name)
legacy=R/'outputs/vco-study-0.18'
for name in ['default','clock','active-envelopes','active-routing','slow-wah']:
 patch={'clock':R/'outputs/motion-clock-study-1/eighth-reset.mlpatch','active-envelopes':R/'outputs/four-envelope-study-0.16/both.mlpatch','active-routing':R/'outputs/routing-study-0.17/combined.mlpatch','slow-wah':R/'outputs/four-lfo-0.15-baseline/slow-wah.mlpatch'}.get(name)
 raw,_=capture(O/('preserved-'+name),patch)
 assert raw==(legacy/('preserved-'+name+'.f32')).read_bytes(),name
 preserved.append(name)
subprocess.run([str(S),'--integration-patches',str(O/'patches')],check=True)
rows=[]
for i in range(3):
 stem=O/('combined-'+str(i));raw,samples=capture(stem,O/f'patches/context-{i}.mlpatch','stress')
 peak=max(map(abs,samples));assert 0<peak<1,(i,peak)
 with wave.open(str(stem.with_suffix('.wav')),'wb') as w:
  w.setnchannels(1);w.setsampwidth(2);w.setframerate(48000);w.writeframes(struct.pack('<'+'h'*len(samples),*(round(x*32767) for x in samples)))
 rows.append({'context':i,'peak':peak,'sha256':hashlib.sha256(raw).hexdigest()})
(O/'measurements.json').write_text(json.dumps({'preserved':preserved,'combined':rows},indent=2),encoding='utf-8')
print('PASS',len(preserved),'existing captures byte-identical;',len(rows),'combined VST3 contexts, max peak',max(x['peak'] for x in rows))

