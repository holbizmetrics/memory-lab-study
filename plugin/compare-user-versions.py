"""Compare preserved 0.7/0.8 VST3s using Holger's saved settings and fixed MIDI."""
import argparse, hashlib, html, json, math, pathlib, struct, subprocess, xml.etree.ElementTree as ET
ROOT=pathlib.Path(__file__).resolve().parents[1]
parser=argparse.ArgumentParser(description=__doc__)
parser.add_argument('--output',default='outputs/user-version-comparison-1')
parser.add_argument('--patch',default="outputs/Holger's Test Output - mlpatches/Memory Lab Study - White Noise Min - Filter Classic.mlpatch")
parser.add_argument('--drive-only',action='store_true',help='Render only the drive-enabled version pair')
args=parser.parse_args(); out=ROOT/args.output; out.mkdir(parents=True,exist_ok=False)
source=ROOT/args.patch
original={e.attrib['id']:float(e.attrib['value']) for e in ET.parse(source).getroot()}
assert len(original)==41 and original['filterFamily']==0 and original['noiseLevel'] in (0,1)
modes=('default',) if args.drive_only else ('default','clean')
host=ROOT/'outputs/plugin-build/MemoryLabValidate_artefacts/Release/MemoryLabValidate.exe'
manifest=dict(source_patch=str(source),source_sha256=hashlib.sha256(source.read_bytes()).hexdigest(),
    midi=dict(notes=[52,55,59],channel=1,velocity=100,note_on_frame=4800,note_off_frame=76800,
              frames=144000,sample_rate=48000,block_size=512,silent_settling_blocks=100),
    limits='Fixed synthetic chord, not the FL song MIDI. Host-state application/readback, not editor LOAD. Mono left-channel captures; original signal duplicated to stereo by the instrument.',
    captures=[],comparisons=[])
samples={}; sections=[]
for version in ('0.7','0.8'):
    bundle=ROOT/f'outputs/distortion-audit-0.8-1/{version}/Memory Lab Study.vst3'
    binary=bundle/'Contents/x86_64-win/Memory Lab Study.vst3'
    tree=ET.parse(source)
    if version=='0.7':
        root=tree.getroot(); root.set('schemaVersion','4')
        for entry in list(root):
            if entry.attrib['id']=='filterFamily': root.remove(entry)
        assert len(root)==40
    patch=out/('v'+version.replace('.','')+'.mlpatch'); tree.write(patch,encoding='utf-8',xml_declaration=True)
    controls=[]
    for mode in modes:
        stem=out/('v'+version.replace('.','')+'-'+mode)
        run=subprocess.run([str(host),'--capture-patch',str(bundle),str(stem),str(patch),mode],capture_output=True,text=True,timeout=40)
        pathlib.Path(str(stem)+'.log').write_text(run.stdout+run.stderr,encoding='utf-8')
        if run.returncode: raise RuntimeError(run.stdout+run.stderr)
        raw=pathlib.Path(str(stem)+'.f32').read_bytes(); values=[v[0] for v in struct.iter_unpack('<f',raw)]
        assert len(values)==144000 and all(math.isfinite(v) for v in values)
        samples[version,mode]=values
        rms=math.sqrt(sum(v*v for v in values)/len(values)); peak=max(map(abs,values))
        row=dict(version=version,mode=mode,module_sha256=hashlib.sha256(binary.read_bytes()).hexdigest(),
                 patch_sha256=hashlib.sha256(patch.read_bytes()).hexdigest(),raw_sha256=hashlib.sha256(raw).hexdigest(),
                 rms=rms,peak=peak,crest_db=20*math.log10(peak/rms),full_scale_samples=sum(abs(v)>=1 for v in values),
                 overrides={} if mode=='default' else dict(drive=0,depth=0,voiceDrive=0))
        manifest['captures'].append(row)
        for matched in (False,True):
            gain=10**(-24/20)/rms if matched else 1.
            if matched: assert peak*gain<.95
            pcm=b''.join(struct.pack('<f',v*gain) for v in values)
            header=b'RIFF'+struct.pack('<I',36+len(pcm))+b'WAVEfmt '+struct.pack('<IHHIIHH',16,3,1,48000,192000,4,32)+b'data'+struct.pack('<I',len(pcm))
            suffix='-matched.wav' if matched else '.wav'
            wav=pathlib.Path(str(stem)+suffix); wav.write_bytes(header+pcm)
            row['matched_wav' if matched else 'original_wav']=wav.name
        label='Drive enabled' if mode=='default' else 'Drive bypassed'
        controls.append(f'<h3>{label}</h3><audio controls preload="none" src="{stem.name}-matched.wav"></audio>')
    sections.append(f'<section><h2>Version {version}</h2>'+''.join(controls)+'</section>')
for mode in modes:
    a=samples['0.7',mode]; b=samples['0.8',mode]; delta=[y-x for x,y in zip(a,b)]
    manifest['comparisons'].append(dict(mode=mode,sample_identical=a==b,max_abs_diff=max(map(abs,delta)),
                                        difference_rms=math.sqrt(sum(v*v for v in delta)/len(delta))))
(out/'manifest.json').write_text(json.dumps(manifest,indent=2),encoding='utf-8')
(out/'index.html').write_text('''<!doctype html><meta charset="utf-8"><title>0.7 vs 0.8: saved Classic settings</title><style>body{font:18px system-ui;background:#101819;color:#e5eee8;max-width:900px;margin:32px auto;padding:16px}section{background:#1a2729;padding:16px;margin:20px 0}audio{width:100%}</style><h1>0.7 versus 0.8: your saved Classic settings</h1><p>Source: '''+html.escape(source.name)+'''</p><p>Actual preserved VST3 modules, same fixed E-minor chord (not the FL song), 48 kHz / 512 frames, velocity 100. All shared parameters preserved. The 0.7 patch omits only the unsupported filter-family parameter; 0.8 explicitly selects Classic.</p><p>These listening copies are matched to -24 dBFS RMS; original WAVs/raw floats, readback text, module hashes and difference metrics remain beside them. Drive bypass, if included, zeros drive, depth and voice drive only. No installed plugin change. Equal samples apply to this fixture, not every possible MIDI/state history. Browser playback has not been verified.</p>'''+''.join(sections)+'''<script>document.addEventListener('play',e=>{document.querySelectorAll('audio').forEach(a=>{if(a!==e.target)a.pause()})},true)</script>''',encoding='utf-8')
print(json.dumps(manifest['comparisons'],indent=2))
