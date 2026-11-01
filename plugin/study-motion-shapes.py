"""Render the actual VST3's four LFO shapes using one controlled chord fixture."""
from pathlib import Path
import hashlib, html, json, math, struct, subprocess, wave
import xml.etree.ElementTree as ET

ROOT=Path(__file__).resolve().parents[1]
OUT=ROOT/'outputs/motion-shapes-study-1'
OUT.mkdir(parents=True,exist_ok=True)
EXE=ROOT/'outputs/plugin-build/MemoryLabValidate_artefacts/Release/MemoryLabValidate.exe'
BUNDLE=ROOT/'outputs/plugin-build/MemoryLab_artefacts/Release/VST3/Memory Lab Study.vst3'
SOURCE=ROOT/"outputs/Holger's Test Output - mlpatches/Memory Lab Study - White Noise Min - Filter Classic.mlpatch"
rows=[]
for shape,name in enumerate(('sine','triangle','saw','pulse')):
    root=ET.parse(SOURCE).getroot();root.set('schemaVersion','6')
    edits={'drive':0,'depth':0,'voiceDrive':0,'noiseLevel':0,'filterFamily':0,
           'profile':1,'cutoff':1450,'resonance':.71,'lfoRate':2,
           'vibrato':0,'pwm':0,'filterLfo':1,'filterEnv':0,'pressure':0}
    for entry in root:
        if entry.get('id') in edits:entry.set('value',str(edits[entry.get('id')]))
    ET.SubElement(root,'PARAM',id='lfoShape',value=str(shape))
    patch=OUT/f'{name}.mlpatch';ET.ElementTree(root).write(patch,encoding='unicode')
    stem=OUT/name
    run=subprocess.run([str(EXE),'--capture-patch',str(BUNDLE),str(stem),str(patch),'default'],capture_output=True,text=True)
    stem.with_suffix('.log').write_text(run.stdout+run.stderr,encoding='utf-8')
    if run.returncode:raise RuntimeError(run.stdout+run.stderr)
    raw=stem.with_suffix('.f32').read_bytes();samples=struct.unpack('<'+'f'*(len(raw)//4),raw)
    assert all(math.isfinite(v) and abs(v)<1 for v in samples)
    with wave.open(str(stem.with_suffix('.wav')),'wb') as output:
        output.setnchannels(1);output.setsampwidth(2);output.setframerate(48000)
        output.writeframes(struct.pack('<'+'h'*len(samples),*(round(v*32767) for v in samples)))
    rows.append({'shape':name,'sha256':hashlib.sha256(raw).hexdigest(),
                 'peak':max(map(abs,samples)),'rms':math.sqrt(sum(v*v for v in samples)/len(samples))})
(OUT/'measurements.json').write_text(json.dumps(rows,indent=2),encoding='utf-8')
# Compare against the verified previous module, using its backup after installation.
old_bundle=ROOT/'outputs/installed-backup-0.8.0-alpha.2'
if not old_bundle.is_dir():old_bundle=Path('C:/Program Files/Common Files/VST3/Memory Lab Study.vst3')
old_root=ET.parse(OUT/'sine.mlpatch').getroot();old_root.set('schemaVersion','5')
old_root.remove(next(e for e in old_root if e.get('id')=='lfoShape'))
old_patch=OUT/'sine-legacy.mlpatch';ET.ElementTree(old_root).write(old_patch,encoding='unicode')
baseline=OUT/'sine-08-baseline'
run=subprocess.run([str(EXE),'--capture-patch',str(old_bundle),str(baseline),str(old_patch),'default'],capture_output=True,text=True,check=True)
baseline.with_suffix('.log').write_text(run.stdout+run.stderr,encoding='utf-8')
assert 'version=0.8.0' in run.stdout, 'Baseline requires the previous 0.8 module'
assert baseline.with_suffix('.f32').read_bytes()==(OUT/'sine.f32').read_bytes(), 'Sine modulation changed from 0.8'
print('PASS: actual VST3 sine filter modulation is byte-identical to installed 0.8 a2')
cards=''.join(f'<article><h2>{html.escape(r["shape"].title())}</h2><audio controls src="{r["shape"]}.wav"></audio></article>' for r in rows)
(OUT/'index.html').write_text('''<!doctype html><html lang="en"><meta charset="utf-8"><title>Memory Lab 0.9 — LFO shapes</title>
<style>body{background:#101819;color:#e5eee8;font:18px system-ui;max-width:850px;margin:40px auto;padding:20px}article{background:#1a2729;padding:20px;margin:16px 0;border-radius:12px}audio{width:100%}</style>
<h1>0.9 · LFO shapes</h1><p>Same E minor chord, 2 Hz LFO, filter modulation ±1 octave. Both drives off, noise off. Only the shape differs. No loudness matching or individual normalization; all clips use the same output gain.</p>
<p>Triangle starts low; sine starts at its midpoint. Saw and pulse have deliberate jumps. These examples demonstrate movement, not hardware equivalence.</p>'''+cards+'</html>',encoding='utf-8')
print(OUT/'index.html')
