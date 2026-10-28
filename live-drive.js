/* Original experimental memory drive. Four substeps; not an alias-free claim. */
class MemoryDrive extends AudioWorkletProcessor {
  static get parameterDescriptors() {return [
    {name:'drive',defaultValue:5,minValue:0,maxValue:12,automationRate:'k-rate'},
    {name:'depth',defaultValue:.8,minValue:0,maxValue:1.5,automationRate:'k-rate'},
    {name:'recovery',defaultValue:250,minValue:50,maxValue:400,automationRate:'k-rate'}];}
  constructor(){super();this.env=0;this.previous=0;this.dcX=0;this.dcY=0;this.d=5;this.dep=.8;this.rel=250;}
  process(inputs,outputs,p){
    const x=inputs[0]?.[0],y=outputs[0][0];
    const smooth=1-Math.exp(-1/(sampleRate*.02));
    const dc=Math.exp(-2*Math.PI*8/sampleRate);
    for(let i=0;i<y.length;i++){
      this.d+=(p.drive[0]-this.d)*smooth;this.dep+=(p.depth[0]-this.dep)*smooth;this.rel+=(p.recovery[0]-this.rel)*smooth;
      const input=x?x[i]:0;let out=0;
      for(let k=1;k<=4;k++){
        const u=this.previous+(input-this.previous)*k/4;
        const level=Math.abs(u)/.25;
        const c=Math.exp(-1/(sampleRate*4*(level>this.env?.005:this.rel*.001)));
        this.env=c*this.env+(1-c)*level;
        // Continuous at zero, identical depth to frozen model for drive >= 2.
        const t=Math.min(1,this.d/2),fade=t*t*(3-2*t);
        const gain=1/(1+this.dep*fade*this.env),v=gain*u;
        out+=this.d<1e-7?v:Math.tanh(this.d*v)/this.d;
      }
      this.previous=input;out/=4;
      const blocked=out-this.dcX+dc*this.dcY;this.dcX=out;this.dcY=blocked;
      y[i]=Number.isFinite(blocked)?blocked:0;
    }return true;
  }
}
registerProcessor('memory-drive',MemoryDrive);
