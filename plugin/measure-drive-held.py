"""Independent held-window RMS/crest check of actual-plugin headroom captures."""
import json, math, pathlib, struct
ROOT=pathlib.Path(__file__).resolve().parents[1]
folder=ROOT/'outputs/lp4-headroom-study-2'
rows=[]
for q in ('3p72','8'):
    for noise in (0,1):
        for variant in ('classic','lp4-original','lp4-compensated'):
            measurements=[]
            for mode in ('clean','default'):
                path=folder/f'q{q}-noise{noise}-{variant}-{mode}.f32'
                values=[v[0] for v in struct.iter_unpack('<f',path.read_bytes())][14400:72000]
                assert len(values)==57600 and all(math.isfinite(v) for v in values)
                rms=math.sqrt(sum(v*v for v in values)/len(values)); peak=max(map(abs,values))
                measurements.append(dict(mode=mode,rms=rms,peak=peak,crest_db=20*math.log10(peak/rms)))
            off,on=measurements
            row=dict(q=q,noise=noise,variant=variant,window_seconds=[.3,1.5],
                     combined_signal_rms_drop_db=20*math.log10(off['rms']/on['rms']),off=off,on=on)
            rows.append(row)
            print(f"Q{q} noise{noise} {variant}: combined RMS drop {row['combined_signal_rms_drop_db']:.2f} dB, crest {off['crest_db']:.2f}->{on['crest_db']:.2f} dB")
(folder/'held-window-independent.json').write_text(json.dumps(rows,indent=2),encoding='utf-8')
