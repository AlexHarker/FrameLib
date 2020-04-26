
#ifndef FRAMELIB_SALLENANDKEY_H
#define FRAMELIB_SALLENANDKEY_H

#include "FrameLib_Filter_Template.h"

class SallenAndKey : public FrameLib_FilterBase<SallenAndKey, 3, 2>
{
    
public:
    
    static ModeType sModes;
    static ParamType sParameters;
    
    SallenAndKey() : k(0.0), a0(0.0), a1(0.0), a2(0.0), a3(0.0), a4(0.0), a5(0.0), v1(0.0), v2(0.0), ic1eq(0.0), ic2eq(0.0) {}
    
    // Reset
    
    void reset();
    
    // Coefficients
    
    void updateCoefficients(double freq, double resonance, double samplingRate);
    
    double process(double x);
    double hpf(double x);
    double bpf(double x);
    double lpf(double x);
    
private:
    
    double k, a0, a1, a2, a3, a4, a5;
    double v1, v2, ic1eq, ic2eq;
};

using FrameLib_SallenAndKey = FrameLib_Filter<SallenAndKey>;

#endif

