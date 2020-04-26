
#ifndef FRAMELIB_ONEPOLEZERO_H
#define FRAMELIB_ONEPOLEZERO_H

#include "FrameLib_Filter_Template.h"

class OnePoleZero : public FrameLib_FilterBase<OnePoleZero, 2, 1>
{
    
public:
    
    static ModeType sModes;
    static ParamType sParameters;
   
    OnePoleZero() : a0(0.0), a1(0.0), r1(0.0), y1(0.0) {}
    
    void reset();
    
    void updateCoefficients(double freq, double samplingRate);
    
    double process(double x);
    double hpf(double x);
    double lpf(double x);
    
private:
    
    double a0;
    double a1;
    double r1;
    double y1;
};

using FrameLib_OnePoleZero = FrameLib_Filter<OnePoleZero>;

#endif
