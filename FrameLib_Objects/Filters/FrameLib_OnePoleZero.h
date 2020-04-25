
#ifndef FRAMELIB_ONEPOLEZERO_H
#define FRAMELIB_ONEPOLEZERO_H

#include "FrameLib_Filter_Template.h"

class OnePoleZero : public FrameLib_FilterBase<OnePoleZero, 2, 1>
{
    
public:
    
    static ModeType sModes;
    static ParamType sParameters;
    
    struct Coefficients
    {
        Coefficients() : a0(0.0), a1(0.0) {}
        Coefficients(double v1, double v2) : a0(v1), a1(v2) {}
        
        double a0, a1;
    };
    
    OnePoleZero() : r1(0.0), y1(0.0) {}
    
    void reset();
    
    Coefficients calculateCoefficients(double freq, double samplingRate);
    
    double process(double x, const Coefficients& coeff);
    double hpf(double x);
    double lpf(double x);
    
private:
    
    double r1;
    double y1;
};

using FrameLib_OnePoleZero = FrameLib_Filter<OnePoleZero>;

#endif
