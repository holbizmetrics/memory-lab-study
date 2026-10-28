"""Attended application of PCLA's ideation contracts and runner to an audio task."""
import argparse
import contextlib
import hashlib
import importlib.util
import json
from pathlib import Path
import shutil
import sys
import time
import uuid

import numpy as np

import memory_dsp as dsp

HERE = Path(__file__).resolve().parent
COMPANION = Path('C:/Users/Holger/Documents/Codex/2026-10-28/expressive-synth-lab/outputs')
PCLA = Path('D:/FromGitHubEtc/PCLA-Experimental')
LOG = Path('D:/FromGitHubEtc/researches/Prometheus.env/knowledge/idea_patterns_2.md')
CLI = Path('C:/Users/Holger/.codex/plugins/.plugin-appserver/codex.exe')


def digest(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def connect(pcla):
    sys.path.insert(0, str(pcla/'tools/ai-bridge'))
    import bridge
    import ideation_stages
    return bridge, ideation_stages


TASK = """Practical, original audio research, not a Relic emulation or circuit inference.
The companion already built a pre/filter-feedback/post drive comparison and a
live static output-tanh slider. Preserve its files. This follow-up compares:
v1 static output saturation; v2 asymmetric biased output saturation; v3 an
envelope-memory/sag output stage, using the SAME snapshot of the companion's
6.8-second mono 48 kHz clean filtered voice and note phrase.
Published manufacturer descriptions do not establish these nonlinear mechanisms.
Your role is to produce testable hypotheses and bounded parameter choices, not code.

Candidate expression MUST be a JSON STRING encoding exactly these fields:
{"kind":"static|biased|sag","bias":0,"depth":0,"attack_ms":5,"release_ms":150}.
v1 kind is static with bias=depth=0 (the fixed baseline).
v2 kind is biased, bias in [0.005,0.08], depth=0.
v3 kind is sag, bias=0, depth in [0.2,1.5].
All attack_ms in [2,30], release_ms in [50,400], finite numbers.
Every candidate is tested at fixed drive factors 2,5,12. No generated code is run.
Static: tanh(d*x)/d. Biased: [tanh(d*(x+b))-tanh(d*b)]/[d*(1-tanh(d*b)^2)],
so the zero-input offset is removed and small-signal slope is one.
Sag: absolute-value envelope follower, scale 0.25, time constants attack/release;
gain=1/(1+depth*envelope); output=tanh(d*gain*x)/d.
All routes use common 4x polyphase oversampling/downsampling and an 8 Hz DC
blocker; whole-clip RMS matching to -24 dBFS happens after rendering. No limiter.
Numerical checks cover finite samples, headroom, zero-drive identity and a history
probe BEFORE the common DC blocker. Musical preference is UNASSESSED until listening.
An analogy is not evidence; phase order matters. Whole-clip RMS is not perceived
loudness, and none of these metrics establishes similarity to hardware.
"""

INSTRUCTIONS = {
    'ideation-image': 'Stage 1: choose a concrete sensory image for expressive drive and map it precisely to the stated DSP. Identify a testable hypothesis and an analogy limitation. No parameter expressions yet.',
    'ideation-variants': 'Stage 2: emit v1/v2/v3 for the three assigned families. v1 is the fixed static reference; v2 changes symmetry, v3 changes memory. expression is the bounded parameter JSON string. State axis, change from seed and testable rationale. Do not pre-filter or claim a listening result.',
    'ideation-calibration': 'Stage 3: assess assumptions, risk, a concrete falsification attempt and predicted test behavior for EACH id. No parameter replacements or deleted variants. Address level matching, memory/latency confounds, DC, transients or aliasing where relevant.',
    'ideation-refinement': 'Stage 4: keep all three ids and assigned families. For each, state a but-what-if question, final bounded parameter JSON string, change and rationale. v1 remains the fixed baseline. Original and refined versions will all be tested; no musical or hardware-match verdict.',
}


def prepare(output, bridge, stages):
    output.mkdir(parents=True, exist_ok=False)
    reference = output/'reference'
    reference.mkdir()
    files = {'clean-raw.wav': COMPANION/'audio/clean-raw.wav', 'companion-settings.json': COMPANION/'settings.json',
             'companion-dsp.mjs': COMPANION/'dsp.mjs', 'inverse_gate_snapshot.py': PCLA/'auto-runtime/inverse_gate_runner.py'}
    sources = {}
    for name, source in files.items():
        shutil.copyfile(source, reference/name)
        sources[name] = {'source': str(source), 'sha256': digest(reference/name)}
    text = LOG.read_text(encoding='utf-8')
    start = text.index('## Extracted Generative Principles: How Ideas Are Born')
    end = text.index('\n## Session:', start)
    recipe = text[start:end]
    (reference/'ideation-recipe.txt').write_text(recipe, encoding='utf-8', newline='\n')
    sources['ideation-recipe.txt'] = {'source': str(LOG), 'sha256': digest(reference/'ideation-recipe.txt')}
    sr, source = dsp.load_source(reference/'clean-raw.wav')
    frozen = {'created_at': bridge.utc(), 'stages': list(stages.ORDER)+['compute'],
              'hypotheses': {'v1':'static', 'v2':'biased', 'v3':'sag'}, 'calls':4, 'timeout_per_call':180,
              'drive_strengths': list(dsp.STRENGTHS), 'matching_dbfs': -24, 'source_frames':len(source),
              'sample_rate':sr, 'source_snapshots':sources,
              'code_sha256': {name:digest(HERE/name) for name in ('memory_dsp.py','pcla_audio.py','memory-player.template.html')},
              'pcla_code_sha256': {name:digest(PCLA/'tools/ai-bridge'/name) for name in ('bridge.py','ideation_stages.py')},
              'cli_sha256':digest(CLI), 'task':TASK,
              'endpoints':['numerical admissibility','memory probe per declared family','retained original and refined renders','manual listening preference'],
              'limits':['one original investigation','not a circuit model or emulation','no automatic musical winner','no 10x claim','no watchers, no automatic knowledge admission']}
    bridge.write_new(output/'contract.json', frozen)
    (output/'contract.sha256').write_text(digest(output/'contract.json'), encoding='ascii')
    return frozen


def check(output, frozen):
    if digest(output/'contract.json') != (output/'contract.sha256').read_text():
        raise ValueError('frozen audio contract changed')
    for name, record in frozen['source_snapshots'].items():
        if digest(output/'reference'/name) != record['sha256']:
            raise ValueError('reference snapshot changed')
    for name, sha in frozen['code_sha256'].items():
        if digest(HERE/name) != sha:
            raise ValueError('audio implementation changed after freeze')
    for name, sha in frozen['pcla_code_sha256'].items():
        if digest(PCLA/'tools/ai-bridge'/name) != sha:
            raise ValueError('PCLA adapter changed after freeze')
    if digest(CLI) != frozen['cli_sha256']:
        raise ValueError('Codex executable changed')


def stage(output, task, history, parents, frozen, bridge, stages):
    check(output, frozen)
    run = output/task
    run.mkdir()
    prompt = ('You are one stage of an attended PCLA research process. No tools, code execution, files, web or other agents. '
              'Return only the requested JSON, at most 1000 words. Historical source examples are not evidence for this task.\n\n'
              +TASK+'\nSOURCE IDEATION PROCESS:\n'+(output/'reference/ideation-recipe.txt').read_text(encoding='utf-8')
              +'\nPRIOR STAGE OUTPUTS:\n'+json.dumps(history)+'\n'+INSTRUCTIONS[task])
    schema = stages.SCHEMAS[task]
    request = {'protocol':bridge.VERSION,'request_id':str(uuid.uuid4()),'created_at':bridge.utc(),
               'task':task,'prompt':prompt,'schema':schema,'prompt_sha256':bridge.digest(prompt.encode()),
               'schema_sha256':bridge.digest(json.dumps(schema,sort_keys=True).encode()),'budgets':stages.BUDGET,
               'snapshots':{},'metadata':{'parents':dict(parents)},'limitations':frozen['limits']}
    bridge.write_new(run/'request.json',request)
    (run/'prompt.txt').write_text(prompt,encoding='utf-8',newline='\n')
    bridge.write_new(run/'schema.json',schema)
    print(task+': started',flush=True)
    bridge.codex(run,CLI,180)
    receipt=bridge.read_json(run/'receipt.json')
    return receipt['response'],digest(run/'receipt.json')


def compile_player(output, entries, bridge):
    sr, source=dsp.load_source(output/'reference/clean-raw.wav')
    p={'kind':'static','bias':0,'depth':0,'attack_ms':5,'release_ms':150}
    _,clean,gain=dsp.render(source,sr,0,p)
    audio=output/'audio';audio.mkdir()
    dsp.write_pcm(audio/'clean.wav',clean,sr)
    rows=[{'id':'clean','route':'clean','drive':0,'file':'audio/clean.wav','seconds':len(clean)/sr,
           'matched_metrics':dsp.metrics(clean,sr)}]
    for entry in entries:
        if entry['candidate']['phase']!='refined' or not entry['ok']:
            continue
        for clip in entry['clips']:
            name=entry['candidate']['variant']+'-'+str(clip['drive'])
            shutil.copyfile(clip['file'],audio/(name+'.wav'))
            rows.append({'id':name,'route':entry['parameters']['kind'],'drive':clip['drive'],
                         'file':'audio/'+name+'.wav','seconds':clip['seconds'],'matched_metrics':clip['matched_metrics']})
    bridge.write_new(output/'manifest.json',rows)
    (output/'index.html').write_text((HERE/'memory-player.template.html').read_text(encoding='utf-8').replace('__MANIFEST__',json.dumps(rows)),encoding='utf-8')
    return rows


def run(output,bridge,stages):
    frozen=prepare(output,bridge,stages)
    history={};parents={};active=None
    try:
        for task in stages.ORDER:
            active=task
            for prior,sha in parents.items():
                if digest(output/prior/'receipt.json')!=sha:raise ValueError('parent stage changed')
            history[task],parents[task]=stage(output,task,history,parents,frozen,bridge,stages)
            print(task+': sealed',flush=True)
        check(output,frozen)
        active='compute'
        candidates=[]
        for phase,key,list_key in [('original','ideation-variants','variants'),('refined','ideation-refinement','refinements')]:
            for item in history[key][list_key]:
                candidates.append({**item,'variant':item['id'],'id':phase+'-'+item['id'],'phase':phase})
        spec=importlib.util.spec_from_file_location('pcla_audio_runner',output/'reference/inverse_gate_snapshot.py')
        module=importlib.util.module_from_spec(spec);spec.loader.exec_module(module)
        def verifier(candidate):
            return dsp.evaluate(candidate,output/'reference/clean-raw.wav',output/'renders'/candidate['id'])
        runner=module.InverseGateRunner(name='drive-memory-audio-1',pre_registration=json.dumps(frozen),
                verifier=verifier,proposer=iter(candidates),out_jsonl=output/'engine-results.jsonl',time_budget_s=30)
        # The renderer and hypothesis-specific controls run for every candidate;
        # numerical success is not musical quality or a Relic-match score.
        with (output/'engine-console.txt').open('x',encoding='utf-8') as stream,contextlib.redirect_stdout(stream):
            verdict=runner.run()
        if len(runner.verifier_results)!=6:raise ValueError('lost a candidate')
        rows=compile_player(output,runner.verifier_results,bridge)
        bridge.write_new(output/'result.json',{'status':'COMPLETE','engine':verdict,'entries':runner.verifier_results,
                        'player_clips':len(rows),'musical_verdict':'UNASSESSED','limits':frozen['limits']})
        print(json.dumps({'status':'COMPLETE','clips':len(rows),'player':str(output/'index.html')}),flush=True)
    except Exception as error:
        bridge.write_new(output/'failure.json',{'stage':active,'error':str(error),'completed_stages':list(history)})
        raise


if __name__=='__main__':
    parser=argparse.ArgumentParser(description=__doc__);parser.add_argument('output',type=Path)
    args=parser.parse_args();bridge,stages=connect(PCLA);run(args.output.resolve(),bridge,stages)
