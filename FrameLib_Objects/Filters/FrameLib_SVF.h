
#ifndef FRAMELIB_SVF_H
#define FRAMELIB_SVF_H

#include "FrameLib_Filter_Template.h"

namespace FrameLib_Filters
{
    class SVF : public Filter<SVF, 2, 3, true>
    {
    public:
        
        SVF() : g(0.0), r(0.0), s1(0.0), s2(0.0), lp(0.0), bp(0.0), hp(0.0) {}
        
        // Filter Implementation
        
        void operator()(double x);
        
        double hpf(double x);
        double bpf(double x);
        double lpf(double x);
        
        void reset();
        
        void updateCoefficients(double freq, double resonance, double samplingRate);
        
        // Description / Parameters / Modes
        
        static constexpr Description sDescription
        {
            "Applies a state variable filter to the input"
        };
        
        static constexpr ParamType sParameters
        {
            Param("freq", "Frequency Values", "Sets the filter cutoff frequency.", 500.0, Min(0.0)),
            Param("res", "Resonance Values", "Sets the filter resonance.", 0.0, Clip(0.0, 1.0))
        };
        
        static constexpr ModeType sModes
        {
            Mode("lowpass", "Low Pass Output", "low pass", &SVF::lpf),
            Mode("bandpass", "Band Pass Output", "band pass", &SVF::bpf),
            Mode("highpass", "High Pass Output", "high pass", &SVF::hpf)
        };
        
    private:
        
        // Coefficients / Memories / Outputs
        
        double g, r;
        double s1, s2;
        double lp, bp, hp;
    };
}

using FrameLib_SVF = FrameLib_Filter<FrameLib_Filters::SVF>;

#endif
