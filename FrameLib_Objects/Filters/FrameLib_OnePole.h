
#ifndef FRAMELIB_ONEPOLE_H
#define FRAMELIB_ONEPOLE_H

#include "FrameLib_Filter_Template.h"

class OnePole : public FrameLib_FilterBase<OnePole, 2, 1>
{
    
public:
    
    static ModeType sModes;
    static ParamType sParameters;
    
    struct Coefficients
    {
        Coefficients() : f0(0.0) {}
        Coefficients(double v) : f0(v) {}
        
        double f0;
    };
    
    OnePole() : y1(0.0) {}
        
    void reset();

    Coefficients calculateCoefficients(double freq, double samplingRate);
    
    double process(double x, const Coefficients& coeff);
    double hpf(double x);
    double lpf(double x);
    
private:
    
    double y1;
};

using FrameLib_OnePole = FrameLib_Filter<OnePole>; 

#endif
