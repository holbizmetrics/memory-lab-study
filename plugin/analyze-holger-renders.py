import pathlib, struct, math, json, hashlib, html
root=pathlib.Path("outputs/Holger's Test Output - mlpatches")
rows=[]
listening=pathlib.Path('outputs/holger-filter-listening-1')
listening.mkdir(exist_ok=True)
players=[]
for path in sorted(root.glob('*.wav')):
    raw=path.read_bytes()
    assert raw[:4]==b'RIFF' and raw[8:12]==b'WAVE'
    p=12; fmt=None; data=None
    while p+8<=len(raw):
        name,size=struct.unpack_from('<4sI',raw,p); p+=8
        if name==b'fmt ': fmt=struct.unpack_from('<HHIIHH',raw,p)
        if name==b'data': data=raw[p:p+size]
        p+=size+(size%2)
    kind,ch,rate,_,align,bits=fmt
    assert kind==3 and bits==32 and data is not None
    samples=[v[0] for v in struct.iter_unpack('<f',data)]
    invalid=sum(not math.isfinite(v) for v in samples)
    assert not invalid
    peak=max(map(abs,samples)); rms=math.sqrt(sum(v*v for v in samples)/len(samples))
    rows.append(dict(file=path.name,rate=rate,bits=bits,channels=ch,seconds=len(data)/align/rate,
        peak_dbfs=20*math.log10(peak),rms_dbfs=20*math.log10(rms),crest_db=20*math.log10(peak/rms),
        full_scale_samples=sum(abs(v)>=1 for v in samples),invalid_samples=invalid,sha256=hashlib.sha256(raw).hexdigest()))
    gain=10**(-24/20)/rms
    assert peak*gain<.95
    rendered=b''.join(struct.pack('<f',v*gain) for v in samples)
    header=b'RIFF'+struct.pack('<I',36+len(rendered))+b'WAVEfmt '+struct.pack('<IHHIIHH',16,3,ch,rate,rate*align,align,32)+b'data'+struct.pack('<I',len(rendered))
    output=path.name.replace('.mlpatch.wav','-matched.wav')
    (listening/output).write_bytes(header+rendered)
    label=path.name.removeprefix('Memory Lab Study - ').removesuffix('.mlpatch.wav')
    players.append(f'<section><h2>{html.escape(label)}</h2><audio controls preload="none" src="{html.escape(output,quote=True)}"></audio></section>')
assert len(rows)==8, f'Expected eight WAVs, found {len(rows)}'
out=pathlib.Path('outputs/holger-renders-analysis.json')
out.write_text(json.dumps(rows,indent=2),encoding='utf-8')
(listening/'index.html').write_text('''<!doctype html><meta charset="utf-8"><title>Holger filter comparison</title><style>body{font:18px system-ui;background:#101819;color:#e5eee8;max-width:900px;margin:32px auto;padding:16px}section{background:#1a2729;padding:16px;margin:16px 0}h2{font-size:20px}audio{width:100%}</style><h1>Your FL filter exports</h1><p>Eight FL exports, each matched to -24 dBFS whole-file RMS. Originals are untouched. Gain matching helps compare timbre; it does not remove distortion. Durations differ by about 4.8 ms. These are not sample-aligned null comparisons, nor proof of identical MIDI or patch recall. Start playback manually.</p>'''+''.join(players)+'''<script>document.addEventListener('play',e=>{document.querySelectorAll('audio').forEach(a=>{if(a!==e.target)a.pause()})},true)</script>''',encoding='utf-8')
for r in rows:
    print(f"{r['file']}: {r['rate']} Hz, {r['seconds']:.4f}s, peak {r['peak_dbfs']:.2f}, RMS {r['rms_dbfs']:.2f}, crest {r['crest_db']:.2f} dB, overs {r['full_scale_samples']}")
