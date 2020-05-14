
#ifndef FRAMELIB_ONEPOLE_H
#define FRAMELIB_ONEPOLE_H

#include "FrameLib_Filter_Template.h"

namespace FrameLib_Filters
{
    class OnePole : public FrameLib_FilterBase<OnePole, 1, 2>
    {
        
    public:
        
        OnePole() : f0(0.0), z1(0.0), lp(0.0), hp(0.0) {}
        
        // Filter Implementation
        
        void operator()(double x);
        
        double hpf(double x);
        double lpf(double x);
        
        void reset();
        
        void updateCoefficients(double freq, double samplingRate);
        
        // Parameters / Modes
        
        constexpr static ParamType sParameters
        {
            Param("freq", "Frequency", "Sets the filter cutoff frequency.", 500.0, Min(0.0))
        };
        
        constexpr static ModeType sModes
        {
            Mode("lpf", "Low Pass Output", "low pass", &OnePole::lpf),
            Mode("hpf", "High Pass Output", "high pass", &OnePole::hpf)
        };
        
    private:
        
        // Coefficients / Memories / Outputs
        
        double f0;
        double z1;
        double lp, hp;
    };
}

using FrameLib_OnePole = FrameLib_Filter<FrameLib_Filters::OnePole>;

#endif
