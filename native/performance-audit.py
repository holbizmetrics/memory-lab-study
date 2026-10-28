"""First performance-event checkpoint; preserve approved disabled-controller patch."""
import hashlib
import json
from pathlib import Path
import subprocess
import numpy as np
from scipy.io import wavfile
from scipy.optimize import minimize_scalar

ROOT=Path(__file__).resolve().parents[1];OUT=ROOT/'outputs/performance-audit-1';EXE=ROOT/'outputs/native-build/memorylab_native.exe'

def main():
    OUT.mkdir(parents=True,exist_ok=False)
    contract={'sample_rates':[44100,48000,96000],'bend_range':'fixed +/-2 semitones; normalized -1..1',
      'sustain':'channel scoped; threshold.5; note-off clears key ownership and pedal can retain envelope gate',
      'pressure':'channel pressure0..1, optional octave-depth-to-cutoff; default depth0',
      'smoothing_ms':20,'replacement_transition_ms':5,
      'criteria':'semantic assertions, exact block invariance, finite/headroom, no test new allocations, unchanged baseline exact, settled two-oscillator bend estimate within25cents of undetuned target',
      'limitations':'not MPE/poly-pressure; 8 slots; same channel/pitch retriggers one slot; 5ms held-value transition is not a complete release-tail crossfade',
      'code_sha256':{n:hashlib.sha256((ROOT/'native'/n).read_bytes()).hexdigest() for n in ['voice.hpp','main.cpp','performance-audit.py']},
      'exe_sha256':hashlib.sha256(EXE.read_bytes()).hexdigest()}
    (OUT/'contract.json').write_text(json.dumps(contract,indent=2))
    run=subprocess.run([str(EXE),'--performance-test'],capture_output=True,text=True);(OUT/'native-checks.txt').write_text(run.stdout+run.stderr);run.check_returncode()
    sr,default=wavfile.read(ROOT/'outputs/native-phrase.wav');approved=np.fromfile(ROOT/'outputs/filter-profiles-1/2-raw.f32',dtype='<f4')
    assert sr==48000 and np.array_equal(default,approved),'performance layer changed default patch'
    labels=['Sustain pedal','Pitch bend','Channel pressure → cutoff','Eight-voice limit and repeated note']
    descriptions=['One C4 key press. Key released at0.75s; pedal keeps it sounding until2.5s, then the release tail fades.',
      'One held C4. Bend goes +2 semitones at0.5s, -2 at1.5s, and back to center at2.5s. Key released at3.2s.',
      'One held C4. Pressure opens cutoff by one octave at0.5s, returns at1.5s, and opens again at2.5s. Key released at3.2s.',
      'Nine staggered notes fill eight voice slots; the oldest is replaced. G4 retriggers at1.6s, then releases at2s. All remaining notes release at2.5s.']
    clips=[];bend_errors=[]
    for mode,label in enumerate(labels):
        file=OUT/f'{mode}-raw.f32';subprocess.run([str(EXE),'--performance-voice',str(file),str(mode)],check=True)
        raw=np.fromfile(file,dtype='<f4');assert len(raw)==192000 and np.isfinite(raw).all() and max(abs(raw))<1
        if mode==1:
            for start,note in [(.9,62),(1.9,58),(2.9,60)]:
                x=raw[int(start*48000):int((start+.25)*48000)].astype(float);x=(x-x.mean())*np.hanning(len(x));t=np.arange(len(x))/48000
                intended=440*2**((note-69)/12)
                # Target is the undetuned first oscillator; second remains +7c.
                # Window cannot cleanly resolve that pair, so use a loose aggregate
                # check of +/-25c, not a false one-cent individual-oscillator claim.
                def cost(f):return -abs(np.sum(x*np.exp(-2j*np.pi*f*t)))**2
                fit=minimize_scalar(cost,bounds=(intended*2**(-80/1200),intended*2**(80/1200)),method='bounded')
                cents=float(1200*np.log2(fit.x/intended));assert abs(cents)<25;bend_errors.append(cents)
        gain=10**(-24/20)/np.sqrt(np.mean(raw.astype(float)**2));matched=raw.astype(float)*gain;assert max(abs(matched))<.98
        wavfile.write(OUT/f'{mode}.wav',48000,matched.astype(np.float32));_,decoded=wavfile.read(OUT/f'{mode}.wav')
        rms=float(20*np.log10(np.sqrt(np.mean(decoded.astype(float)**2))));assert abs(rms+24)<.001
        clips.append({'mode':mode,'label':label,'rms_dbfs':rms,'peak':float(max(abs(decoded)))})
    report={'clips':clips,'baseline_exact':True,'bend_pair_estimate_cents_from_first_oscillator':bend_errors,'native_checks':run.stdout.strip(),'listening':'UNASSESSED'}
    (OUT/'report.json').write_text(json.dumps(report,indent=2))
    cards=''.join(f'<section><h2>{c["label"]}</h2><p>{descriptions[c["mode"]]}</p><audio controls preload="metadata" src="{c["mode"]}.wav"></audio></section>' for c in clips)
    (OUT/'index.html').write_text('<!doctype html><html lang="en"><meta charset="utf-8"><meta name="viewport" content="width=device-width"><title>Native performance study</title><style>body{font:17px system-ui;background:#101d19;color:#e6efdf;max-width:850px;margin:30px auto;padding:20px}section{background:#1c3026;padding:18px;margin:18px 0;border-radius:8px}h2{font-size:21px}audio{width:100%}</style><h1>Native performance study</h1><p>Four-second event demonstrations. Each clip has its own explicitly described key/controller sequence. The oscillator, LP filter and memory-drive defaults remain. Clips are globally matched to −24dBFS RMS; nothing autoplays.</p>'+cards+'<p>Original instrument research, not hardware recordings. This tests the native event core; the existing browser MIDI prototype has not been upgraded with these controls yet.</p><script>const players=[...document.querySelectorAll("audio")];players.forEach(p=>p.onplay=()=>players.forEach(q=>{if(q!==p)q.pause();}));</script></html>',encoding='utf-8')
    print(json.dumps(report),flush=True)

if __name__=='__main__':main()
