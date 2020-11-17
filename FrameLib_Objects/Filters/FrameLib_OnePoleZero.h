
#ifndef FRAMELIB_ONEPOLEZERO_H
#define FRAMELIB_ONEPOLEZERO_H

#include "FrameLib_Filter_Template.h"

namespace FrameLib_Filters
{
    class OnePoleZero : public Filter<OnePoleZero, 1, 2, true>
    {
    public:
        
        OnePoleZero() : a0(0.0), a1(0.0), r1(0.0), y1(0.0) {}
        
        // Filter Implementation
        
        void operator()(double x);
        
        double hpf(double x);
        double lpf(double x);
        
        void reset();
        
        void updateCoefficients(double freq, double samplingRate);
        
        // Description / Parameters / Modes
        
        constexpr static Description sDescription
        {
            "One-pole and one-zero filter"
        };
        
        constexpr static ParamType sParameters
        {
            Param("freq", "Frequency", "Sets the filter cutoff frequency.", 500.0, Min(0.0))
        };
        
        constexpr static ModeType sModes
        {
            Mode("lpf", "Low Pass Output", "low pass", &OnePoleZero::lpf),
            Mode("hpf", "High Pass Output", "high pass", &OnePoleZero::hpf)
        };
        
    private:
        
        // Coefficients / Memories
        
        double a0, a1;
        double r1, y1;
    };
}

using FrameLib_OnePoleZero = FrameLib_Filter<FrameLib_Filters::OnePoleZero>;

#endif
