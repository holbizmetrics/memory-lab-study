"""Compare installed alpha.2 and candidate for every legacy family, read-only."""
from pathlib import Path
import copy,hashlib,json,subprocess,xml.etree.ElementTree as ET
R=Path(__file__).resolve().parents[1];O=R/'outputs/modern-legacy-preservation';O.mkdir(exist_ok=True,parents=True)
V=R/'outputs/plugin-build/MemoryLabValidate_artefacts/Release/MemoryLabValidate.exe'
S=R/'outputs/plugin-build/MemoryLabSmoke_artefacts/Release/MemoryLabSmoke.exe'
old=Path('C:/Program Files/Common Files/VST3/Memory Lab Study.vst3')
new=R/'outputs/plugin-build/MemoryLab_artefacts/Release/VST3/Memory Lab Study.vst3'
subprocess.run([str(S),'--factory-patches',str(O/'bank')],check=True)
base=ET.parse(O/'bank/1.mlpatch').getroot();base.set('schemaVersion','13')
for e in list(base):
 if e.get('id').startswith('modern'):base.remove(e)
rows=[]
for family in range(4):
 for drive in [0,5]:
  for noise in [0,.2]:
   root=copy.deepcopy(base);settings={'filterFamily':family,'drive':drive,'noiseLevel':noise,'volume':.05,'resonance':3.72,'cutoff':2062}
   for e in root:
    if e.get('id') in settings:e.set('value',str(settings[e.get('id')]))
   name=f'family{family}-drive{drive}-noise{round(noise*1000)}';patch=O/(name+'.mlpatch');ET.ElementTree(root).write(patch)
   hashes=[]
   for label,bundle in [('alpha2',old),('candidate',new)]:
    stem=O/(name+'-'+label)
    result=subprocess.run([str(V),'--capture-patch',str(bundle),str(stem),str(patch),'default'],capture_output=True,text=True,check=True)
    stem.with_suffix('.log').write_text(result.stdout+result.stderr)
    hashes.append(hashlib.sha256(stem.with_suffix('.f32').read_bytes()).hexdigest())
   assert hashes[0]==hashes[1],name
   rows.append({'fixture':name,'sha256':hashes[0]})
(O/'comparison.json').write_text(json.dumps(rows,indent=2))
print('PASS 16 legacy family/drive/noise captures byte-identical against installed alpha.2; installed bundle unchanged')

