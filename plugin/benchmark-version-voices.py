"""Serial, alternating-order offline processBlock timings; not a realtime xrun test."""
import hashlib, json, pathlib, re, statistics, subprocess
ROOT=pathlib.Path(__file__).resolve().parents[1]
out=ROOT/'outputs/version-voice-timing-1'; out.mkdir(parents=True,exist_ok=False)
host=ROOT/'outputs/plugin-build/MemoryLabValidate_artefacts/Release/MemoryLabValidate.exe'
rows=[]
for repeat in range(6):
    for mode,voices in (('single',1),('stress',8)):
        for version in (('0.7','0.8') if repeat%2==0 else ('0.8','0.7')):
            bundle=ROOT/f'outputs/distortion-audit-0.8-1/{version}/Memory Lab Study.vst3'
            patch=ROOT/f'outputs/user-version-comparison-1/v{version.replace(".","")}.mlpatch'
            stem=out/f'r{repeat}-{mode}-v{version.replace(".","")}'
            result=subprocess.run([str(host),'--capture-patch',str(bundle),str(stem),str(patch),mode],capture_output=True,text=True,timeout=40)
            pathlib.Path(str(stem)+'.log').write_text(result.stdout+result.stderr,encoding='utf-8')
            if result.returncode: raise RuntimeError(result.stdout+result.stderr)
            def metric(name): return float(re.search(r'\b'+name+r'=([0-9.eE+-]+)',result.stdout).group(1))
            rows.append(dict(repeat=repeat,version=version,voices=voices,median_us=metric('held_median_us'),p95_us=metric('held_p95_us'),
                             held_blocks=int(metric('held_blocks')),worst_block_budget_ratio=metric('worst_block_budget_ratio'),
                             raw_sha256=hashlib.sha256(pathlib.Path(str(stem)+'.f32').read_bytes()).hexdigest()))
summary=[]
for voices in (1,8):
    old=[r for r in rows if r['voices']==voices and r['version']=='0.7']
    new=[r for r in rows if r['voices']==voices and r['version']=='0.8']
    assert len({r['raw_sha256'] for r in old+new})==1, 'Audio differs between versions/runs'
    a=statistics.median(r['median_us'] for r in old); b=statistics.median(r['median_us'] for r in new)
    summary.append(dict(voices=voices,median_of_block_medians_us_v07=a,median_of_block_medians_us_v08=b,ratio_v08_to_v07=b/a,
                        v07_run_range_us=[min(r['median_us'] for r in old),max(r['median_us'] for r in old)],
                        v08_run_range_us=[min(r['median_us'] for r in new),max(r['median_us'] for r in new)],
                        worst_observed_block_budget_ratio=max(r['worst_block_budget_ratio'] for r in old+new),audio_identical=True))
report=dict(summary=summary,runs=rows,conditions='48kHz/512 frames, fixed saved Classic/noise0 patch, shared drive5/depth.8; six fresh-instance repeats each, serial/alternating order, 131 held-note blocks per run. Eight is the maximum voice count. No realtime device, xrun counter, host project or small-buffer claim. Timing differences are descriptive, not a causal verdict.')
(out/'measurements.json').write_text(json.dumps(report,indent=2),encoding='utf-8')
print(json.dumps(summary,indent=2))
