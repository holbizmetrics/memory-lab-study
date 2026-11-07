"""Real VST3 route captures and byte-exact 0.16 preservation checks."""
from pathlib import Path
import hashlib,json,math,struct,subprocess,xml.etree.ElementTree as ET,wave
ROOT=Path(__file__).resolve().parents[1]
OUT=ROOT/'outputs/routing-study-0.17';OUT.mkdir(parents=True,exist_ok=True)
SMOKE=ROOT/'outputs/plugin-build/MemoryLabSmoke_artefacts/Release/MemoryLabSmoke.exe'
EXE=ROOT/'outputs/plugin-build/MemoryLabValidate_artefacts/Release/MemoryLabValidate.exe'
BUNDLE=ROOT/'outputs/plugin-build/MemoryLab_artefacts/Release/VST3/Memory Lab Study.vst3'
subprocess.run([str(SMOKE),'--factory-patches',str(OUT/'bank')],check=True)
base=ET.parse(OUT/'bank/1.mlpatch').getroot()
def capture(stem,patch=None):
 args=[str(EXE),'--capture-patch' if patch else '--capture',str(BUNDLE),str(stem)]
 if patch:args.append(str(patch))
 result=subprocess.run(args+['stress' if stem.name=='combined-eight-voices' else 'default'],capture_output=True,text=True,check=True)
 stem.with_suffix('.log').write_text(result.stdout+result.stderr,encoding='utf-8')
 raw=stem.with_suffix('.f32').read_bytes();samples=struct.unpack('<'+'f'*(len(raw)//4),raw)
 assert all(math.isfinite(x) for x in samples)
 return raw,samples
routes={'lfoAmp':.8,'lfo2Amp':.8,'lfo3Amp':.8,'lfo4Amp':.8,'ampPitch':200,'fltPitch':200,'env3Pwm':.2,'env4Pwm':.2}
conditions=[('clean',{})]
for key,amount in routes.items():
 for sign in (1,-1):conditions.append((key+('-positive' if sign>0 else '-negative'),{key:sign*amount}))
conditions += [('combined',routes),('combined-eight-voices',routes)]
rows=[]
for name,settings in conditions:
 root=ET.fromstring(ET.tostring(base));values={'cutoff':1500,'resonance':.8,**settings}
 for entry in root:
  if entry.get('id') in values:entry.set('value',str(values[entry.get('id')]))
 stem=OUT/name;ET.ElementTree(root).write(stem.with_suffix('.mlpatch'),encoding='unicode')
 raw,samples=capture(stem,stem.with_suffix('.mlpatch'));peak=max(map(abs,samples));rms=math.sqrt(sum(v*v for v in samples)/len(samples))
 assert 0<peak<1 and rms>1e-6,name
 with wave.open(str(stem.with_suffix('.wav')),'wb') as f:
  f.setnchannels(1);f.setsampwidth(2);f.setframerate(48000);f.writeframes(struct.pack('<'+'h'*len(samples),*(round(v*32767) for v in samples)))
 rows.append(dict(name=name,peak=peak,rms=rms))
for name,_ in conditions[1:]:assert (OUT/f'{name}.f32').read_bytes()!=(OUT/'clean.f32').read_bytes(),name
for key in routes:assert (OUT/f'{key}-positive.f32').read_bytes()!=(OUT/f'{key}-negative.f32').read_bytes(),key
preservation={}
fixtures=[('default',None,ROOT/'outputs/four-envelope-study-0.16/default-preserved.f32'),('clock',ROOT/'outputs/motion-clock-study-1/eighth-reset.mlpatch',ROOT/'outputs/four-envelope-study-0.16/legacy-clock-preserved.f32'),('active-envelopes',ROOT/'outputs/four-envelope-study-0.16/both.mlpatch',ROOT/'outputs/four-envelope-study-0.16/both.f32')]
legacy=ROOT/'outputs/four-lfo-0.15-baseline/slow-wah.mlpatch'
if legacy.exists():fixtures.append(('slow-wah',legacy,legacy.with_suffix('.f32')))
for name,patch,golden in fixtures:
 raw,_=capture(OUT/f'preserved-{name}',patch)
 assert raw==golden.read_bytes(),name+' changed'
 preservation[name]=hashlib.sha256(raw).hexdigest()
(OUT/'measurements.json').write_text(json.dumps(dict(fixtures=rows,preservation=preservation),indent=2),encoding='utf-8')
print(f'PASS {len(rows)} real VST3 route fixtures; peak {max(r["peak"] for r in rows):.6f}; preserved '+', '.join(preservation))
