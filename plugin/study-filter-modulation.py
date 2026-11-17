"""0.23 actual module routes and exact 0.22 sound-preservation captures."""
from pathlib import Path
import json,struct,subprocess,math,hashlib,wave,xml.etree.ElementTree as ET
R=Path(__file__).resolve().parents[1];O=R/'outputs/filter-modulation-study-0.23';O.mkdir(exist_ok=True)
S=R/'outputs/plugin-build/MemoryLabSmoke_artefacts/Release/MemoryLabSmoke.exe';V=R/'outputs/plugin-build/MemoryLabValidate_artefacts/Release/MemoryLabValidate.exe';B=R/'outputs/plugin-build/MemoryLab_artefacts/Release/VST3/Memory Lab Study.vst3'
previous=R/'outputs/envelope-character-study-0.22';parts=R/'outputs/parts-study-0.19';meta=json.loads((parts/'measurements.json').read_text(encoding='utf-8'))
def capture(stem,patch=None,mode='default'):
 if patch:subprocess.run([str(S),'--check-patch-file',str(patch)],check=True,capture_output=True)
 args=[str(V),'--capture-patch',str(B),str(stem),str(patch),mode] if patch else [str(V),'--capture',str(B),str(stem),mode]
 r=subprocess.run(args,check=True,capture_output=True,text=True);stem.with_suffix('.log').write_text(r.stdout+r.stderr,encoding='utf-8')
 raw=stem.with_suffix('.f32').read_bytes();a=struct.unpack('<'+'f'*(len(raw)//4),raw);assert all(math.isfinite(x) for x in a),stem
 return raw,a
jobs=[]
for row in meta['fixtures']:
 name=row['name']+'-drive'+str(row['drive']);jobs.append((name,parts/(name+'.mlpatch'),'stress'))
for row in meta['preservation']:
 name=row['name']+'-candidate';jobs.append((name,parts/(name+'.mlpatch'),'default'))
for name in ['default','clock','active-envelopes','active-routing','slow-wah']:
 patch={'clock':R/'outputs/motion-clock-study-1/eighth-reset.mlpatch','active-envelopes':R/'outputs/four-envelope-study-0.16/both.mlpatch','active-routing':R/'outputs/routing-study-0.17/combined.mlpatch','slow-wah':R/'outputs/four-lfo-0.15-baseline/slow-wah.mlpatch'}.get(name)
 jobs.append(('preserved-'+name,patch,'default'))
for i in range(3):jobs.append(('combined-'+str(i),R/f'outputs/integration-study-0.20/patches/context-{i}.mlpatch','stress'))
for source in range(1,5):
 for polarity in ['bipolar','unipolar']:
  name=f'lfo{source}-{polarity}';jobs.append((name,R/f'outputs/polarity-study-0.21/{name}.mlpatch','single'))
for row in json.loads((previous/'measurements.json').read_text())['fixtures']:
 name=row['name'];jobs.append((name,previous/(name+'.mlpatch'),'stress' if name=='combined-curved' else 'single'))
preserved=[]
for name,patch,mode in jobs:
 raw,_=capture(O/name,patch,mode);assert raw==(previous/(name+'.f32')).read_bytes(),name;preserved.append(name)
subprocess.run([str(S),'--factory-patches',str(O/'bank')],check=True,capture_output=True)
base=ET.parse(O/'bank/1.mlpatch').getroot();rows=[]
def fixture(name,values,mode='single',root=None):
 root=ET.fromstring(ET.tostring(base if root is None else root))
 for e in root:
  if e.get('id') in values:e.set('value',str(values[e.get('id')]))
 stem=O/name;patch=stem.with_suffix('.mlpatch');ET.ElementTree(root).write(patch,encoding='unicode');raw,a=capture(stem,patch,mode)
 peak=max(map(abs,a));assert 0<peak<1,(name,peak)
 with wave.open(str(stem.with_suffix('.wav')),'wb') as w:w.setnchannels(1);w.setsampwidth(2);w.setframerate(48000);w.writeframes(struct.pack('<'+'h'*len(a),*(round(x*32767) for x in a)))
 rows.append(dict(name=name,peak=peak,sha256=hashlib.sha256(raw).hexdigest()));return raw
common=dict(drive=0,voiceDrive=0,volume=.04,cutoff=1400,resonance=1.2,profile=.4,modernProfile=.3,ampAttack=10,ampDecay=0,ampSustain=.75,ampRelease=1000,filterEnv=0,env3Pitch=0,env3Cutoff=0,env3Pwm=0,env3Attack=1200,env3Decay=700,env3Sustain=.2,env3Release=400,env3Curve=1,vibrato=0,pwm=0,filterLfo=0,lfoRate=1.2,lfoShape=0,lfoSync=0)
routes=['filterModEnvProfile','filterModLfoProfile','filterModEnvRes','filterModLfoRes']
for family in ['classic','modern']:
 values={**common,'modernEnabled':int(family=='modern')};baseline=fixture(family+'-base',values)
 for route in routes:
  amount=.6 if route.endswith('Profile') else 3
  assert fixture(family+'-'+route,{**values,route:amount})!=baseline,route
fixture('negative-profile',{**common,'profile':.7,'filterModEnvProfile':-.6})
fixture('negative-resonance',{**common,'resonance':4,'filterModLfoRes':-3})
subprocess.run([str(S),'--integration-patches',str(O/'combined')],check=True,capture_output=True)
combined=ET.parse(O/'combined/context-0.mlpatch').getroot();values={}
for part in range(4):
 prefix='' if part==0 else f'part{part+1}__'
 for key,value in dict(volume=.005,drive=0,voiceDrive=0,filterModEnvProfile=.3,filterModLfoProfile=.2,filterModEnvRes=1.5,filterModLfoRes=.5,env3Curve=1,env3TimeScale=.5).items():values[prefix+key]=value
fixture('combined-filter-routes',values,'stress',combined)
(O/'measurements.json').write_text(json.dumps(dict(preserved=preserved,fixtures=rows),indent=2),encoding='utf-8')
print('PASS',len(preserved),'0.22 captures byte-identical;',len(rows),'filter modulation VST3 fixtures; peak',max(x['peak'] for x in rows))
