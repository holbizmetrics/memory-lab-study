import pathlib, struct, math, json, html
root=pathlib.Path('outputs/holger-plugin-captures-2')
patches=sorted(pathlib.Path("outputs/Holger's Test Output - mlpatches").glob('*.mlpatch'))
rows=[]; sections=[]
for i,patch in enumerate(patches):
    label=patch.stem.removeprefix('Memory Lab Study - ')
    players=[]
    for mode in ('default','clean'):
        stem=f'{i:02}-{mode}'; raw=(root/(stem+'.f32')).read_bytes()
        values=[v[0] for v in struct.iter_unpack('<f',raw)]
        assert len(values)==144000 and all(math.isfinite(v) for v in values)
        rms=math.sqrt(sum(v*v for v in values)/len(values)); peak=max(map(abs,values))
        gain=10**(-24/20)/rms
        assert gain*peak<.95
        audio=b''.join(struct.pack('<f',v*gain) for v in values)
        header=b'RIFF'+struct.pack('<I',36+len(audio))+b'WAVEfmt '+struct.pack('<IHHIIHH',16,3,1,48000,192000,4,32)+b'data'+struct.pack('<I',len(audio))
        (root/(stem+'-matched.wav')).write_bytes(header+audio)
        row=dict(patch=patch.name,mode=mode,rms=rms,peak=peak,crest_db=20*math.log10(peak/rms),full_scale_samples=sum(abs(v)>=1 for v in values))
        rows.append(row)
        title='Drive enabled' if mode=='default' else 'Drive bypassed (filter/noise preserved)'
        players.append(f'<p>{title}</p><audio controls preload="none" src="{stem}-matched.wav"></audio>')
    sections.append('<section><h2>'+html.escape(label)+'</h2>'+''.join(players)+'</section>')
(root/'measurements.json').write_text(json.dumps(rows,indent=2),encoding='utf-8')
(root/'index.html').write_text('''<!doctype html><meta charset="utf-8"><title>Actual plugin: saved patch comparisons</title><style>body{font:18px system-ui;background:#101819;color:#e5eee8;max-width:900px;margin:32px auto;padding:16px}section{background:#1a2729;padding:16px;margin:16px 0}h2{font-size:20px}audio{width:100%}</style><h1>Saved settings through the actual 0.8 plugin</h1><p>Fixed E-minor chord, velocity 100, 48 kHz, 512-frame blocks, 3 seconds. All 41 patch values read back after host-state application. This exercises host-state recall, not the editor's patch-file loader. The phrase is controlled and is not claimed identical to Holger's FL phrase.</p><p>Each clip is matched to -24 dBFS RMS with peak headroom. Originals remain untouched, including over-full-scale floating-point samples in LP4 MAX with drive bypassed. No limiter was added. Bypass zeros shared drive, memory depth and voice drive; filter and noise settings stay intact. Browser playback has not been verified.</p>'''+''.join(sections)+'''<script>document.addEventListener('play',e=>{document.querySelectorAll('audio').forEach(a=>{if(a!==e.target)a.pause()})},true)</script>''',encoding='utf-8')
for i in range(8):
    wet,dry=rows[2*i:2*i+2]
    print(f"{wet['patch']}: RMS wet/dry {20*math.log10(wet['rms']/dry['rms']):.2f} dB; crest {wet['crest_db']:.2f}/{dry['crest_db']:.2f}; dry overs {dry['full_scale_samples']}")
