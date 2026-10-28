"""Bounded, attended PCLA ideation for the next complete-voice milestone."""
import hashlib
import json
from pathlib import Path
import sys
import uuid

HERE=Path(__file__).resolve().parent
PCLA=Path('D:/FromGitHubEtc/PCLA-Experimental/tools/ai-bridge')
sys.path.insert(0,str(PCLA))
import bridge
import ideation_stages as stages

CONTEXT='''GOAL: Work toward a fuller original Relic-inspired playable instrument,
not merely package the output drive as VST3. No circuit or hardware-match claim.
OFFICIAL FACTS retrieved 2026-10-28:
https://www.shearelectronics.com/relic/specs/ describes eight voice cores, each
two SEM-topology VCOs, seven VCAs, four dAHDSR envelopes, four eight-shape LFOs,
and filter controls cutoff, resonance, bandwidth, profile. CLASSIC profile
HP2/notch/LP2; MODERN BP4/XF4/LP2/LP3/LP4. Four-part multitimbrality is listed.
https://www.shearelectronics.com/relic/voice-core/ describes an SVF plus an extra
dipole and gain cells. Exact topology, component values, transfer measurements,
distortion mechanism and calibration algorithms are NOT established by this text.
CURRENT CODE: separate DrivePlacementLab-Codex. Browser prototype two native
saw/square oscillators, seven-cent detune, lowpass1450 Q2.4, velocity ADSR,
eight held notes, output memory drive (depth .8, attack5ms, recovery250ms,
drive5), keyboard/MIDI. Source phrase snapshot and frozen 10 comparison takes.
USER FEEDBACK: envelope-memory medium musical, usable in productions; live audio,
MIDI and velocity confirmed. Not a hardware similarity judgment or blind test.
PCLA already found zero-drive discontinuity, corrected only in live stage by
depth smoothstep below drive2. Offline frozen references remain unchanged.
COMPANION owns expressive-synth-lab and requested we own standalone native DSP/
VST3 foundation. Do not edit companion, FPSDK, or PCLA engine. No launchers,
watchers, installers, publication or generated code execution in this research.
RESEARCH QUESTION: Which three bounded next experiments most efficiently turn
this into a broader expressive voice while preserving the approved drive sound?
Consider oscillators, profile-filter architecture, modulation/performance/voices.
Do not presume random drift is analog accuracy, or blend independent Q/bandwidth
controls with an unsupported claim of reproducing the real supervariable filter.
Separate documented facts, original design choices, unknowns and required audio
evidence. Give implementation-independent falsifiers and suitable numeric/audio
checks. No invented listening results, performance gains or completed work.
Each expression is JSON text with fields module, experiment, falsifier (short
strings; <=400 total characters); this is an experiment plan, NOT executable code.
Keep all v1/v2/v3 through calibration and refinement. Their scopes may be adjusted
but not erased. Recommend a practical sequencing rationale in rationale fields.
'''

def main():
 out=HERE/'outputs/voice-research-1';out.mkdir(parents=True,exist_ok=False)
 recipe=(HERE/'outputs/ideation-1/reference/ideation-recipe.txt').read_text(encoding='utf-8')
 contract={'context':CONTEXT,'model_calls':4,'timeout_s':180,'stage5':'local plan integrity audit only; DSP/listening pending',
           'bridge_sha256':hashlib.sha256((PCLA/'bridge.py').read_bytes()).hexdigest(),
           'stages_sha256':hashlib.sha256((PCLA/'ideation_stages.py').read_bytes()).hexdigest()}
 bridge.write_new(out/'contract.json',contract)
 history={}
 prompts=['Map one concrete sensory image to a full expressive voice. State limits.',
          'Propose three complementary bounded experiments, v1/v2/v3, using required JSON expression fields.',
          'Critique each proposed experiment: unknowns, confounds, falsifiers, and predicted test outcomes. Do not report tests as completed.',
          'Refine all experiments, with required JSON expression fields. Propose implementation order and preservation checks in rationale.']
 for task,instruction in zip(stages.ORDER,prompts):
  run=out/task;run.mkdir()
  prompt='Attended PCLA stage. No tools, files, web, code execution or other agents. Return requested JSON only, <=1000 words.\n'+CONTEXT+'\nOBSERVED IDEATION PROCESS:\n'+recipe+'\nPRIOR STAGES:\n'+json.dumps(history)+'\n'+instruction
  schema=stages.SCHEMAS[task]
  request={'protocol':bridge.VERSION,'request_id':str(uuid.uuid4()),'created_at':bridge.utc(),'task':task,'prompt':prompt,'schema':schema,
           'prompt_sha256':bridge.digest(prompt.encode()),'schema_sha256':bridge.digest(json.dumps(schema,sort_keys=True).encode()),
           'budgets':stages.BUDGET,'snapshots':{},'metadata':{},'limitations':['proposal only','no measured emulation','no tools']}
  bridge.write_new(run/'request.json',request);bridge.write_new(run/'schema.json',schema)
  (run/'prompt.txt').write_text(prompt,encoding='utf-8')
  print(task+': started',flush=True)
  bridge.codex(run,Path('C:/Users/Holger/.codex/plugins/.plugin-appserver/codex.exe'),180)
  history[task]=bridge.read_json(run/'receipt.json')['response']
  print(task+': sealed',flush=True)
 checks=[]
 for stage,key in [('ideation-variants','variants'),('ideation-refinement','refinements')]:
  for row in history[stage][key]:
   try:
    plan=json.loads(row['expression']);ok=isinstance(plan,dict) and set(plan)=={'module','experiment','falsifier'} and all(isinstance(v,str) and v.strip() for v in plan.values())
   except (ValueError,TypeError):ok=False
   checks.append({'stage':stage,'id':row['id'],'valid_plan_fields':ok})
 bridge.write_new(out/'plan-audit.json',{'checks':checks,'all_valid':all(c['valid_plan_fields'] for c in checks),'DSP_tests':'NOT_RUN','listening':'NOT_RUN'})
 bridge.write_new(out/'stages.json',history)
 print(json.dumps({'output':str(out),'plan_checks':checks}),flush=True)

if __name__=='__main__':main()
