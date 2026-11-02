"""Create four seeded patch examples and render them through the actual VST3."""
from pathlib import Path
import json, math, struct, subprocess, wave
ROOT=Path(__file__).resolve().parents[1]
OUT=ROOT/'outputs/dice-study-1';OUT.mkdir(parents=True,exist_ok=True)
SMOKE=ROOT/'outputs/plugin-build/MemoryLabSmoke_artefacts/Release/MemoryLabSmoke.exe'
EXE=ROOT/'outputs/plugin-build/MemoryLabValidate_artefacts/Release/MemoryLabValidate.exe'
BUNDLE=ROOT/'outputs/plugin-build/MemoryLab_artefacts/Release/VST3/Memory Lab Study.vst3'
subprocess.run([str(SMOKE),'--dice-patches',str(OUT)],check=True)
rows=[]
for seed in range(4):
 stem=OUT/f'seed-{seed}'
 run=subprocess.run([str(EXE),'--capture-patch',str(BUNDLE),str(stem),str(stem.with_suffix('.mlpatch')),'default'],capture_output=True,text=True)
 stem.with_suffix('.log').write_text(run.stdout+run.stderr,encoding='utf-8')
 if run.returncode:raise RuntimeError(run.stdout+run.stderr)
 raw=stem.with_suffix('.f32').read_bytes();samples=struct.unpack('<'+'f'*(len(raw)//4),raw)
 assert all(math.isfinite(v) and abs(v)<1 for v in samples),'Example exceeds fixture headroom'
 with wave.open(str(stem.with_suffix('.wav')),'wb') as f:
  f.setnchannels(1);f.setsampwidth(2);f.setframerate(48000);f.writeframes(struct.pack('<'+'h'*len(samples),*(round(v*32767) for v in samples)))
 rows.append({'seed':seed,'peak':max(map(abs,samples)),'rms':math.sqrt(sum(v*v for v in samples)/len(samples))})
(OUT/'measurements.json').write_text(json.dumps(rows,indent=2),encoding='utf-8')
cards=''.join(f'<article><h2>Seed {r["seed"]}</h2><audio controls src="seed-{r["seed"]}.wav"></audio><p><a href="seed-{r["seed"]}.mlpatch">Patch file</a></p></article>' for r in rows)
(OUT/'index.html').write_text('''<!doctype html><html lang="en"><meta charset="utf-8"><title>Memory Lab 0.11 dice</title><style>body{background:#101819;color:#e5eee8;font:18px system-ui;max-width:850px;margin:40px auto;padding:20px}article{background:#1a2729;padding:20px;margin:16px 0}audio{width:100%}a{color:#b9e7cb}</style><h1>0.11 Patch dice</h1><p>Four seeded rolls, all sections unlocked. Same E minor chord, same output gain. Both drives off in these fixtures; a live roll preserves your current drive settings. No loudness normalization. New notes are played after each roll.</p>'''+cards+'</html>',encoding='utf-8')
print('PASS: four actual VST3 random-patch examples rendered')
print(OUT/'index.html')
