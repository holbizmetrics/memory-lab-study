const fs=require('node:fs');
const path=require('node:path');
const {execFileSync}=require('node:child_process');
const {createHash}=require('node:crypto');
const assert=require('node:assert/strict');

const root=path.resolve(__dirname,'..');
const build=path.join(root,'outputs/plugin-build');
const output=path.join(root,'outputs/stereo-spread-study-0.26');
const validate=path.join(build,'MemoryLabValidate_artefacts/Release/MemoryLabValidate.exe');
const smoke=path.join(build,'MemoryLabSmoke_artefacts/Release/MemoryLabSmoke.exe');
const bundle=path.join(build,'MemoryLab_artefacts/Release/VST3/Memory Lab Study.vst3');
fs.mkdirSync(output,{recursive:true});
const digest=bytes=>createHash('sha256').update(bytes).digest('hex');
const run=(exe,args)=>execFileSync(exe,args,{encoding:'utf8',stdio:['ignore','pipe','pipe']});

function capture(name,patch,mode){
    const stem=path.join(output,name);
    fs.writeFileSync(stem+'.log',run(validate,['--capture-stereo-patch',bundle,stem,patch,mode]));
    return ['.f32','.right.f32'].map(suffix=>fs.readFileSync(stem+suffix));
}
const preserved=[];
for(const version of ['filter-modulation-study-0.23','oscillator-pwm-study-0.24','voice-path-study-0.25']){
    const folder=path.join(root,'outputs',version);
    const study=JSON.parse(fs.readFileSync(path.join(folder,'measurements.json'),'utf8'));
    for(const {name,sha256} of study.fixtures){
        const stem='preserved-'+version.slice(-4)+'-'+name;
        const [left,right]=capture(stem,path.join(folder,name+'.mlpatch'),name==='combined-filter-routes'?'stress':'single');
        assert.equal(digest(left),sha256,stem);
        assert(left.equals(right),stem+' is no longer centered');
        preserved.push(stem);
    }
}

const bank=path.join(output,'bank');
run(smoke,['--factory-patches',bank]);
const template=fs.readFileSync(path.join(bank,'1.mlpatch'),'utf8');
function render(name,settings){
    let patch=template;
    const controls={stereoSpread:0,drive:0,voiceDrive:0,volume:.07,cutoff:3000,
        sawLevel:.4,pulseLevel:0,ampAttack:20,ampSustain:.75,...settings};
    for(const [id,value] of Object.entries(controls)){
        const pattern=new RegExp('(<PARAM id="'+id+'" value=")[^"]+');
        assert.match(patch,pattern);
        patch=patch.replace(pattern,(_,open)=>open+value);
    }
    const file=path.join(output,name+'.mlpatch');
    fs.writeFileSync(file,patch);
    run(smoke,['--check-patch-file',file]);
    const [left,right]=capture(name,file,'default');
    let mid=0,side=0,peak=0;
    for(let i=0;i<left.length;i+=4){
        const l=left.readFloatLE(i),r=right.readFloatLE(i);
        assert(Number.isFinite(l)&&Number.isFinite(r));
        peak=Math.max(peak,Math.abs(l),Math.abs(r));
        mid+=((l+r)/2)**2;
        side+=((l-r)/2)**2;
    }
    assert(peak>0&&peak<1,name+' fixture headroom');
    const stats={name,peak,sideToMid:Math.sqrt(side/mid),left: digest(left),right:digest(right)};
    return {left,right,stats};
}
const centred=render('chord-centred',{});
const wide=render('chord-wide',{stereoSpread:1});
const driven=render('chord-wide-drive',{stereoSpread:1,drive:5});
assert.equal(centred.stats.sideToMid,0);
assert(wide.stats.sideToMid>.1&&driven.stats.sideToMid>.1);
for(let i=0;i<centred.left.length;i+=4){
    const fold=(wide.left.readFloatLE(i)+wide.right.readFloatLE(i))*.5;
    assert(Math.abs(centred.left.readFloatLE(i)-fold)<1e-7,'clean stereo fold differs from centre');
}
const fixtures=[centred.stats,wide.stats,driven.stats];
fs.writeFileSync(path.join(output,'measurements.json'),JSON.stringify({preserved,fixtures},null,2)+'\n');
console.log('PASS '+preserved.length+' older VST3 captures exact in both channels; '+fixtures.length+' stereo fixtures and clean mono fold');
