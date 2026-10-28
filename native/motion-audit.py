"""Dynamic oscillator/control audit; raw outputs retained and no listening claim."""
import hashlib
import json
from pathlib import Path
import subprocess
import numpy as np
from scipy.io import wavfile

ROOT=Path(__file__).resolve().parents[1]
OUT=ROOT/'outputs/motion-audit-1'
EXE=ROOT/'outputs/native-build/memorylab_native.exe'

def fourier(phase,dt):
    octave=np.log2(.45/dt);level=np.floor(octave).astype(int);blend=octave-level
    high=2.**level;low=high/2;out=np.zeros(len(phase))
    for h in range(1,17):
        weight=np.where(h<=low,1,np.where(h<=high,blend,0))
        out-=weight*2*np.sin(2*np.pi*h*phase)/(np.pi*h)
    return out

def main():
    OUT.mkdir(parents=True,exist_ok=False)
    contract={'rates':[44100,48000,96000],'scenarios':['pulse-width steps','detune steps','combined steps'],
      'control_smoothing_ms':20,'bank_boundary':'dt=.45/16, crossed with +/-30 cent target steps',
      'reference_limits':{'sample_error':1e-4,'state_error':1e-10,'boundary_jump':1e-6},
      'scope':'continuity at fixed phase, actual full voice event/block behavior, dynamic bank Fourier comparison; not perceptual clicklessness or total alias certification',
      'code_sha256':{name:hashlib.sha256((ROOT/'native'/name).read_bytes()).hexdigest() for name in ['voice.hpp','main.cpp','motion-audit.py']},
      'executable_sha256':hashlib.sha256(EXE.read_bytes()).hexdigest()}
    (OUT/'contract.json').write_text(json.dumps(contract,indent=2))
    run=subprocess.run([str(EXE),'--motion-test'],capture_output=True,text=True)
    (OUT/'native-checks.txt').write_text(run.stdout+run.stderr);run.check_returncode()
    records=[]
    for sr in contract['rates']:
        file=OUT/f'bank-{sr}.f64';subprocess.run([str(EXE),'--bank-motion',str(file),str(sr)],check=True)
        x=np.fromfile(file,dtype='<f8').reshape(-1,8);assert len(x)==2*sr and np.isfinite(x).all()
        a=fourier(x[:,2],x[:,4]);b=fourier(x[:,3]-x[:,5],x[:,4])-fourier(x[:,3],x[:,4])
        sample_error=max(float(np.max(abs(x[:,0]-a))),float(np.max(abs(x[:,1]-b))))
        detune=7.;width=.43;smooth=1-np.exp(-1/(sr*.02));state_error=0
        for i in range(2*sr):
            t=i/sr;targetD=7 if t<.5 or t>=1.5 else -30 if t<1 else 30
            targetW=.43 if t<.5 or t>=1.5 else .1 if t<1 else .9
            detune+=(targetD-detune)*smooth;width+=(targetW-width)*smooth
            state_error=max(state_error,abs(x[i,4]-.45/16*2**(detune/1200)),abs(x[i,5]-width))
        assert sample_error<1e-4 and state_error<1e-10
        records.append({'sample_rate':sr,'max_reference_error':sample_error,'max_state_error':state_error})
        for scenario in range(3):
            raw=OUT/f'voice-{sr}-{scenario}.f32';subprocess.run([str(EXE),'--motion-voice',str(raw),str(sr),str(scenario)],check=True)
            voice=np.fromfile(raw,dtype='<f4');assert len(voice)==3*sr and np.isfinite(voice).all() and max(abs(voice))<1
            if sr==48000:
                matched=voice.astype(float)*10**(-24/20)/np.sqrt(np.mean(voice.astype(float)**2));assert max(abs(matched))<.98
                wavfile.write(OUT/f'{scenario}.wav',sr,matched.astype(np.float32))
    report={'dynamic_reference':records,'native_checks':run.stdout.strip(),'voice_renders':9,'listening':'UNASSESSED'}
    (OUT/'report.json').write_text(json.dumps(report,indent=2))
    cards=''.join(f'<p>{name}</p><audio controls preload="metadata" src="{i}.wav"></audio>' for i,name in enumerate(contract['scenarios']))
    (OUT/'index.html').write_text('<!doctype html><html lang="en"><meta charset="utf-8"><title>Native oscillator motion</title><style>body{font:18px system-ui;background:#101d19;color:#e6efdf;max-width:850px;margin:40px auto;padding:20px}audio{width:100%}a{color:#c3e3a3}</style><h1>Native oscillator motion</h1><p>Three seconds each: MIDI60 held for two seconds then released. Controls change at 0.5, 1 and 1.5 seconds, with 20 ms smoothing. Each clip is globally matched to −24 dBFS RMS. Press Play on a clip; nothing autoplays.</p>'+cards+'<p>Original voice including filter and approved memory-drive settings. Not hardware recordings or a Relic emulation. Listen for unpleasant ticks or sudden jumps at the changes.</p><script>const players=[...document.querySelectorAll("audio")];players.forEach(p=>p.addEventListener("play",()=>players.forEach(other=>{if(other!==p)other.pause();})));</script></html>',encoding='utf-8')
    print(json.dumps(report),flush=True)

if __name__=='__main__':main()
