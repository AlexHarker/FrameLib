
#ifndef FRAMELIB_ONEPOLEZERO_H
#define FRAMELIB_ONEPOLEZERO_H

#include "FrameLib_Filter_Template.h"

class OnePoleZero : public FrameLib_FilterBase<OnePoleZero, 1, 2>
{
    
public:
    
    OnePoleZero() : a0(0.0), a1(0.0), r1(0.0), y1(0.0) {}

    void operator()(double x);
    
    double hpf(double x);
    double lpf(double x);
    
    void reset();
    
    void updateCoefficients(double freq, double samplingRate);
    
    constexpr static ParamType sParameters
    {{
        Param("freq", 500.0, Min(0.0))
    }};
    
    constexpr static ModeType sModes
    {{
        Mode("lpf", &OnePoleZero::lpf),
        Mode("hpf", &OnePoleZero::hpf)
    }};
    
private:
    
    double a0;
    double a1;
    double r1;
    double y1;
};

using FrameLib_OnePoleZero = FrameLib_Filter<OnePoleZero>;

#endif
