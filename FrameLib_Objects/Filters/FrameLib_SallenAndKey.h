
#ifndef FRAMELIB_SALLENANDKEY_H
#define FRAMELIB_SALLENANDKEY_H

#include "FrameLib_Filter_Template.h"

namespace FrameLib_Filters
{
    class SallenAndKey : public FrameLib_FilterBase<SallenAndKey, 2, 3>
    {
        
    public:
        
        SallenAndKey() : k(0.0), a0(0.0), a1(0.0), a2(0.0), a3(0.0), a4(0.0), a5(0.0), ic1eq(0.0), ic2eq(0.0), v1(0.0), v2(0.0) {}
        
        // Filter Implementation
        
        void operator()(double x);
        
        double hpf(double x);
        double bpf(double x);
        double lpf(double x);
        
        void reset();
        
        void updateCoefficients(double freq, double resonance, double samplingRate);
        
        // Parameters / Modes
        
        constexpr static ParamType sParameters
        {
            Param("freq", "Frequency", "sets the filter cutoff frequency.", 500.0, Min(0.0)),
            Param("reson", "Resonance", "sets the filter resonance.", 0.0, Clip(0.0, 1.0))
        };
        
        constexpr static ModeType sModes
        {
            Mode("lpf", "Low Pass Output", &SallenAndKey::lpf),
            Mode("bpf", "Band Pass Output", &SallenAndKey::bpf),
            Mode("hpf", "High Pass Output", &SallenAndKey::hpf)
        };
        
    private:
        
        // Coefficients / Memories / Outputs
        
        double k, a0, a1, a2, a3, a4, a5;
        double ic1eq, ic2eq;
        double v1, v2;
    };
}

using FrameLib_SallenAndKey = FrameLib_Filter<FrameLib_Filters::SallenAndKey>;

#endif

