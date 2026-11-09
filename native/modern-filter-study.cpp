#include "modern-filter.hpp"
#include <complex>
#include <iostream>
#include <stdexcept>
#include <limits>

using C=std::complex<double>;
constexpr double pi=3.14159265358979323846;
void require(bool pass,const char*why){if(!pass)throw std::runtime_error(why);}
// Independent z-domain reference; no production filter state or blend code.
std::array<C,5> response(double sr,double fc,double q,double bw,double f){
 auto ratio=[&](double corner){return std::tan(pi*f/sr)/std::tan(pi*std::clamp(corner,5.,sr*.4)/sr);};
 auto lp=[&](double corner,double resonance){double w=ratio(corner);return 1./C(1-w*w,w/resonance);};
 auto hp=[&](double corner,double resonance){double w=ratio(corner);return -w*w/C(1-w*w,w/resonance);};
 const C low=lp(fc,q),band=hp(fc/std::exp2(bw*.5),std::sqrt(q))*lp(fc*std::exp2(bw*.5),std::sqrt(q));
 return {band,.5*(band+low),low,low/C(1,ratio(fc)),low*lp(fc,.7071067811865475244)};
}
void tests(){
 for(double sr:{44100.,48000.,96000.,192000.}){
  for(double q:{.5,2.4,3.72,8.})for(double bw:{.25,1.,4.}){
   constexpr double fc=1800;
   memorylab::ModernFilter filter;
   constexpr int length=32768;
   std::array<std::array<C,5>,5> measured{};
   const std::array<double,5> frequencies{80.,450.,1800.,5500.,12000.};
   for(int n=0;n<length;++n){
    const auto sample=filter.tickAll(n==0?1.:0.,sr,fc,q,bw);
    for(int k=0;k<5;++k){
     require(std::isfinite(sample[k]),"nonfinite impulse response");
     for(int f=0;f<5;++f)measured[f][k]+=sample[k]*std::polar(1.,-2*pi*frequencies[f]*n/sr);
    }
   }
   for(int f=0;f<5;++f){const auto expected=response(sr,fc,q,bw,frequencies[f]);
    for(int k=0;k<5;++k)require(std::abs(measured[f][k]-expected[k])<1e-8,"impulse differs from independent transfer function");
   }
   const auto a=response(sr,fc,q,bw,fc);
   require(std::abs(a[4])<=q*.708,"LP4 unexpectedly squares resonance gain");
  }
  // Rapid live changes, including cutoff limits and noise-like excitation.
  memorylab::ModernFilter whole;
  for(int n=0;n<1000000;++n){
   const double x=std::sin(n*1.2345)*.2,fc=(n/137)%2?30.:std::min(18000.,sr*.4);
   const double q=(n/71)%2?8.:.5,bw=(n/97)%2?4.:.25,p=(n%1000)/999.;
   const double a=whole.tick(x,sr,fc,q,bw,p);
   require(std::isfinite(a)&&std::abs(a)<100,"unstable automated filter");
  }
  whole.reset();
  for(int n=0;n<1000;++n)for(double a:whole.tickAll(0,sr,1800,8,4))
   require(a==0,"reset retained signal energy");
 }
 const std::array<double,5> anchors{1,3,8,11,17};
 for(int k=0;k<5;++k)require(memorylab::ModernFilter::blend(anchors,k*.25)==anchors[k],"profile anchor mismatch");
 for(int k=0;k<4;++k)require(memorylab::ModernFilter::blend(anchors,k*.25+.125)==.5*(anchors[k]+anchors[k+1]),"profile interpolation mismatch");
 const auto narrow=response(48000,1800,1.,.25,450),wide=response(48000,1800,1.,4.,450);
 require(std::abs(wide[0])>2*std::abs(narrow[0]),"bandwidth does not open BP skirt");
 for(int k=2;k<5;++k)require(narrow[k]==wide[k],"bandwidth unexpectedly alters low-pass anchors");
 auto slope=[&](int k){auto a=response(192000,500,.7071067811865475,1.,4000),b=response(192000,500,.7071067811865475,1.,8000);return 20*std::log10(std::abs(b[k]/a[k]));};
 require(slope(2)<-11&&slope(2)>-13,"LP2 slope");
 require(slope(3)<-17&&slope(3)>-19,"LP3 slope");
 require(slope(4)<-23&&slope(4)>-25,"LP4 slope");
 std::cout<<"PASS modern filter research: independent complex impulse reference, four rates, Q/BW extremes, profile anchors/interpolation, bandwidth isolation, pole slopes, reset and automated finite output\n";
}
int main(){try{tests();return 0;}catch(const std::exception&e){std::cerr<<"FAIL: "<<e.what()<<'\n';return 1;}}
