
#ifndef FRAMELIB_0DFSVF_H
#define FRAMELIB_0DFSVF_H

#include "FrameLib_Filter_Template.h"

class SVF : public FrameLib_FilterBase<SVF, 2, 3>
{
    
public:

    SVF() : r(0.0), g(0.0), s1(0.0), s2(0.0), lp(0.0), bp(0.0), hp(0.0) {}
    
    void operator()(double x);
    
    double hpf(double x);
    double bpf(double x);
    double lpf(double x);
    
    // Reset
    
    void reset();
    
    // Coefficients
    
    void updateCoefficients(double freq, double resonance, double samplingRate);
    
    constexpr static ParamType sParameters
    {{
        Param("freq", 500.0, Min(0.0)),
        Param("reson", 0.0, Clip(0.0, 1.0))
    }};
    
    constexpr static ModeType sModes
    {{
        Mode("lpf", &SVF::lpf),
        Mode("bpf", &SVF::bpf),
        Mode("hpf", &SVF::hpf)
    }};
    
private:
    
    double r, g;
    double s1, s2, lp, bp, hp;
};

using FrameLib_0dfSVF = FrameLib_Filter<SVF>;

#endif
