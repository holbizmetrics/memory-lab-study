"""Classical response targets via independent bilinear-transform reference."""
import hashlib
import json
from pathlib import Path
import subprocess
import numpy as np
from scipy import signal
from scipy.io import wavfile

ROOT=Path(__file__).resolve().parents[1];OUT=ROOT/'outputs/filter-audit-1';EXE=ROOT/'outputs/native-build/memorylab_native.exe'

def main():
    OUT.mkdir(parents=True,exist_ok=False)
    contract={'sample_rates':[44100,48000,96000],'cutoffs':[30,300,1450,12000],
      'q':[.5,2.4,8],'profile':[0,.25,.5,.75,1],'impulse_seconds':2,
      'target':'bilinear-transform analogue denominator s^2+(w0/Q)s+w0^2, w0 prewarped; HP/notch/LP linear blend',
      'sample_max_error':2e-8,'complex_response_max_error':2e-6,
      'limitations':'original classic SVF; no independent bandwidth knob or hardware identity; sweep listening unassessed',
      'code_sha256':{name:hashlib.sha256((ROOT/'native'/name).read_bytes()).hexdigest() for name in ['voice.hpp','main.cpp','filter-audit.py']},
      'exe_sha256':hashlib.sha256(EXE.read_bytes()).hexdigest()}
    (OUT/'contract.json').write_text(json.dumps(contract,indent=2))
    rows=[]
    for sr in contract['sample_rates']:
      for cutoff in contract['cutoffs']:
       for q in contract['q']:
        for profile in contract['profile']:
          name=f'{sr}-{cutoff}-{q}-{profile}';file=OUT/(name+'.f64')
          subprocess.run([str(EXE),'--filter-impulse',str(file),str(sr),str(cutoff),str(q),str(profile)],check=True)
          actual=np.fromfile(file,dtype='<f8');assert len(actual)==2*sr and np.isfinite(actual).all()
          w0=2*sr*np.tan(np.pi*cutoff/sr);hw=1 if profile<=.5 else 2-2*profile;lw=2*profile if profile<=.5 else 1
          b,a=signal.bilinear([hw,0,lw*w0*w0],[1,w0/q,w0*w0],fs=sr)
          impulse=np.zeros(len(actual));impulse[0]=1;expected=signal.lfilter(b,a,impulse)
          error=float(np.max(abs(actual-expected)))
          frequencies=np.fft.rfftfreq(len(actual),1/sr)
          _,target=signal.freqz(b,a,worN=2*np.pi*frequencies/sr)
          response_error=float(np.max(abs(np.fft.rfft(actual)-target)))
          assert error<contract['sample_max_error'] and response_error<contract['complex_response_max_error'],(name,error,response_error)
          rows.append({'id':name,'sample_error':error,'complex_response_error':response_error,'peak':float(max(abs(actual)))})
    clips=[]
    for mode,label in enumerate(['High-pass','Notch','Low-pass baseline','Profile sweep: HP → notch → LP']):
        file=OUT/f'voice-{mode}.f32';subprocess.run([str(EXE),'--profile-voice',str(file),str(mode)],check=True)
        raw=np.fromfile(file,dtype='<f4');assert len(raw)==326400 and np.isfinite(raw).all()
        matched=raw.astype(float)*10**(-24/20)/np.sqrt(np.mean(raw.astype(float)**2));assert max(abs(matched))<.98
        wavfile.write(OUT/f'{mode}.wav',48000,matched.astype(np.float32));clips.append({'mode':mode,'label':label,'raw_rms':float(np.sqrt(np.mean(raw.astype(float)**2)))})
    report={'response_cases':len(rows),'measurements':rows,'clips':clips,'listening':'UNASSESSED'}
    (OUT/'report.json').write_text(json.dumps(report,indent=2))
    cards=''.join(f'<p>{c["label"]}</p><audio controls preload="metadata" src="{c["mode"]}.wav"></audio>' for c in clips)
    (OUT/'index.html').write_text('<!doctype html><html lang="en"><meta charset="utf-8"><title>Classic filter profiles</title><style>body{font:18px system-ui;background:#101d19;color:#e6efdf;max-width:850px;margin:40px auto;padding:20px}audio{width:100%}</style><h1>Classic filter profiles</h1><p>The same seven-note phrase, oscillators and memory drive. Static cutoff 1450 Hz, Q 2.4. Each complete clip matched to −24 dBFS RMS, preserving within-phrase dynamics. Press Play on a clip; no autoplay.</p>'+cards+'<p>The sweep changes only filter profile, from HP through notch to LP over six seconds. It contains the same seven key presses as the other clips. The three static profiles are references, not three drive placements. Original design; no Relic circuit or hardware-match claim.</p><script>const players=[...document.querySelectorAll("audio")];players.forEach(p=>p.onplay=()=>players.forEach(q=>{if(q!==p)q.pause();}));</script></html>',encoding='utf-8')
    print(json.dumps({'cases':len(rows),'max_sample_error':max(r['sample_error'] for r in rows),'max_complex_response_error':max(r['complex_response_error'] for r in rows),'clips':len(clips)}),flush=True)

if __name__=='__main__':main()
