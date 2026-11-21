const fs = require('node:fs');
const path = require('node:path');
const {execFileSync} = require('node:child_process');
const {createHash} = require('node:crypto');
const assert = require('node:assert/strict');

const root = path.resolve(__dirname, '..');
const previous = path.join(root, 'outputs/filter-modulation-study-0.23');
const output = path.join(root, 'outputs/oscillator-pwm-study-0.24');
const build = path.join(root, 'outputs/plugin-build');
const validator = path.join(build, 'MemoryLabValidate_artefacts/Release/MemoryLabValidate.exe');
const smoke = path.join(build, 'MemoryLabSmoke_artefacts/Release/MemoryLabSmoke.exe');
const bundle = path.join(build, 'MemoryLab_artefacts/Release/VST3/Memory Lab Study.vst3');
fs.mkdirSync(output, {recursive:true});

const digest = bytes => createHash('sha256').update(bytes).digest('hex');
const run = (exe,args) => execFileSync(exe,args,{encoding:'utf8',stdio:['ignore','pipe','pipe']});
function capture(name, patch, mode='single') {
    const stem = path.join(output,name);
    const log = run(validator,['--capture-patch',bundle,stem,patch,mode]);
    fs.writeFileSync(stem+'.log',log);
    return fs.readFileSync(stem+'.f32');
}
const oldStudy = JSON.parse(fs.readFileSync(path.join(previous,'measurements.json'),'utf8'));
for (const fixture of oldStudy.fixtures) {
    const mode = fixture.name === 'combined-filter-routes' ? 'stress' : 'single';
    const raw = capture(fixture.name,path.join(previous,fixture.name+'.mlpatch'),mode);
    assert.equal(digest(raw),fixture.sha256,fixture.name+' changed after migration');
}
const bank = path.join(output,'bank');
run(smoke,['--factory-patches',bank]);
const template = fs.readFileSync(path.join(bank,'1.mlpatch'),'utf8');
const common = {drive:0,voiceDrive:0,volume:0.08,cutoff:2500,resonance:0.9,
    sawLevel:0,pulseLevel:0,osc1PulseLevel:0.5,osc1Width:0.5,
    lfoRate:2,lfo2Rate:1.5,lfo3Rate:1.25,lfo4Rate:0.9};
function record(name, settings) {
    let patch = template;
    for (const [id,value] of Object.entries({...common,...settings})) {
        const field = new RegExp('(<PARAM id="'+id+'" value=")[^"]+');
        assert.match(patch,field);
        patch = patch.replace(field,(_,open)=>open+value);
    }
    const file = path.join(output,name+'.mlpatch');
    fs.writeFileSync(file,patch);
    run(smoke,['--check-patch-file',file]);
    const raw = capture(name,file);
    let peak=0;
    for(let i=0;i<raw.length;i+=4){
        const value=raw.readFloatLE(i);
        assert(Number.isFinite(value),name+' nonfinite sample');
        peak=Math.max(peak,Math.abs(value));
    }
    assert(peak>0&&peak<1,name+' peak outside fixture range');
    return {name,peak,sha256:digest(raw)};
}
const base = record('osc1-static',{});
const osc2 = record('osc2-static',{osc1PulseLevel:0,pulseLevel:0.5});
const fixtures = [base,osc2];
for (const source of ['Lfo1','Lfo2','Lfo3','Lfo4','Env3','Env4']) {
    const patch = {[('osc1Pwm'+source)]:0.26};
    const active = record('osc1-'+source,patch);
    assert.notEqual(active.sha256,base.sha256,source+' had no effect');
    const other = record('osc2-'+source,{...patch,osc1PulseLevel:0,pulseLevel:0.5});
    assert.equal(other.sha256,osc2.sha256,source+' changed OSC 2');
    fixtures.push(active,other);
}
fs.writeFileSync(path.join(output,'measurements.json'),JSON.stringify({
    preserved:oldStudy.fixtures.map(f=>f.name),fixtures
},null,2)+'\n');
console.log('PASS '+oldStudy.fixtures.length+' 0.23 captures exact; '+fixtures.length+' new actual VST3 fixtures');
