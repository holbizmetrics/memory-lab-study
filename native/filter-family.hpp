#pragma once
#include <cmath>
#include <array>
// Original filter topology. No claim of a hardware circuit model.
namespace memorylab {
class FilterFamily {
 struct Section {
  double ic1=0,ic2=0;
  struct Output{double lp,bp;};
  Output tick(double x,double g,double k){const double hp=(x-ic2-(k+g)*ic1)/(1+g*(g+k));const double bp=g*hp+ic1,lp=g*bp+ic2;ic1=2*bp-ic1;ic2=2*lp-ic2;return {lp,k*bp};}
 } first,lowSecond,bandSecond;
public:
 std::array<double,3> tickAll(double x,double sr,double cutoff,double q){
  const double g=std::tan(3.14159265358979323846*cutoff/sr),k=1/q;
  const auto a=first.tick(x,g,k);const auto lp4=lowSecond.tick(a.lp,g,k).lp,bp4=bandSecond.tick(a.bp,g,k).bp;
  return {lp4,bp4,.5*(bp4+a.lp)};
 }
 double tick(double x,double sr,double cutoff,double q,int mode){const auto out=tickAll(x,sr,cutoff,q);return mode==1?out[0]:mode==2?out[1]:out[2];}
};
using StudyFilterFamily=FilterFamily;
}
