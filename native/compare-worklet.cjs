const fs=require('node:fs'),vm=require('node:vm'),cp=require('node:child_process'),path=require('node:path'),assert=require('node:assert/strict');
const root=path.resolve(__dirname,'..'),fixture=path.join(root,'outputs/native-drive.f32');
cp.execFileSync(path.join(root,'outputs/native-build/memorylab_native.exe'),['--drive-fixture',fixture]);
let Processor;vm.runInNewContext(fs.readFileSync(path.join(root,'live-drive.js'),'utf8'),{sampleRate:48000,AudioWorkletProcessor:class{},registerProcessor:(_,p)=>Processor=p});
const native=fs.readFileSync(fixture),processor=new Processor(),input=new Float32Array(1),output=new Float32Array(1);
const p={drive:[5],depth:[.8],recovery:[250]};let error=0,power=0;
for(let i=0;i<48000;i++){
 if(i===16000)p.drive[0]=0;if(i===32000){p.drive[0]=12;p.depth[0]=1.5;p.recovery[0]=50;}
 input[0]=.4*Math.sin(i*2*Math.PI*220/48000);processor.process([[input]],[[output]],p);
 const difference=output[0]-native.readFloatLE(i*4);error=Math.max(error,Math.abs(difference));power+=difference*difference;
}
assert(error<1e-7,`worklet regression: ${error}`);
const report={frames:48000,sample_rate:48000,max_absolute_error:error,rms_error:Math.sqrt(power/48000),tolerance:1e-7,
 scope:'Actual browser worklet algorithm vs native memory stage; steady input and drive/depth/recovery changes. Voice/filter equivalence and perceived sound NOT established.'};
fs.writeFileSync(path.join(root,'outputs/native-worklet-comparison.json'),JSON.stringify(report,null,2));console.log(JSON.stringify(report));
