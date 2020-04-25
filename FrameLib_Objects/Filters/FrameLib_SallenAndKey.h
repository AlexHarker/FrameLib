
#ifndef FRAMELIB_SALLENANDKEY_H
#define FRAMELIB_SALLENANDKEY_H

#include "FrameLib_Filter_Template.h"

class SallenAndKey : public FrameLib_FilterBase<SallenAndKey, 3, 2>
{
    
public:
    
    static ModeType sModes;
    static ParamType sParameters;
    
    struct Coefficients
    {
        Coefficients() : k(0.0), a0(0.0), a1(0.0), a2(0.0), a3(0.0), a4(0.0), a5(0.0) {}
        Coefficients(double K, double A0, double A1, double A2, double A3, double A4, double A5)
        : k(K), a0(A0), a1(A1), a2(A2), a3(A3), a4(A4), a5(A5) {}
        
        double k, a0, a1, a2, a3, a4, a5;
    };
    
    SallenAndKey() : v1(0.0), v2(0.0), ic1eq(0.0), ic2eq(0.0) {}
    
    // Reset
    
    void reset();
    
    // Coefficients
    
    Coefficients calculateCoefficients(double freq, double resonance, double samplingRate);
    
    double process(double x, const Coefficients& coeff);
    double hpf(double x);
    double bpf(double x);
    double lpf(double x);
    
private:
    
    double v1, v2, ic1eq, ic2eq;
};

using FrameLib_SallenAndKey = FrameLib_Filter<SallenAndKey>;

#endif

