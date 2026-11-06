"""Render original starting sounds through the built VST3; no installed-host access."""
from pathlib import Path
import hashlib, json, math, struct, subprocess, wave

ROOT = Path(__file__).resolve().parents[1]
OUT = ROOT / 'outputs/preset-study-0.13'
OUT.mkdir(parents=True, exist_ok=True)
SMOKE = ROOT / 'outputs/plugin-build/MemoryLabSmoke_artefacts/Release/MemoryLabSmoke.exe'
VALIDATE = ROOT / 'outputs/plugin-build/MemoryLabValidate_artefacts/Release/MemoryLabValidate.exe'
BUNDLE = ROOT / 'outputs/plugin-build/MemoryLab_artefacts/Release/VST3/Memory Lab Study.vst3'
subprocess.run([str(SMOKE), '--factory-patches', str(OUT)], check=True)
names = ['Clean Foundation', 'Round Bass', 'Soft Fifth', 'Slow Air', 'Glass Pluck',
         'Noise Tap', 'Breath Swell', 'Clockwork Pulse']
rows = []
for index, name in enumerate(names, 1):
    stem = OUT / str(index)
    result = subprocess.run([str(VALIDATE), '--capture-patch', str(BUNDLE), str(stem),
                             str(stem.with_suffix('.mlpatch')), 'default'],
                            capture_output=True, text=True, check=True)
    stem.with_suffix('.log').write_text(result.stdout + result.stderr, encoding='utf-8')
    raw = stem.with_suffix('.f32').read_bytes()
    samples = struct.unpack('<' + 'f' * (len(raw) // 4), raw)
    assert all(math.isfinite(v) and abs(v) < 1 for v in samples), name
    rms = math.sqrt(sum(v*v for v in samples) / len(samples))
    assert rms > 1e-6, name
    with wave.open(str(stem.with_suffix('.wav')), 'wb') as wav:
        wav.setnchannels(1); wav.setsampwidth(2); wav.setframerate(48000)
        wav.writeframes(struct.pack('<' + 'h'*len(samples), *(round(v*32767) for v in samples)))
    rows.append(dict(name=name, index=index, peak=max(map(abs, samples)), rms=rms,
                     raw_sha256=hashlib.sha256(raw).hexdigest()))
(OUT / 'measurements.json').write_text(json.dumps(rows, indent=2), encoding='utf-8')
cards = ''.join(f'<article><h2>{r["name"]}</h2><audio controls preload="none" src="{r["index"]}.wav"></audio>'
                f'<p><a href="{r["index"]}.mlpatch">Patch</a></p></article>' for r in rows)
(OUT / 'index.html').write_text('''<!doctype html><html lang="en"><meta charset="utf-8">
<meta name="viewport" content="width=device-width,initial-scale=1"><title>0.13 Starting sounds</title>
<style>body{background:#101819;color:#e5eee8;font:18px system-ui;max-width:850px;margin:40px auto;padding:20px}article{background:#1a2729;padding:20px;margin:16px 0}audio{width:100%}a{color:#b9e7cb}</style>
<h1>0.13 Starting sounds</h1><p>Actual VST3 captures. Same E-minor chord, output 0.25, both drives zero.
No normalization. Three-second clips: slow attacks are only partly demonstrated; hold notes longer in the instrument.
Clockwork Pulse enables the arp. These technical fixtures do not establish musical approval.</p>'''
    + cards + '</html>', encoding='utf-8')
print(f'PASS: eight starting sounds rendered through actual VST3; max peak {max(r["peak"] for r in rows):.6f}')
print(OUT / 'index.html')
