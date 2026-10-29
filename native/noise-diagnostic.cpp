// Reuse the frozen listening fixture and WAV writer, but compile the production
// path (no MEMORYLAB_EXPERIMENTAL_NOISE), rather than the fixed study hook.
#include <string>
#define main noiseStudyMain
#include "noise-study.cpp"
#undef main

std::vector<float> readWav(const fs::path&p){std::ifstream f(p,std::ios::binary);require(bool(f),"reference WAV missing");f.seekg(0,std::ios::end);require(f.tellg()==std::streampos(44+144000*4),"reference WAV size");f.seekg(44);std::vector<float>a(144000);f.read(reinterpret_cast<char*>(a.data()),a.size()*4);require(bool(f),"reference read");return a;}
int main(int argc,char**argv){try{
 require(argc==3,"Expected original study folder and a new diagnostic folder");const fs::path reference(argv[1]),folder(argv[2]);require(fs::create_directory(folder),"diagnostic folder exists or parent missing");
 std::ofstream report(folder/"comparison.csv");report<<"fixture,mode,max_absolute_difference,rms_difference\n";
 for(bool chord:{false,true})for(int mode=0;mode<3;++mode){const auto a=render(48000,chord,mode,512),b=readWav(reference/(std::string(chord?"chord":"single")+"-"+(mode==0?"no-noise":mode==1?"air-blend":"noise-solo")+"-raw.wav"));require(a.size()==b.size(),"comparison size");double peak=0,sum=0;
  for(std::size_t i=0;i<a.size();++i){const double d=double(a[i])-b[i];peak=std::max(peak,std::abs(d));sum+=d*d;}
  report<<(chord?"chord":"percussion")<<','<<mode<<','<<peak<<','<<std::sqrt(sum/a.size())<<'\n';require(peak<1e-7,"production differs from approved study at matched settings");
 }
 std::ofstream html(folder/"index.html"),levels(folder/"levels.csv");
 html<<"<!doctype html><meta charset='utf-8'><title>Noise path diagnostic</title><style>body{font:18px system-ui;background:#101819;color:#e5eee8;max-width:900px;margin:40px auto;padding:16px}section{background:#1a2729;padding:16px;margin:16px 0}audio{width:100%}</style><h1>Noise and the shared drive</h1><p>Same E minor chord, low-pass sweep and envelope. Compare settings within each chain. All listening clips are independently matched to -24 dBFS RMS; raw files retained. Drive can change the whole mixed signal when noise is added. This is a diagnostic comparison, not a proposed replacement sound.</p>";
 levels<<"file,raw_rms,raw_peak,listening_gain,listening_peak\n";
 for(int chain=0;chain<3;++chain)for(int mode=0;mode<3;++mode){auto a=render(48000,true,mode,512,chain);const std::string name="chain"+std::to_string(chain)+"-mode"+std::to_string(mode);wav(folder/(name+"-raw.wav"),a);double sum=0,peak=0;for(float x:a){sum+=double(x)*x;peak=std::max(peak,std::abs(double(x)));}const double rms=std::sqrt(sum/a.size()),gain=std::pow(10.,-24./20)/rms;require(peak*gain<.95,"matched headroom");for(auto&x:a)x=float(x*gain);wav(folder/(name+".wav"),a);levels<<name<<','<<rms<<','<<peak<<','<<gain<<','<<peak*gain<<'\n';html<<"<section><h2>"<<(chain==0?"Both drives bypassed":chain==1?"Shared drive 5 / memory .8":"Voice drive 24 dB + shared drive 5 / memory .8")<<" / "<<(mode==0?"oscillators only":mode==1?"oscillators + noise .08":"noise only .35")<<"</h2><audio controls preload='none' src='"<<name<<".wav'></audio></section>";}
 html<<"<script>document.addEventListener('play',e=>{document.querySelectorAll('audio').forEach(a=>{if(a!==e.target)a.pause()})},true)</script>";
 require(bool(report)&&bool(html)&&bool(levels),"write diagnostic report");std::cout<<"PASS: six production renders match approved raw fixtures within 1e-7 absolute; nine drive-context listening clips and raw files written\n";return 0;
}catch(const std::exception&e){std::cerr<<"FAIL: "<<e.what()<<'\n';return 1;}}
