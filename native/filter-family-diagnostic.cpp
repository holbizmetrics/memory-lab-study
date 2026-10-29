#include <string>
#define main filterStudyMain
#include "filter-family-study.cpp"
#undef main
int main(int argc,char**argv){try{
 require(argc==3,"Expected original study folder and a new CSV path");const fs::path reference(argv[1]),path(argv[2]);require(!fs::exists(path),"report already exists");std::ofstream report(path);require(bool(report),"open report");report<<"file,max_absolute_difference,rms_difference\n";
 double worst=0;
 for(int context=0;context<3;++context)for(const auto&s:settings){const bool chord=context!=0,driven=context==2;const std::string name=std::string(driven?"chord-shared-":chord?"chord-":"single-")+s.name+"-raw.wav";
  std::ifstream f(reference/name,std::ios::binary);require(bool(f),"reference missing");f.seekg(0,std::ios::end);require(f.tellg()==std::streampos(44+129600*4),"reference size");f.seekg(44);std::vector<float>b(129600);f.read(reinterpret_cast<char*>(b.data()),b.size()*4);require(bool(f),"read reference");const auto a=render(48000,chord,s,512,driven);require(a.size()==b.size(),"sample count");double peak=0,sum=0;
  for(std::size_t i=0;i<a.size();++i){const double d=double(a[i])-b[i];peak=std::max(peak,std::abs(d));sum+=d*d;}worst=std::max(worst,peak);report<<name<<','<<peak<<','<<std::sqrt(sum/a.size())<<'\n';require(peak<1e-7,"production filter differs from study");
 }
 require(bool(report),"write report");std::cout<<"PASS: 12 production filter contexts match study within 1e-7 absolute. Worst difference "<<worst<<'\n';return 0;
}catch(const std::exception&e){std::cerr<<"FAIL: "<<e.what()<<'\n';return 1;}}
