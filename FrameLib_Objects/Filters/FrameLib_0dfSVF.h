
#ifndef FRAMELIB_0DFSVF_H
#define FRAMELIB_0DFSVF_H

#include "FrameLib_Filter_Template.h"

class SVF : public FrameLib_FilterBase<SVF, 3, 2>
{
    
public:
    
    static ModeType sModes;
    static ParamType sParameters;
    
    SVF() : r(0.0), g(0.0), s1(0.0), s2(0.0), lp(0.0), bp(0.0), hp(0.0) {}
    
    // Reset
    
    void reset();
    
    // Coefficients
    
    void updateCoefficients(double freq, double resonance, double samplingRate);
    
    double process(double x);
    double hpf(double x);
    double bpf(double x);
    double lpf(double x);
    
private:
    
    double r, g;
    double s1, s2, lp, bp, hp;
};

using FrameLib_0dfSVF = FrameLib_Filter<SVF>;

#endif
