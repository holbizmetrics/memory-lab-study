"""Independent analysis of actual compiled oscillator fixtures; no hardware claim."""
import hashlib
import json
from pathlib import Path
import subprocess
import sys
import numpy as np
from scipy.optimize import minimize_scalar
from scipy.io import wavfile

ROOT=Path(__file__).resolve().parents[1]
OUT=ROOT/'outputs'/(sys.argv[1] if len(sys.argv)>1 else 'oscillator-audit-1')
EXE=ROOT/'outputs/native-build/memorylab_native.exe'
MODE=sys.argv[2] if len(sys.argv)>2 else '--osc-fixture'
if MODE not in ('--osc-fixture','--table-fixture'):raise ValueError('unknown fixture mode')

def pitch(audio,sr,expected):
    # One second, after startup. Search only +/-5 cents around known target;
    # also require that the fundamental is the strongest local candidate.
    x=audio[int(sr*.5):int(sr*1.5)].astype(float)
    x-=x.mean();x*=np.hanning(len(x));t=np.arange(len(x))/sr
    def score(f):return -abs(np.sum(x*np.exp(-2j*np.pi*f*t)))**2
    result=minimize_scalar(score,bounds=(expected*2**(-5/1200),expected*2**(5/1200)),method='bounded',options={'xatol':1e-9})
    return float(1200*np.log2(result.x/expected))

def residual(audio,sr,f):
    # A diagnostic, NOT total alias power: energy away from intended harmonic
    # lines. +/-6 Hz masks suppress Hann leakage. Mask overlap at high notes and
    # folded aliases landing on harmonic lines can hide aliasing.
    x=audio[int(sr*.5):int(sr*1.5)].astype(float)
    x-=x.mean();power=abs(np.fft.rfft(x*np.hanning(len(x))))**2
    bins=np.fft.rfftfreq(len(x),1/sr);mask=np.zeros(len(bins),dtype=bool)
    for harmonic in np.arange(f,sr/2,f):mask|=abs(bins-harmonic)<=6
    mask|=bins<10
    return float(10*np.log10(max(power[~mask].sum()/power.sum(),1e-20)))

def main():
    OUT.mkdir(parents=True,exist_ok=False)
    contract={'sample_rates':[44100,48000,96000],'notes':[36,60,84,108],'widths':[.1,.43,.9],
              'seconds':2,'detune_cents':7,'pitch_target_cents':1,'mode':MODE,
              'numerical_limits':'finite samples; abs(saw,pulse)<2.1; abs(mix,drive)<2',
              'spectral_diagnostic':'nonharmonic FFT energy with +/-6 Hz harmonic masks; characterize, not pass/fail alias threshold',
              'scope':'Oscillators isolated before filter; same mix before and through drive. No listener or hardware result.',
              'executable_sha256':hashlib.sha256(EXE.read_bytes()).hexdigest(),
              'code_sha256':{name:hashlib.sha256((ROOT/'native'/name).read_bytes()).hexdigest() for name in ['voice.hpp','main.cpp','oscillator-audit.py']}}
    (OUT/'contract.json').write_text(json.dumps(contract,indent=2))
    rows=[]
    for sr in contract['sample_rates']:
        for note in contract['notes']:
            for width in contract['widths']:
                name=f'{sr}-{note}-{width}'
                path=OUT/(name+'.f32')
                subprocess.run([str(EXE),MODE,str(path),str(sr),str(note),str(width)],check=True)
                x=np.fromfile(path,dtype='<f4').reshape(-1,4)
                f=440*2**((note-69)/12);f2=f*2**(7/1200)
                assert len(x)==2*sr and np.isfinite(x).all()
                assert np.max(abs(x[:,:2]))<2.1 and np.max(abs(x[:,2:]))<2
                errors=[pitch(x[:,0],sr,f),pitch(x[:,1],sr,f2)]
                rows.append({'id':name,'sample_rate':sr,'note':note,'width':width,
                             'pitch_error_cents':errors,'pitch_pass':max(map(abs,errors))<1,
                             'nonharmonic_energy_db':[residual(x[:,0],sr,f),residual(x[:,1],sr,f2)],
                             'rms_before_drive':float(np.sqrt(np.mean(x[:,2]**2))),
                             'rms_after_drive':float(np.sqrt(np.mean(x[:,3]**2)))})
                if sr==48000 and note==60:
                    for col,label in [(2,'source'),(3,'driven')]:
                        raw=x[:,col].astype(float);matched=raw*(10**(-24/20)/np.sqrt(np.mean(raw**2)))
                        assert np.max(abs(matched))<.98
                        wavfile.write(OUT/(f'width-{width}-{label}.wav'),sr,matched.astype(np.float32))
    report={'cases':rows,'pitch_pass_count':sum(r['pitch_pass'] for r in rows),'count':len(rows),
            'all_pitch_pass':all(r['pitch_pass'] for r in rows),'spectral_status':'CHARACTERIZED_ONLY; no alias acceptance claim'}
    (OUT/'report.json').write_text(json.dumps(report,indent=2))
    print(json.dumps({'count':len(rows),'pitch_pass_count':report['pitch_pass_count'],
      'max_pitch_error_cents':max(abs(v) for r in rows for v in r['pitch_error_cents']),
      'nonharmonic_energy_db_range':[min(v for r in rows for v in r['nonharmonic_energy_db']),max(v for r in rows for v in r['nonharmonic_energy_db'])]}),flush=True)

if __name__=='__main__':main()
