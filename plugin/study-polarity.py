"""Actual VST3 0.21 polarity examples and immutable 0.20 preservation checks."""
from pathlib import Path
import json,struct,subprocess,hashlib,math,wave,xml.etree.ElementTree as ET
R=Path(__file__).resolve().parents[1];O=R/'outputs/polarity-study-0.21';O.mkdir(exist_ok=True)
S=R/'outputs/plugin-build/MemoryLabSmoke_artefacts/Release/MemoryLabSmoke.exe'
V=R/'outputs/plugin-build/MemoryLabValidate_artefacts/Release/MemoryLabValidate.exe'
B=R/'outputs/plugin-build/MemoryLab_artefacts/Release/VST3/Memory Lab Study.vst3'
old=R/'outputs/integration-study-0.20'
def capture(stem,patch=None,mode='default'):
 if patch:subprocess.run([str(S),'--check-patch-file',str(patch)],check=True,capture_output=True)
 args=[str(V),'--capture-patch',str(B),str(stem),str(patch),mode] if patch else [str(V),'--capture',str(B),str(stem),mode]
 r=subprocess.run(args,check=True,capture_output=True,text=True);stem.with_suffix('.log').write_text(r.stdout+r.stderr,encoding='utf-8')
 raw=stem.with_suffix('.f32').read_bytes();a=struct.unpack('<'+'f'*(len(raw)//4),raw);assert all(math.isfinite(x) for x in a)
 return raw,a
meta=json.loads((R/'outputs/parts-study-0.19/measurements.json').read_text(encoding='utf-8'));preserved=[]
parts=R/'outputs/parts-study-0.19'
for row in meta['fixtures']:
 name=row['name']+'-drive'+str(row['drive']);raw,_=capture(O/name,parts/(name+'.mlpatch'),'stress');assert raw==(old/(name+'.f32')).read_bytes(),name;preserved.append(name)
for row in meta['preservation']:
 name=row['name']+'-candidate';raw,_=capture(O/name,parts/(name+'.mlpatch'));assert raw==(old/(name+'.f32')).read_bytes(),name;preserved.append(name)
for name in ['default','clock','active-envelopes','active-routing','slow-wah']:
 patch={'clock':R/'outputs/motion-clock-study-1/eighth-reset.mlpatch','active-envelopes':R/'outputs/four-envelope-study-0.16/both.mlpatch','active-routing':R/'outputs/routing-study-0.17/combined.mlpatch','slow-wah':R/'outputs/four-lfo-0.15-baseline/slow-wah.mlpatch'}.get(name)
 stem='preserved-'+name;raw,_=capture(O/stem,patch);assert raw==(old/(stem+'.f32')).read_bytes(),stem;preserved.append(stem)
for i in range(3):
 name='combined-'+str(i);raw,_=capture(O/name,old/f'patches/context-{i}.mlpatch','stress');assert raw==(old/(name+'.f32')).read_bytes(),name;preserved.append(name)
subprocess.run([str(S),'--factory-patches',str(O/'bank')],check=True,capture_output=True)
base=ET.parse(O/'bank/0.mlpatch').getroot() if (O/'bank/0.mlpatch').exists() else ET.parse(O/'bank/1.mlpatch').getroot()
rows=[]
for source in range(1,5):
 prefix='lfo' if source==1 else f'lfo{source}'
 pair=[]
 for uni in [0,1]:
  root=ET.fromstring(ET.tostring(base));values={'drive':0,'voiceDrive':0,'volume':.12,'cutoff':800,'resonance':.9,'vibrato':0,'pwm':0,'filterLfo':0,'filterEnv':0,'ampAttack':5,'ampDecay':0,'ampSustain':1,'ampRelease':100,prefix+'Shape':0,prefix+'Rate':1,prefix+'Sync':0,prefix+'Unipolar':uni,('vibrato' if source==1 else prefix+'Pitch'):50}
  for e in root:
   if e.get('id') in values:e.set('value',str(values[e.get('id')]))
  stem=O/f'lfo{source}-{("unipolar" if uni else "bipolar")}'
  patch=stem.with_suffix('.mlpatch');ET.ElementTree(root).write(patch,encoding='unicode')
  raw,a=capture(stem,patch,'single');peak=max(map(abs,a));assert 0<peak<1;pair.append(raw)
  with wave.open(str(stem.with_suffix('.wav')),'wb') as w:w.setnchannels(1);w.setsampwidth(2);w.setframerate(48000);w.writeframes(struct.pack('<'+'h'*len(a),*(round(x*32767) for x in a)))
  rows.append(dict(source=source,unipolar=uni,peak=peak,sha256=hashlib.sha256(raw).hexdigest()))
 assert pair[0]!=pair[1],source
(O/'measurements.json').write_text(json.dumps(dict(preserved=preserved,fixtures=rows),indent=2),encoding='utf-8')
print('PASS',len(preserved),'0.20 captures byte-identical;',len(rows),'actual module polarity fixtures; peak',max(x['peak'] for x in rows))
