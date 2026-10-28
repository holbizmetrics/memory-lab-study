const $=id=>document.getElementById(id);
let ctx,drive,out,initializing,access,timers=[],voices=new Map(),held=new Map();
const message=t=>$('status').textContent=t;
async function init(){
 if(!initializing)initializing=(async()=>{
  ctx=new AudioContext();await ctx.audioWorklet.addModule('live-drive.js');
  drive=new AudioWorkletNode(ctx,'memory-drive',{numberOfInputs:1,numberOfOutputs:1,outputChannelCount:[1]});
  out=ctx.createGain();out.gain.value=Number($('volume').value);drive.connect(out).connect(ctx.destination);
  controls();
 })().catch(e=>{initializing=null;throw e;});
 await initializing;await ctx.resume();message('Audio ready — play a note');
}
function controls(){if(!ctx||!drive)return;for(const name of ['drive','depth','recovery'])drive.parameters.get(name).value=Number($(name).value);out.gain.setTargetAtTime(Number($('volume').value),ctx.currentTime,.02);}
for(const id of ['drive','depth','recovery','volume'])$(id).oninput=()=>{$(id).nextElementSibling.value=$(id).value;controls();};
function on(note,velocity=100){
 if(!ctx||ctx.state!=='running')return;
 if(voices.has(note))off(note);
 if(voices.size>=8){message('Eight-note limit — release a note');return;}
 const t=ctx.currentTime,f=440*2**((note-69)/12),amp=ctx.createGain(),filter=ctx.createBiquadFilter();
 filter.type='lowpass';filter.frequency.value=1450;filter.Q.value=2.4;
 amp.gain.setValueAtTime(0,t);amp.gain.linearRampToValueAtTime(velocity/127*.32,t+.012);amp.gain.linearRampToValueAtTime(velocity/127*.32*.65,t+.192);
 const a=ctx.createOscillator(),b=ctx.createOscillator();a.type='sawtooth';b.type='square';a.frequency.value=f;b.frequency.value=f;b.detune.value=7;
 const ga=ctx.createGain(),gb=ctx.createGain();ga.gain.value=.42;gb.gain.value=.28;
 a.connect(ga).connect(amp);b.connect(gb).connect(amp);amp.connect(filter).connect(drive);a.start();b.start();
 const v={a,b,amp,filter,ga,gb};voices.set(note,v);
 message(`Playing MIDI ${note} · velocity ${velocity}`);
}
function off(note){const v=voices.get(note);if(!v)return;voices.delete(note);const t=ctx.currentTime;
 v.amp.gain.cancelAndHoldAtTime(t);v.amp.gain.linearRampToValueAtTime(0,t+.5);v.a.stop(t+.51);v.b.stop(t+.51);
 v.a.onended=()=>{for(const n of Object.values(v))n.disconnect();};}
function stop(){timers.forEach(clearTimeout);timers=[];for(const n of [...voices.keys()])off(n);held.clear();message('Stopped — release tails fade out');}
function safe(fn){return (...args)=>Promise.resolve().then(()=>fn(...args)).catch(e=>message(e.message));}
$('start').onclick=safe(init);$('stop').onclick=stop;
$('demo').onclick=safe(async()=>{await init();stop();for(const [s,n,d,v] of [[0,48,.32,.45],[.52,55,.3,.65],[1.04,60,.42,.9],[1.65,63,.25,.55],[2.08,67,.65,1],[3,60,1.2,.82],[4.5,48,1.2,1]]){timers.push(setTimeout(()=>on(n,Math.round(v*127)),s*1000),setTimeout(()=>off(n),(s+d)*1000));}});
$('preset').onclick=()=>{for(const [k,v] of Object.entries({drive:5,depth:.8,recovery:250})){$(k).value=v;$(k).nextElementSibling.value=v;}controls();};
const codes=['KeyA','KeyW','KeyS','KeyE','KeyD','KeyF','KeyT','KeyG','KeyY','KeyH','KeyU','KeyJ','KeyK'];
for(let i=0;i<13;i++){const button=document.createElement('button');button.textContent=60+i;button.setAttribute('aria-label',`Play MIDI note ${60+i}`);button.onpointerdown=safe(async e=>{button.setPointerCapture(e.pointerId);await init();if(button.hasPointerCapture(e.pointerId))on(60+i);});button.onpointerup=button.onpointercancel=button.onlostpointercapture=()=>off(60+i);$('keys').append(button);}
window.addEventListener('keydown',e=>{if(e.repeat||['INPUT','SELECT','TEXTAREA'].includes(e.target.tagName))return;const i=codes.indexOf(e.code);if(i>=0){e.preventDefault();held.set(e.code,60+i);on(60+i);}});
window.addEventListener('keyup',e=>{if(held.has(e.code)){off(held.get(e.code));held.delete(e.code);}});window.addEventListener('blur',stop);document.addEventListener('visibilitychange',()=>{if(document.hidden)stop();});
function selectPort(){stop();for(const input of access.inputs.values())input.onmidimessage=null;const input=access.inputs.get($('ports').value);if(input)input.onmidimessage=event=>{const [s,n,v]=event.data,kind=s&240;if(kind===144&&v)on(n,v);else if(kind===128||kind===144)off(n);else if(kind===176&&(n===120||n===123))stop();};}
function ports(){const old=$('ports').value;$('ports').replaceChildren(new Option('Choose MIDI input',''));for(const input of access.inputs.values())$('ports').add(new Option(input.name||input.id,input.id));$('ports').value=old;selectPort();message(`${access.inputs.size} MIDI input(s) available — select one`);}
$('ports').onchange=selectPort;$('midi').onclick=safe(async()=>{await init();if(!navigator.requestMIDIAccess)throw Error('Web MIDI unavailable; use Chrome on localhost or the on-screen keys.');access=await navigator.requestMIDIAccess({sysex:false});access.onstatechange=ports;ports();});
