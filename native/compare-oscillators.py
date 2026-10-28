"""Independent harmonic reference and retained candidate/baseline listening set."""
import json
from pathlib import Path
import numpy as np
from scipy.io import wavfile

ROOT=Path(__file__).resolve().parents[1]
BASE=ROOT/'outputs/oscillator-audit-2'
CANDIDATE=ROOT/'outputs/oscillator-table-1'
OUT=ROOT/'outputs/oscillator-comparison-1'

def reference(phase,dt):
    allowed=max(1,.45/dt);level=min(9,int(np.log2(allowed)))
    blend=np.clip(np.log2(allowed)-level,0,1)
    low=1 if level==0 else 2**(level-1);high=2**level
    y=np.zeros(len(phase))
    for h in range(1,high+1):
        weight=1 if h<=low else blend
        y-=weight*2*np.sin(2*np.pi*h*phase)/(np.pi*h)
    return y

def main():
    OUT.mkdir(parents=True,exist_ok=False)
    (OUT/'contract.json').write_text(json.dumps({'reference_cases':'all 18 cases at MIDI84/108 across rates and widths',
        'reference_peak_error_limit':1e-4,'reference':'direct double-precision Fourier sum with specified bank harmonic weights; independent of table generation and lookup',
        'listening':'MIDI60/108, widths .1/.43; baseline/candidate, before/through drive, -24 dBFS whole-clip RMS; 16 clips',
        'limits':'static signals only; modulation/transients and CPU deadlines not certified'},indent=2))
    records=[]
    for sr in [44100,48000,96000]:
        for note in [84,108]:
            for width in [.1,.43,.9]:
                name=f'{sr}-{note}-{width}';actual=np.fromfile(CANDIDATE/(name+'.f32'),dtype='<f4').reshape(-1,4)
                t=np.arange(2*sr);dt=440*2**((note-69)/12)/sr;dt2=dt*2**(7/1200)
                phase=t*dt;phase2=.17+t*dt2
                saw=reference(phase,dt);pulse=reference(phase2-width,dt2)-reference(phase2,dt2)
                errors=[float(np.max(abs(actual[:,i]-expected))) for i,expected in enumerate([saw,pulse])]
                records.append({'id':name,'max_absolute_error':errors,'pass':max(errors)<1e-4})
    (OUT/'reference-report.json').write_text(json.dumps(records,indent=2))
    assert all(r['pass'] for r in records),'independent Fourier-reference error'
    clips=[]
    for note in [60,108]:
        for width in [.1,.43]:
            for source,folder in [('baseline',BASE),('wavetable',CANDIDATE)]:
                x=np.fromfile(folder/(f'48000-{note}-{width}.f32'),dtype='<f4').reshape(-1,4)
                for col,stage in [(2,'source'),(3,'driven')]:
                    y=x[:,col].astype(float);y*=10**(-24/20)/np.sqrt(np.mean(y*y))
                    assert np.max(abs(y))<.98
                    name=f'{note}-{width}-{source}-{stage}.wav';wavfile.write(OUT/name,48000,y.astype(np.float32))
                    clips.append({'file':name,'label':f'MIDI {note} · width {width} · {source} · {stage}'})
    (OUT/'manifest.json').write_text(json.dumps(clips,indent=2))
    buttons=''.join(f'<button data-file="{c["file"]}">{c["label"]}</button>' for c in clips)
    html='''<!doctype html><html lang="en"><meta charset="utf-8"><meta name="viewport" content="width=device-width"><title>Oscillator comparison</title>
<style>body{background:#101d19;color:#e6efdf;font:17px system-ui;max-width:900px;margin:30px auto;padding:20px}button{padding:12px;margin:5px;border:0;border-radius:5px;background:#c3e3a3;color:#142116}audio{width:100%;margin:15px 0}#choices button{display:block}a{color:#c3e3a3}</style>
<h1>Native oscillator comparison</h1><p>Choose a take, then press Play. Baseline and wavetable use the same phase, pitch, width and drive settings. These are isolated oscillator mixtures, without the synth filter or note envelope.</p>
<p id="selected" role="status">Select a take</p><audio id="audio" controls preload="none"></audio><button id="replay">Replay from start</button><button id="stop">Stop</button>
<p>Each two-second take is matched to −24 dBFS RMS. Perceived loudness may differ. MIDI 108 is intentionally a demanding high-note test. Keep playback comfortable.</p><div id="choices">BUTTONS</div>
<p>Original research, not recordings or an emulation of Relic. The approved browser instrument remains unchanged. <a href="../../live.html">Return to playable study</a>.</p>
<script>const a=document.getElementById('audio'),s=document.getElementById('selected');document.querySelectorAll('[data-file]').forEach(b=>b.onclick=()=>{a.pause();a.src=b.dataset.file;a.load();s.textContent=b.textContent+' · ready';});document.getElementById('stop').onclick=()=>{a.pause();a.currentTime=0;};document.getElementById('replay').onclick=()=>{if(a.getAttribute('src')){a.currentTime=0;a.play().catch(e=>s.textContent=e.message);}};a.onerror=()=>s.textContent='Audio could not load';</script></html>'''.replace('BUTTONS',buttons)
    (OUT/'index.html').write_text(html,encoding='utf-8')
    print(json.dumps({'fourier_cases':len(records),'max_reference_error':max(v for r in records for v in r['max_absolute_error']),
        'listening_clips':len(clips),'player':str(OUT/'index.html')}))

if __name__=='__main__':main()
