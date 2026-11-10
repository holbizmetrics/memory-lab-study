#pragma once
#include <algorithm>
#include <array>
#include <cmath>
#include <complex>

namespace memorylab {
// Behavioural approximation of the documented profile sequence, not a Relic
// circuit model. Keep the released FilterFamily arithmetic untouched.
class ModernFilter {
 struct Section {
  double s1=0,s2=0;
  struct Output { double hp,lp; };
  Output tick(double x,double g,double q){
   const double k=1/q;
   const double hp=(x-s2-(k+g)*s1)/(1+g*(g+k));
   const double bp=g*hp+s1,lp=g*bp+s2;
   s1=2*bp-s1;s2=2*lp-s2;return {hp,lp};
  }
 } low,extraLow,bandHigh,bandLow;
 double onePoleState=0;
 static double g(double sr,double fc){
  return std::tan(3.14159265358979323846*std::clamp(fc,5.,sr*.4)/sr);
 }
public:
 static std::complex<double> response(double sr,double cutoff,double q,double bandwidth,double profile,double frequency){
  using C=std::complex<double>;
  const double w=std::tan(3.14159265358979323846*std::min(frequency,sr*.49)/sr);
  const auto lp=[&](double corner,double resonance){const double r=w/g(sr,corner);return 1./C(1-r*r,r/resonance);};
  const auto hp=[&](double corner,double resonance){const double r=w/g(sr,corner);return -r*r/C(1-r*r,r/resonance);};
  q=std::clamp(q,.5,8.);bandwidth=std::clamp(bandwidth,.25,4.);
  const C low=lp(cutoff,q),band=hp(cutoff/std::exp2(bandwidth*.5),std::sqrt(q))*lp(cutoff*std::exp2(bandwidth*.5),std::sqrt(q));
  const std::array<C,5> anchors{band,.5*(band+low),low,low/C(1,w/g(sr,cutoff)),low*lp(cutoff,.7071067811865475244)};
  const double position=4*std::clamp(profile,0.,1.);const int left=std::min(3,int(position));
  return anchors[left]+(position-left)*(anchors[left+1]-anchors[left]);
 }
 void reset(){*this=ModernFilter{};}
 // bandwidth: octave separation of the BP4 lower/upper corner frequencies.
 // Resonance controls one low-pass section; the extra poles use fixed damping.
 // Thus LP4 does not square Q as the legacy cascaded family does.
 std::array<double,5> tickAll(double x,double sr,double cutoff,double q,double bandwidth){
  q=std::clamp(q,.5,8.);bandwidth=std::clamp(bandwidth,.25,4.);
  const double center=g(sr,cutoff),spread=std::exp2(bandwidth*.5);
  const double lp2=low.tick(x,center,q).lp;
  const double lp4=extraLow.tick(lp2,center,.7071067811865475244).lp;
  const double lp3=(center*lp2+onePoleState)/(1+center);
  onePoleState=2*lp3-onePoleState;
  const double lower=bandHigh.tick(x,g(sr,cutoff/spread),std::sqrt(q)).hp;
  const double bp4=bandLow.tick(lower,g(sr,cutoff*spread),std::sqrt(q)).lp;
  return {bp4,.5*(bp4+lp2),lp2,lp3,lp4};
 }
 static double blend(const std::array<double,5>&anchors,double profile){
  const double position=4*std::clamp(profile,0.,1.);
  const int left=std::min(3,int(position));const double fraction=position-left;
  return anchors[left]+fraction*(anchors[left+1]-anchors[left]);
 }
 double tick(double x,double sr,double cutoff,double q,double bandwidth,double profile){
  return blend(tickAll(x,sr,cutoff,q,bandwidth),profile);
 }
};
}
