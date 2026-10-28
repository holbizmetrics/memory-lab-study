const fs=require('node:fs'),vm=require('node:vm'),assert=require('node:assert/strict');
let Processor;
vm.runInNewContext(fs.readFileSync(__dirname+'/live-drive.js','utf8'),{AudioWorkletProcessor:class{},sampleRate:48000,registerProcessor:(name,p)=>Processor=p});
function render(d,depth,input){const p=new Processor();p.d=d;p.dep=depth;const y=new Float32Array(input.length);p.process([[input]],[[y]],{drive:[d],depth:[depth],recovery:[250]});return y;}
const x=Float32Array.from({length:48000},(_,i)=>.4*Math.sin(i*2*Math.PI*220/48000));
for(const d of [0,1e-8,.1,2,5,12])for(const depth of [0,.8,1.5]){const y=render(d,depth,x);assert(y.every(Number.isFinite));assert(Math.max(...y.map(Math.abs))<1);}
const zero=render(0,.8,x),near=render(1e-8,.8,x);
assert(Math.max(...zero.map((v,i)=>Math.abs(v-near[i])))<1e-6,'zero-drive continuity');
assert(render(5,.8,new Float32Array(512)).every(v=>v===0),'silent startup');
const p=new Processor(),params={drive:[5],depth:[.8],recovery:[250]};
const high=new Float32Array(4096).fill(.7),probe=new Float32Array(128).fill(.1),y=new Float32Array(128);
p.process([[high]],[[new Float32Array(high.length)]],params);p.process([[probe]],[[y]],params);
assert(y.some((v,i)=>Math.abs(v-render(5,.8,probe)[i])>1e-4),'history must affect response');
console.log('PASS: 18 bounded renders, zero-drive continuity, silence and history response');
