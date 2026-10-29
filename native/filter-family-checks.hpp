#pragma once
#include <complex>
void filterChecks(){
 double worst=0;
 for(double sr:{44100.,48000.,96000.})for(double cutoff:{600.,2200.})for(double q:{.7071067811865476,1.2})for(int mode:{1,2,3}){
  StudyFilterFamily filter;std::vector<double>impulse(16384);
  for(std::size_t i=0;i<impulse.size();++i){impulse[i]=filter.tick(i==0?1.:0.,sr,cutoff,q,mode);require(std::isfinite(impulse[i]),"nonfinite filter impulse");}
  require(std::abs(impulse.back())<1e-12,"filter impulse did not decay");
  for(double frequency:{50.,200.,600.,2200.,6000.,14000.}){
   const double w=std::tan(pi*frequency/sr)/std::tan(pi*cutoff/sr);
   const std::complex<double>den(1-w*w,w/q),lp=1./den,bp=std::complex<double>(0,w/q)/den;
   const auto expected=mode==1?lp*lp:mode==2?bp*bp:.5*(bp*bp+lp);
   std::complex<double>measured{};for(std::size_t i=0;i<impulse.size();++i)measured+=impulse[i]*std::exp(std::complex<double>(0,-2*pi*frequency*i/sr));
   worst=std::max(worst,std::abs(measured-expected));require(std::abs(measured-expected)<1e-8,"filter transfer disagrees with declared equation");
  }
  double dc=0;for(double x:impulse)dc+=x;require(std::abs(dc-(mode==1?1:mode==2?0:.5))<1e-8,"filter DC gain mismatch");
 }
 // Bounded high-Q/edge-cutoff exercise; not a full nonlinear or automation audit.
 for(double sr:{44100.,48000.,96000.})for(double cutoff:{30.,18000.})for(int mode:{1,2,3}){
  StudyFilterFamily filter;double peak=0;for(int i=0;i<int(sr);++i){const double x=i==0?1.:0.,y=filter.tick(x,sr,std::min(cutoff,sr*.4),8,mode);require(std::isfinite(y),"high-Q nonfinite filter");peak=std::max(peak,std::abs(y));}require(peak<100,"excessive high-Q impulse");
 }
 std::cout<<"PASS filter equations: 36 impulse cases / 216 complex response comparisons, DC gain, decay; 18 high-Q edge cases finite. Worst complex error "<<worst<<'\n';
}
