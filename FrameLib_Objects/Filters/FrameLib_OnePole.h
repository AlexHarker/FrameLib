
#ifndef FRAMELIB_ONEPOLE_H
#define FRAMELIB_ONEPOLE_H

#include "FrameLib_Filter_Template.h"

namespace FrameLib_Filters
{
    class OnePole : public FrameLib_FilterBase<OnePole, 1, 2>
    {
        
    public:
        
        OnePole() : f0(0.0), y1(0.0) {}
        
        // Filter Implementation
        
        void operator()(double x);
        
        double hpf(double x);
        double lpf(double x);
        
        void reset();
        
        void updateCoefficients(double freq, double samplingRate);
        
        // Parameters / Modes
        
        constexpr static ParamType sParameters
        {
            Param("freq", 500.0, Min(0.0))
        };
        
        constexpr static ModeType sModes
        {
            Mode("lpf", &OnePole::lpf),
            Mode("hpf", &OnePole::hpf)
        };
        
    private:
        
        // Coefficients / Memories
        
        double f0;
        double y1;
    };
}

using FrameLib_OnePole = FrameLib_Filter<FrameLib_Filters::OnePole>;

#endif
