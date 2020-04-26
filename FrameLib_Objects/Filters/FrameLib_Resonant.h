
#ifndef FRAMELIB_RESONANT_H
#define FRAMELIB_RESONANT_H

#include "FrameLib_Filter_Template.h"

class Resonant : public FrameLib_FilterBase<Resonant, 2, 2>
{
    
public:
    
    static ModeType sModes;
    static ParamType sParameters;
    
    Resonant() : scl(0.0), r2(0.0), y1(0.0), y2(0.0) {}
    
    void reset();
    
    void updateCoefficients(double freq, double reson, double samplingRate);
    
    double process(double x);
    double hpf(double x);
    double lpf(double x);
    
private:
    
    double scl, r2;
    double y1, y2;
};

using FrameLib_Resonant = FrameLib_Filter<Resonant>;

#endif
