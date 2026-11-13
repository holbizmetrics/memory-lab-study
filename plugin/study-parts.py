"""Real VST3 parts fixtures and bit-exact comparison with installed alpha.3."""
from pathlib import Path
import copy,hashlib,json,math,struct,subprocess,xml.etree.ElementTree as ET,wave
R=Path(__file__).resolve().parents[1];O=R/'outputs/parts-study-0.19';O.mkdir(exist_ok=True,parents=True)
S=R/'outputs/plugin-build/MemoryLabSmoke_artefacts/Release/MemoryLabSmoke.exe'
V=R/'outputs/plugin-build/MemoryLabValidate_artefacts/Release/MemoryLabValidate.exe'
NEW=R/'outputs/plugin-build/MemoryLab_artefacts/Release/VST3/Memory Lab Study.vst3'
OLD=Path('C:/Program Files/Common Files/VST3/Memory Lab Study.vst3')
assert hashlib.sha256((OLD/'Contents/x86_64-win/Memory Lab Study.vst3').read_bytes()).hexdigest().upper()=='9CC772E035486906AB2086B09E1F741BC87700EE8E5B63A5CC7CA30E6EFC1863','installed comparison is not verified alpha.3'
subprocess.run([str(S),'--factory-patches',str(O/'bank')],check=True)
def read(n):return ET.parse(O/f'bank/{n}.mlpatch').getroot()
def update(root,settings):
 for e in root:
  if e.get('id') in settings:e.set('value',str(settings[e.get('id')]))
def capture(bundle,stem,root,mode='default'):
 patch=stem.with_suffix('.mlpatch');ET.ElementTree(root).write(patch)
 if bundle==NEW:subprocess.run([str(S),'--check-patch-file',str(patch)],capture_output=True,check=True)
 r=subprocess.run([str(V),'--capture-patch',str(bundle),str(stem),str(patch),mode],capture_output=True,text=True,check=True)
 stem.with_suffix('.log').write_text(r.stdout+r.stderr,encoding='utf-8')
 return stem.with_suffix('.f32').read_bytes()
preserved=[]
base=read(1);base.set('schemaVersion','14')
for e in list(base):
 if e.get('id').startswith('part') or e.get('id')=='splitNote':base.remove(e)
for family in range(4):
 for drive in [0,5]:
  for noise in [0,.2]:
   root=copy.deepcopy(base);update(root,{'filterFamily':family,'drive':drive,'noiseLevel':noise,'volume':.05,'resonance':3.72,'cutoff':2062})
   name=f'legacy-f{family}-d{drive}-n{round(noise*1000)}'
   old=capture(OLD,O/(name+'-alpha3'),root);new=capture(NEW,O/(name+'-candidate'),root)
   assert old==new,name
   preserved.append({'name':name,'sha256':hashlib.sha256(new).hexdigest()})
for profile in [0,.25,.5,.75,1]:
 root=copy.deepcopy(base);update(root,{'modernEnabled':1,'modernProfile':profile,'modernBandwidth':1,'drive':0,'volume':.05})
 name=f'modern-p{round(profile*100)}'
 old=capture(OLD,O/(name+'-alpha3'),root);new=capture(NEW,O/(name+'-candidate'),root);assert old==new,name
 preserved.append({'name':name,'sha256':hashlib.sha256(new).hexdigest()})
base=read(2);native=[e.get('id') for e in base if not e.get('id').startswith(('arp','part')) and e.get('id')!='splitNote']
assert len(native)==99
for part,preset in [(2,4),(3,5),(4,3)]:
 values={e.get('id'):e.get('value') for e in read(preset)}
 update(base,{f'part{part}__{id}':values[id] for id in native})
rows=[]
for mode,name in [(0,'single'),(1,'layer2'),(2,'split2'),(3,'layer4')]:
 for drive in [0,5]:
  root=copy.deepcopy(base);settings={'partMode':mode,'splitNote':55}
  for part in range(4):
   prefix='' if part==0 else f'part{part+1}__'
   settings[prefix+'volume']=.04;settings[prefix+'drive']=drive
  update(root,settings);stem=O/(name+'-drive'+str(drive));raw=capture(NEW,stem,root,'stress')
  samples=struct.unpack('<'+'f'*(len(raw)//4),raw);assert all(math.isfinite(x) for x in samples)
  peak=max(map(abs,samples));assert 0<peak<1,(name,drive,peak)
  rms=math.sqrt(sum(x*x for x in samples)/len(samples))
  with wave.open(str(stem.with_suffix('.wav')),'wb') as w:
   w.setnchannels(1);w.setsampwidth(2);w.setframerate(48000);w.writeframes(struct.pack('<'+'h'*len(samples),*(round(x*32767) for x in samples)))
  rows.append({'name':name,'drive':drive,'peak':peak,'rms':rms})
(O/'measurements.json').write_text(json.dumps({'fixtures':rows,'preservation':preserved},indent=2),encoding='utf-8')
print('PASS',len(rows),'actual VST3 part fixtures; peak',max(x['peak'] for x in rows),';',len(preserved),'legacy/modern captures byte-identical against alpha.3')

