"""Isolated static-Q gain study through the preserved real VST3; no DSP/install edits."""
import argparse, hashlib, html, json, math, pathlib, struct, subprocess, xml.etree.ElementTree as ET

ROOT = pathlib.Path(__file__).resolve().parents[1]
parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument('--output', default='outputs/lp4-headroom-study-1')
args = parser.parse_args()
out = ROOT / args.output
out.mkdir(parents=True, exist_ok=False)
host = ROOT / 'outputs/plugin-build/MemoryLabValidate_artefacts/Release/MemoryLabValidate.exe'
bundle = ROOT / 'outputs/distortion-audit-0.8-1/0.8/Memory Lab Study.vst3'
sources = {}
for path in (ROOT / "outputs/Holger's Test Output - mlpatches").glob('*.mlpatch'):
    tree = ET.parse(path)
    values = {e.attrib['id']: float(e.attrib['value']) for e in tree.getroot()}
    if values['filterFamily'] == 1:
        sources[int(values['noiseLevel'])] = path
assert set(sources) == {0, 1}
rows = []; sections = []; linear_checks = []
for q in (3.72, 8.0):
    for noise in (0, 1):
        controls = []
        dry = {}
        for variant, family, gain in (('classic', 0, 1.), ('lp4-original', 1, 1.), ('lp4-compensated', 1, 1/q)):
            tree = ET.parse(sources[noise]); entries = {e.attrib['id']: e for e in tree.getroot()}
            entries['resonance'].set('value', str(q))
            entries['filterFamily'].set('value', str(family))
            # Static linear filter: scaling every source by 1/Q equals scaling its
            # settled LP4 output before either drive. No audio-engine change needed.
            for key in ('sawLevel', 'pulseLevel', 'noiseLevel'):
                entries[key].set('value', str(float(entries[key].attrib['value']) * gain))
            name = f'q{q:g}-noise{noise}-{variant}'.replace('.', 'p')
            patch = out / (name + '.mlpatch')
            tree.write(patch, encoding='utf-8', xml_declaration=True)
            audio = []
            for mode in ('default', 'clean'):
                stem = out / (name + '-' + mode)
                result = subprocess.run([str(host), '--capture-patch', str(bundle), str(stem), str(patch), mode], capture_output=True, text=True, timeout=40)
                stem.with_suffix('.log').write_text(result.stdout + result.stderr, encoding='utf-8')
                if result.returncode:
                    raise RuntimeError(result.stdout + result.stderr)
                raw = stem.with_suffix('.f32').read_bytes()
                values = [x[0] for x in struct.iter_unpack('<f', raw)]
                assert len(values) == 144000 and all(math.isfinite(v) for v in values)
                rms = math.sqrt(sum(v*v for v in values)/len(values)); peak = max(map(abs, values))
                level = 10**(-24/20)/rms
                assert peak*level < .95
                pcm = b''.join(struct.pack('<f', v*level) for v in values)
                header = b'RIFF'+struct.pack('<I',36+len(pcm))+b'WAVEfmt '+struct.pack('<IHHIIHH',16,3,1,48000,192000,4,32)+b'data'+struct.pack('<I',len(pcm))
                wav = out / (stem.name + '-matched.wav')
                wav.write_bytes(header + pcm)
                reread = [x[0] for x in struct.iter_unpack('<f', wav.read_bytes()[44:])]
                assert abs(20*math.log10(math.sqrt(sum(v*v for v in reread)/len(reread)))+24) < .001
                row = dict(q=q, noise=noise, variant=variant, mode=mode, source_scale=gain, rms=rms, peak=peak,
                           crest_db=20*math.log10(peak/rms), full_scale_samples=sum(abs(v)>=1 for v in values),
                           raw_sha256=hashlib.sha256(raw).hexdigest(), patch_sha256=hashlib.sha256(patch.read_bytes()).hexdigest())
                rows.append(row)
                title = 'Drive enabled' if mode == 'default' else 'Drive bypassed'
                audio.append(f'<p>{title}</p><audio controls preload="none" src="{html.escape(wav.name)}"></audio>')
                if mode == 'clean': dry[variant] = values
            controls.append(f'<article><h3>{variant}</h3>'+''.join(audio)+'</article>')
        error = max(abs(c-r/q) for c,r in zip(dry['lp4-compensated'], dry['lp4-original']))
        # Host controls are snapped to their .001 increments, so this is a measured
        # approximation of 1/Q, not an exact implementation of a future DSP option.
        relative = error / max(map(abs, dry['lp4-original']))
        linear_checks.append(dict(q=q, noise=noise, max_error=error, relative_to_original_peak=relative))
        assert relative < .004, f'Unexpected source scaling difference: {relative}'
        sections.append(f'<section><h2>Q {q:g} / noise {"MAX" if noise else "MIN"}</h2>'+''.join(controls)+'</section>')
(out/'measurements.json').write_text(json.dumps(dict(captures=rows, scaling_checks=linear_checks), indent=2),encoding='utf-8')
(out/'index.html').write_text('''<!doctype html><meta charset="utf-8"><title>LP4 headroom study</title><style>body{font:18px system-ui;background:#101819;color:#e5eee8;max-width:1100px;margin:32px auto;padding:16px}section{background:#1a2729;padding:16px;margin:20px 0}article{padding:12px;border-top:1px solid #43534e}audio{width:100%}h3{font-size:19px}</style><h1>LP4: isolated gain-compensation study</h1><p>Actual preserved 0.8 VST3, fixed E-minor chord, velocity 100, 48 kHz / 512 frames. Your settings, with Q 3.72 or 8 and noise MIN/MAX. Originals and installed plugin unchanged.</p><p>Each clip is matched to -24 dBFS RMS with peak headroom. Compare timbre and attack, not playback loudness. Compensation here reduces every source by 1/Q before the linear filter, approximating a post-filter pre-drive gain adjustment after control quantization. It does not merely turn down the final output. The resonance peak matches Classic at cutoff in the ideal model; full frequency responses remain different, and low-frequency level is also reduced. This is a candidate, not an approved fix, calibrated hardware model, or new synth version.</p><p>Drive-bypassed means drive/depth/voiceDrive zero, preserving the filter/noise choices. Raw over-full-scale samples are retained in float files; matched listening copies are attenuated without adding a limiter. This fixed phrase is not the FL song. Browser playback has not been verified.</p>'''+''.join(sections)+'''<script>document.addEventListener('play',e=>{document.querySelectorAll('audio').forEach(a=>{if(a!==e.target)a.pause()})},true)</script>''',encoding='utf-8')
for r in rows:
    if r['mode'] == 'default':
        print(f"Q={r['q']:g} noise={r['noise']} {r['variant']}: peak={r['peak']:.6f} rms={r['rms']:.6f} crest={r['crest_db']:.2f} dB")
print('PASS: 24 actual-plugin captures, 24 matched WAVs level-checked; four source-scaling checks within control quantization tolerance.')
