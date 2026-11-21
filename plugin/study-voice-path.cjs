const fs = require('node:fs');
const path = require('node:path');
const {execFileSync} = require('node:child_process');
const {createHash} = require('node:crypto');
const assert = require('node:assert/strict');

const root = path.resolve(__dirname, '..');
const output = path.join(root,'outputs/voice-path-study-0.25');
const build = path.join(root,'outputs/plugin-build');
const validate = path.join(build,'MemoryLabValidate_artefacts/Release/MemoryLabValidate.exe');
const smoke = path.join(build,'MemoryLabSmoke_artefacts/Release/MemoryLabSmoke.exe');
const bundle = path.join(build,'MemoryLab_artefacts/Release/VST3/Memory Lab Study.vst3');
fs.mkdirSync(output,{recursive:true});
const hash = bytes => createHash('sha256').update(bytes).digest('hex');
const run = (exe,args) => execFileSync(exe,args,{encoding:'utf8',stdio:['ignore','pipe','pipe']});

function capture(name,patch,mode='single') {
    const stem = path.join(output,name);
    fs.writeFileSync(stem+'.log',run(validate,['--capture-patch',bundle,stem,patch,mode]));
    return fs.readFileSync(stem+'.f32');
}
const preserved=[];
for(const version of ['filter-modulation-study-0.23','oscillator-pwm-study-0.24']){
    const folder=path.join(root,'outputs',version);
    const study=JSON.parse(fs.readFileSync(path.join(folder,'measurements.json'),'utf8'));
    for(const {name,sha256} of study.fixtures){
        const stem='preserved-'+version.slice(-4)+'-'+name;
        const mode=name==='combined-filter-routes'?'stress':'single';
        assert.equal(hash(capture(stem,path.join(folder,name+'.mlpatch'),mode)),sha256,stem);
        preserved.push(stem);
    }
}

const bank=path.join(output,'bank');
run(smoke,['--factory-patches',bank]);
const base=fs.readFileSync(path.join(bank,'1.mlpatch'),'utf8');
const controls={drive:0,voiceDrive:0,volume:.065,cutoff:165,resonance:3.4,
    sawLevel:.36,pulseLevel:0,noiseLevel:0,ampDelay:80,ampAttack:1,ampDecay:100,
    ampSustain:.55,ampRelease:45,filterEnv:0};
const fixtures=[];
function render(name,overrides){
    let patch=base;
    for(const [id,value] of Object.entries({...controls,...overrides})){
        const pattern=new RegExp('(<PARAM id="'+id+'" value=")[^"]+');
        assert.match(patch,pattern);
        patch=patch.replace(pattern,(_,open)=>open+value);
    }
    const file=path.join(output,name+'.mlpatch');
    fs.writeFileSync(file,patch);
    run(smoke,['--check-patch-file',file]);
    const raw=capture(name,file);
    const audio=Float32Array.from({length:raw.length/4},(_,i)=>raw.readFloatLE(4*i));
    let peak=0;
    for(const v of audio){assert(Number.isFinite(v),name+' nonfinite');peak=Math.max(peak,Math.abs(v));}
    assert(peak>0&&peak<1,name+' outside fixture headroom');
    assert(audio.slice(0,Math.floor(.13*48000)).every(v=>v===0),name+' amp delay leaked');
    assert(audio.slice(Math.floor(2.25*48000)).every(v=>Math.abs(v)<1e-8),name+' release did not settle');
    const held=audio.slice(48000*.35,48000*1.55);
    const rms=Math.sqrt(held.reduce((sum,v)=>sum+v*v,0)/held.length);
    const stats={name,peak,rms,sha256:hash(raw)};
    fixtures.push(stats);
    return {audio,stats};
}

function wav(name,audio,gain){
    const bytes=Buffer.alloc(44+audio.length*2);
    bytes.write('RIFF',0);bytes.writeUInt32LE(bytes.length-8,4);
    bytes.write('WAVEfmt ',8);bytes.writeUInt32LE(16,16);
    bytes.writeUInt16LE(1,20);bytes.writeUInt16LE(1,22);
    bytes.writeUInt32LE(48000,24);bytes.writeUInt32LE(96000,28);
    bytes.writeUInt16LE(2,32);bytes.writeUInt16LE(16,34);
    bytes.write('data',36);bytes.writeUInt32LE(bytes.length-44,40);
    for(let i=0;i<audio.length;++i)bytes.writeInt16LE(Math.round(audio[i]*gain*32767),44+2*i);
    fs.writeFileSync(path.join(output,name+'.wav'),bytes);
}
for(const source of ['saw','noise'])for(const voiceDrive of [0,16]){
    const name=source+'-'+voiceDrive+'dB';
    const mix=source==='noise'?{sawLevel:0,noiseLevel:.3}:{};
    const legacy=render(name+'-legacy',{...mix,voiceDrive,postFilterAmp:0});
    const post=render(name+'-post',{...mix,voiceDrive,postFilterAmp:1});
    assert.notEqual(legacy.stats.sha256,post.stats.sha256,name+' same path');
    const level=Math.min(legacy.stats.rms,post.stats.rms);
    for(const take of [legacy,post])wav(take.stats.name+'-level-matched',take.audio,level/take.stats.rms);
}
fs.writeFileSync(path.join(output,'measurements.json'),JSON.stringify({preserved,fixtures},null,2)+'\n');
console.log('PASS '+preserved.length+' older captures exact; '+fixtures.length+' VST3 path fixtures and level-matched WAVs');
