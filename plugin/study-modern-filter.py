"""Actual VST3 modern profile/headroom fixtures; no installed-plugin mutation."""
from pathlib import Path
import copy,hashlib,json,math,struct,subprocess,xml.etree.ElementTree as ET,wave
R=Path(__file__).resolve().parents[1];O=R/'outputs/modern-filter-study-0.18';O.mkdir(exist_ok=True,parents=True)
S=R/'outputs/plugin-build/MemoryLabSmoke_artefacts/Release/MemoryLabSmoke.exe'
V=R/'outputs/plugin-build/MemoryLabValidate_artefacts/Release/MemoryLabValidate.exe'
B=R/'outputs/plugin-build/MemoryLab_artefacts/Release/VST3/Memory Lab Study.vst3'
subprocess.run([str(S),'--factory-patches',str(O/'bank')],check=True)
base=ET.parse(O/'bank/1.mlpatch').getroot()
rows=[]
for profile,name in [(0,'bp4'),(.25,'xf4'),(.5,'lp2'),(.75,'lp3'),(1,'lp4')]:
 for drive in [0,5]:
  root=copy.deepcopy(base);settings={'modernEnabled':1,'modernProfile':profile,'modernBandwidth':1,'cutoff':2062,'resonance':3.72,'drive':drive,'volume':.1}
  for e in root:
   if e.get('id') in settings:e.set('value',str(settings[e.get('id')]))
  stem=O/(name+'-drive'+str(drive));patch=stem.with_suffix('.mlpatch');ET.ElementTree(root).write(patch)
  subprocess.run([str(S),'--check-patch-file',str(patch)],capture_output=True,check=True)
  r=subprocess.run([str(V),'--capture-patch',str(B),str(stem),str(patch),'default'],capture_output=True,text=True,check=True)
  stem.with_suffix('.log').write_text(r.stdout+r.stderr)
  raw=stem.with_suffix('.f32').read_bytes();samples=struct.unpack('<'+'f'*(len(raw)//4),raw)
  assert all(math.isfinite(x) for x in samples)
  peak=max(map(abs,samples));assert peak<1, (name,drive,peak)
  rms=math.sqrt(sum(x*x for x in samples)/len(samples))
  with wave.open(str(stem.with_suffix('.wav')),'wb') as w:
   w.setnchannels(1);w.setsampwidth(2);w.setframerate(48000);w.writeframes(struct.pack('<'+'h'*len(samples),*(round(x*32767) for x in samples)))
  rows.append({'profile':name,'drive':drive,'peak':peak,'rms':rms})
(O/'measurements.json').write_text(json.dumps(rows,indent=2))
print('PASS 10 actual VST3 profiles/drive fixtures, each file-loader validated; peak',max(x['peak'] for x in rows))

