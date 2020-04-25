
#ifndef FRAMELIB_0DFSVF_H
#define FRAMELIB_0DFSVF_H

#include "FrameLib_Filter_Template.h"

class ZeroDelayFeedbackSVF : public FrameLib_FilterBase<ZeroDelayFeedbackSVF, 3, 2>
{
    
public:
    
    static ModeType sModes;
    static ParamType sParameters;
    
    struct Coefficients
    {
        Coefficients() : r(0.0), g(0.0) {}
        Coefficients(double r0, double g0)
        : r(r0), g(g0) {}
        
        double r;
        double g;
    };
    
    ZeroDelayFeedbackSVF() : s1(0.0), s2(0.0), lp(0.0), bp(0.0), hp(0.0) {}
    
    // Reset
    
    void reset();
    
    // Coefficients
    
    Coefficients calculateCoefficients(double freq, double resonance, double samplingRate);
    
    double process(double x, const Coefficients& coeff);
    double hpf(double x);
    double bpf(double x);
    double lpf(double x);
    
private:
    
    double s1, s2, lp, bp, hp;
};

using FrameLib_0dfSVF = FrameLib_Filter<ZeroDelayFeedbackSVF>;

#endif
