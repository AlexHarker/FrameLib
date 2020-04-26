
#ifndef FRAMELIB_ONEPOLE_H
#define FRAMELIB_ONEPOLE_H

#include "FrameLib_Filter_Template.h"

class OnePole : public FrameLib_FilterBase<OnePole, 2, 1>
{
    
public:
    
    static ModeType sModes;
    static ParamType sParameters;
    
    OnePole() : f0(0.0), y1(0.0) {}
        
    void reset();

    void updateCoefficients(double freq, double samplingRate);
    
    void operator()(double x);
    double hpf(double x);
    double lpf(double x);
    
private:
    
    double f0;
    double y1;
};

using FrameLib_OnePole = FrameLib_Filter<OnePole>; 

#endif
