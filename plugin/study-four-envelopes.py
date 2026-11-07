"""Render the real 0.16 VST3; preserve public defaults and local user fixtures."""
from pathlib import Path
import hashlib,json,math,struct,subprocess,xml.etree.ElementTree as ET,wave
ROOT=Path(__file__).resolve().parents[1]
OUT=ROOT/'outputs/four-envelope-study-0.16';OUT.mkdir(parents=True,exist_ok=True)
SMOKE=ROOT/'outputs/plugin-build/MemoryLabSmoke_artefacts/Release/MemoryLabSmoke.exe'
EXE=ROOT/'outputs/plugin-build/MemoryLabValidate_artefacts/Release/MemoryLabValidate.exe'
BUNDLE=ROOT/'outputs/plugin-build/MemoryLab_artefacts/Release/VST3/Memory Lab Study.vst3'
subprocess.run([str(SMOKE),'--factory-patches',str(OUT/'bank')],check=True)
base=ET.parse(OUT/'bank/1.mlpatch').getroot()
def capture(stem,patch=None,mode='default'):
 args=[str(EXE),'--capture-patch' if patch else '--capture',str(BUNDLE),str(stem)]
 if patch:args.append(str(patch))
 result=subprocess.run(args+[mode],capture_output=True,text=True,check=True)
 stem.with_suffix('.log').write_text(result.stdout+result.stderr,encoding='utf-8')
 raw=stem.with_suffix('.f32').read_bytes();samples=struct.unpack('<'+'f'*(len(raw)//4),raw)
 assert all(math.isfinite(x) for x in samples)
 return raw,samples
conditions=[('clean',{},'default'),('inactive-stages',{'env3Delay':1200,'env3Attack':7500,'env3Sustain':.1,'env4Attack':9000,'env4Release':10000},'default')]
for n in (3,4):
 for route,value in [('Pitch',200),('Cutoff',2)]:
  conditions.append((f'env{n}-{route.lower()}',{f'env{n}{route}':value,f'env{n}Attack':800,f'env{n}Sustain':.25},'default'))
both={'env3Pitch':200,'env3Attack':1500,'env3Sustain':.2,'env4Cutoff':2,'env4Attack':600,'env4Sustain':.5}
conditions += [('both',both,'default'),('both-eight-voices',both,'stress'),('negative-depths',{'env3Pitch':-300,'env4Cutoff':-2},'default')]
rows=[]
for name,settings,mode in conditions:
 root=ET.fromstring(ET.tostring(base));values={'cutoff':1000,'resonance':.8,**settings}
 for entry in root:
  if entry.get('id') in values:entry.set('value',str(values[entry.get('id')]))
 stem=OUT/name;ET.ElementTree(root).write(stem.with_suffix('.mlpatch'),encoding='unicode')
 raw,samples=capture(stem,stem.with_suffix('.mlpatch'),mode)
 peak=max(map(abs,samples));rms=math.sqrt(sum(v*v for v in samples)/len(samples))
 assert 0<peak<1 and rms>1e-6,name
 with wave.open(str(stem.with_suffix('.wav')),'wb') as f:
  f.setnchannels(1);f.setsampwidth(2);f.setframerate(48000);f.writeframes(struct.pack('<'+'h'*len(samples),*(round(v*32767) for v in samples)))
 rows.append(dict(name=name,peak=peak,rms=rms))
assert (OUT/'clean.f32').read_bytes()==(OUT/'inactive-stages.f32').read_bytes()
for name,_,_ in conditions[2:]:assert (OUT/f'{name}.f32').read_bytes()!=(OUT/'clean.f32').read_bytes(),name
preservation={}
for name in ('default','legacy-clock'):
 old_patch=ROOT/'outputs/motion-clock-study-1/eighth-reset.mlpatch' if name=='legacy-clock' else None
 raw,_=capture(OUT/f'{name}-preserved',patch=old_patch)
 golden=ROOT/f'outputs/four-lfo-0.15-validation/{name}.f32'
 assert raw==golden.read_bytes(),name+' changed'
 preservation[name]=hashlib.sha256(raw).hexdigest()
legacy=ROOT/'outputs/four-lfo-0.15-baseline/slow-wah.mlpatch'
if legacy.exists():
 raw,_=capture(OUT/'slow-wah-preserved',legacy)
 assert raw==legacy.with_suffix('.f32').read_bytes(),'saved user sound changed'
 preservation['slow-wah']=hashlib.sha256(raw).hexdigest()
(OUT/'measurements.json').write_text(json.dumps(dict(fixtures=rows,preservation=preservation),indent=2),encoding='utf-8')
print(f'PASS {len(rows)} real VST3 envelope fixtures; peak {max(r["peak"] for r in rows):.6f}; preserved '+', '.join(preservation))
