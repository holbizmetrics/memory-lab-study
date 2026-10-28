"""Bounded modulation slice: independent disabled-baseline check and listening."""
import hashlib
import json
from pathlib import Path
import subprocess
import numpy as np
from scipy.io import wavfile

ROOT=Path(__file__).resolve().parents[1];OUT=ROOT/'outputs/modulation-audit-1';EXE=ROOT/'outputs/native-build/memorylab_native.exe'

def main():
    OUT.mkdir(parents=True,exist_ok=False)
    labels=['Unmodulated baseline','Filter envelope','Pitch LFO (vibrato)','Pulse-width LFO','Filter-cutoff LFO','Combined modulation']
    contract={'rates':[44100,48000,96000],'modes':labels,'lfo':'sine, per voice, note-retriggered phase 0, rate4.5Hz',
      'routes':{'pitch':'8 cents peak','pulse_width':'.15 peak width units','cutoff':'.5 octaves peak','filter_envelope':'1.5 octaves, velocity scaled'},
      'filter_envelope':'attack20ms, decay350ms, sustain.15, release400ms, linear contours',
      'depth_smoothing':'20ms; explicit 100ms pre-roll trimmed from listening clips',
      'acceptance':'finite/headroom; partition invariance; envelope shape/release within1e-9; disabled baseline bit-identical; each enabled route differs from baseline RMS>1e-4',
      'scope':'one additional envelope and one sine LFO, not full four-envelope/four-LFO coverage or hardware equivalence',
      'code_sha256':{name:hashlib.sha256((ROOT/'native'/name).read_bytes()).hexdigest() for name in ['voice.hpp','main.cpp','modulation-audit.py']},
      'exe_sha256':hashlib.sha256(EXE.read_bytes()).hexdigest()}
    (OUT/'contract.json').write_text(json.dumps(contract,indent=2))
    run=subprocess.run([str(EXE),'--mod-test'],capture_output=True,text=True);(OUT/'native-checks.txt').write_text(run.stdout+run.stderr);run.check_returncode()
    clips=[];baseline=None
    for mode,label in enumerate(labels):
        file=OUT/f'{mode}-raw.f32';subprocess.run([str(EXE),'--mod-voice',str(file),str(mode)],check=True)
        raw=np.fromfile(file,dtype='<f4');assert len(raw)==326400 and np.isfinite(raw).all() and max(abs(raw))<1
        if mode==0:
            baseline=raw.copy();approved=np.fromfile(ROOT/'outputs/filter-profiles-1/2-raw.f32',dtype='<f4')
            assert np.array_equal(raw,approved),'disabled modulation changed approved baseline'
        difference=float(np.sqrt(np.mean((raw.astype(float)-baseline.astype(float))**2)))
        if mode:assert difference>1e-4,(label,'route had negligible effect')
        gain=10**(-24/20)/np.sqrt(np.mean(raw.astype(float)**2));matched=raw.astype(float)*gain;assert max(abs(matched))<.98
        wavfile.write(OUT/f'{mode}.wav',48000,matched.astype(np.float32));sr,decoded=wavfile.read(OUT/f'{mode}.wav')
        rms=float(20*np.log10(np.sqrt(np.mean(decoded.astype(float)**2))));assert sr==48000 and abs(rms+24)<.001
        clips.append({'mode':mode,'label':label,'rms_dbfs':rms,'peak':float(max(abs(decoded))),'rms_difference_from_raw_baseline':difference})
    report={'clips':clips,'disabled_baseline_exact':True,'native_checks':run.stdout.strip(),'listening':'UNASSESSED'}
    (OUT/'report.json').write_text(json.dumps(report,indent=2))
    descriptions=['Original approved LP voice, with all modulation depths zero.',
      'Each key press opens the filter, then it settles. Separate linear filter envelope, scaled by note velocity.',
      '4.5 Hz sine LFO, ±8 cents on both oscillators; each note starts its own LFO.',
      '4.5 Hz sine LFO, ±0.15 pulse width around 0.43.',
      '4.5 Hz sine LFO, ±0.5 octaves around the filter cutoff.',
      'All four routes at the same individual depths; this is an interaction test, not an automatic winner.']
    cards=''.join(f'<section><h2>{c["label"]}</h2><p>{descriptions[c["mode"]]}</p><audio controls preload="metadata" src="{c["mode"]}.wav"></audio></section>' for c in clips)
    (OUT/'index.html').write_text('<!doctype html><html lang="en"><meta charset="utf-8"><meta name="viewport" content="width=device-width"><title>Native modulation study</title><style>body{font:17px system-ui;background:#101d19;color:#e6efdf;max-width:850px;margin:30px auto;padding:20px}section{background:#1c3026;padding:18px;margin:18px 0;border-radius:8px}h2{font-size:21px}audio{width:100%}</style><h1>Native modulation study</h1><p>Same seven key presses and velocity pattern in every clip. These are note-retriggered per-voice effects, not additional key presses. Original voice with the approved oscillator, LP filter and memory-drive defaults.</p><p>Each 6.8-second clip is globally matched to −24 dBFS RMS. Dynamics within notes remain; perceived loudness can differ. Press Play on a clip; nothing autoplays.</p>'+cards+'<p>This is an original partial modulation system, not a complete Relic emulation or its four-envelope/four-LFO architecture.</p><script>const players=[...document.querySelectorAll("audio")];players.forEach(p=>p.onplay=()=>players.forEach(q=>{if(q!==p)q.pause();}));</script></html>',encoding='utf-8')
    print(json.dumps(report),flush=True)

if __name__=='__main__':main()
