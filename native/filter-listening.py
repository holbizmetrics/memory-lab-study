"""Finalize listening artifacts without repeating completed static response tests."""
import hashlib
import json
from pathlib import Path
import subprocess
import numpy as np
from scipy.io import wavfile

ROOT=Path(__file__).resolve().parents[1];OUT=ROOT/'outputs/filter-profiles-1';EXE=ROOT/'outputs/native-build/memorylab_native.exe'

def main():
    prior=ROOT/'outputs/filter-audit-1';contract=json.loads((prior/'contract.json').read_text())
    assert hashlib.sha256((ROOT/'native/voice.hpp').read_bytes()).hexdigest()==contract['code_sha256']['voice.hpp'],'filter implementation changed since static audit'
    OUT.mkdir(parents=True,exist_ok=False)
    record={'static_response_report':str(prior/'report.json'),'static_report_sha256':hashlib.sha256((prior/'report.json').read_bytes()).hexdigest(),
      'voice_source_sha256':contract['code_sha256']['voice.hpp'],'executable_sha256':hashlib.sha256(EXE.read_bytes()).hexdigest(),
      'sweep':'HP pre-roll; 201 target profile values from 0 to 1 in six seconds, 30ms target steps with 20ms smoothing',
      'listening':'UNASSESSED; no browser playback verification'}
    (OUT/'contract.json').write_text(json.dumps(record,indent=2))
    run=subprocess.run([str(EXE),'--profile-test'],capture_output=True,text=True);(OUT/'native-checks.txt').write_text(run.stdout+run.stderr);run.check_returncode()
    clips=[]
    for mode,label in enumerate(['High-pass','Notch','Low-pass baseline','Sweep: HP → notch → LP']):
        file=OUT/f'{mode}-raw.f32';subprocess.run([str(EXE),'--profile-voice',str(file),str(mode)],check=True)
        raw=np.fromfile(file,dtype='<f4');assert len(raw)==326400 and np.isfinite(raw).all()
        gain=10**(-24/20)/np.sqrt(np.mean(raw.astype(float)**2));matched=raw.astype(float)*gain;assert max(abs(matched))<.98
        wavfile.write(OUT/f'{mode}.wav',48000,matched.astype(np.float32))
        sr,decoded=wavfile.read(OUT/f'{mode}.wav');rms=float(20*np.log10(np.sqrt(np.mean(decoded.astype(float)**2))))
        assert sr==48000 and abs(rms+24)<.001
        clips.append({'mode':mode,'label':label,'rms_dbfs':rms,'peak':float(max(abs(decoded))),'gain':float(gain)})
    # Validate the default LP path against the ordinary default-patch phrase.
    sr,default=wavfile.read(ROOT/'outputs/native-phrase.wav')
    lp=np.fromfile(OUT/'2-raw.f32',dtype='<f4');assert sr==48000 and np.array_equal(default,lp),'LP default changed'
    report={'clips':clips,'LP_matches_default_exactly':True,'native_checks':run.stdout.strip(),'listening':'UNASSESSED'}
    (OUT/'report.json').write_text(json.dumps(report,indent=2))
    cards=''.join(f'<p>{c["label"]}</p><audio controls preload="metadata" src="{c["mode"]}.wav"></audio>' for c in clips)
    (OUT/'index.html').write_text('<!doctype html><html lang="en"><meta charset="utf-8"><title>Classic filter profiles</title><style>body{font:18px system-ui;background:#101d19;color:#e6efdf;max-width:850px;margin:40px auto;padding:20px}audio{width:100%}</style><h1>Classic filter profiles</h1><p>Same seven-note phrase, oscillators and approved memory-drive settings. Cutoff 1450 Hz, Q 2.4. Each complete clip matched to −24 dBFS RMS; perceived loudness may differ. Press Play on a clip; no autoplay.</p>'+cards+'<p>The sweep starts at HP, passes through notch around three seconds, and reaches LP around six seconds. It uses the same seven key presses as the static clips. Only profile changes during the sweep. Original classic SVF design, not a Relic circuit reconstruction.</p><script>const players=[...document.querySelectorAll("audio")];players.forEach(p=>p.onplay=()=>players.forEach(q=>{if(q!==p)q.pause();}));</script></html>',encoding='utf-8')
    print(json.dumps(report),flush=True)

if __name__=='__main__':main()
