"""Real VST3 oscillator waveform captures and byte-exact 0.17 preservation."""
from pathlib import Path
import hashlib,json,math,struct,subprocess,xml.etree.ElementTree as ET,wave
ROOT=Path(__file__).resolve().parents[1]
OUT=ROOT/'outputs/vco-study-0.18';OUT.mkdir(parents=True,exist_ok=True)
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
conditions=[
 ('clean',{}),('inactive-width',{'osc1Width':.2}),
 ('osc1-pulse',{'sawLevel':0,'pulseLevel':0,'osc1PulseLevel':.4}),
 ('osc1-pulse-narrow',{'sawLevel':0,'pulseLevel':0,'osc1PulseLevel':.4,'osc1Width':.2}),
 ('osc2-saw',{'sawLevel':0,'pulseLevel':0,'osc2SawLevel':.4}),
 ('osc2-saw-octave',{'sawLevel':0,'pulseLevel':0,'osc2SawLevel':.4,'osc2Interval':12}),
 ('all-four',{'sawLevel':.2,'pulseLevel':.2,'osc1PulseLevel':.2,'osc2SawLevel':.2}),
 ('combined-eight-voices',{'sawLevel':.3,'pulseLevel':.3,'osc1PulseLevel':.3,'osc2SawLevel':.3,'volume':.1})]
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
assert (OUT/'clean.f32').read_bytes()==(OUT/'inactive-width.f32').read_bytes(),'Inactive width changed audio'
for name in ('osc1-pulse','osc1-pulse-narrow','osc2-saw','osc2-saw-octave','all-four'):
 assert (OUT/f'{name}.f32').read_bytes()!=(OUT/'clean.f32').read_bytes(),name
assert (OUT/'osc1-pulse.f32').read_bytes()!=(OUT/'osc1-pulse-narrow.f32').read_bytes()
assert (OUT/'osc2-saw.f32').read_bytes()!=(OUT/'osc2-saw-octave.f32').read_bytes()
preservation={}
fixtures=[('default',None,ROOT/'outputs/routing-study-0.17/preserved-default.f32'),('clock',ROOT/'outputs/motion-clock-study-1/eighth-reset.mlpatch',ROOT/'outputs/routing-study-0.17/preserved-clock.f32'),('active-envelopes',ROOT/'outputs/four-envelope-study-0.16/both.mlpatch',ROOT/'outputs/routing-study-0.17/preserved-active-envelopes.f32'),('active-routing',ROOT/'outputs/routing-study-0.17/combined.mlpatch',ROOT/'outputs/routing-study-0.17/combined.f32')]
legacy=ROOT/'outputs/four-lfo-0.15-baseline/slow-wah.mlpatch'
if legacy.exists():fixtures.append(('slow-wah',legacy,legacy.with_suffix('.f32')))
for name,patch,golden in fixtures:
 raw,_=capture(OUT/f'preserved-{name}',patch)
 assert raw==golden.read_bytes(),name+' changed'
 preservation[name]=hashlib.sha256(raw).hexdigest()
(OUT/'measurements.json').write_text(json.dumps(dict(fixtures=rows,preservation=preservation),indent=2),encoding='utf-8')
print(f'PASS {len(rows)} real VST3 waveform fixtures; peak {max(r["peak"] for r in rows):.6f}; preserved '+', '.join(preservation))
