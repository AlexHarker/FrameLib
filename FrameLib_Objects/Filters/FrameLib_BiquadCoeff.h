
#ifndef FRAMELIB_BIQUADCOEFF_H
#define FRAMELIB_BIQUADCOEFF_H

#include "FrameLib_Filter_Template.h"

namespace FrameLib_Filters
{
    class BiquadCoeff : public Coefficients<BiquadCoeff, 3, 8, 5>
    {
        struct Common
        {
            Common(double freq, double samplingRate, double Q);
            
            double cosw0;
            double alpha;
        };
        
        struct ShelfCommon
        {
            ShelfCommon(double freq, double samplingRate, double gain, double S);
            
            double A;
            double alphaFactor;
            double Am1;
            double Ap1;
            double Am1cosw0;
            double Ap1cosw0;
        };

        // Use an alias for brevity
        
        using coeff = double&;

    public:
        
        // Filter Implementation
        
        void lpf(double f0, double gain, double Q, double sr, coeff a1, coeff a2, coeff b0, coeff b1, coeff b2);
        void bpf(double f0, double gain, double Q, double sr, coeff a1, coeff a2, coeff b0, coeff b1, coeff b2);
        void hpf(double f0, double gain, double Q, double sr, coeff a1, coeff a2, coeff b0, coeff b1, coeff b2);
        void bell(double f0, double gain, double Q, double sr, coeff a1, coeff a2, coeff b0, coeff b1, coeff b2);
        void resonant(double f0, double gain, double Q, double sr, coeff a1, coeff a2, coeff b0, coeff b1, coeff b2);
        void notch(double f0, double gain, double Q, double sr, coeff a1, coeff a2, coeff b0, coeff b1, coeff b2);
        void lowshelf(double f0, double gain, double S, double sr, coeff a1, coeff a2, coeff b0, coeff b1, coeff b2);
        void highshelf(double f0, double gain, double S, double sr, coeff a1, coeff a2, coeff b0, coeff b1, coeff b2);

        // Description / Parameters / Modes
        
        static constexpr Description sDescription
        {
            "Calculates biquad filter coefficients"
        };
        
        static constexpr ParamType sParameters
        {
            Param("freq", "Frequency Values", "Sets the filter frequency.", 0.0, Min(0.0)),
            Param("gain", "Gain Values", "Sets the linear gain of the filter.", 1.0, Min(0.0)),
            Param("qs", "Q or S Values", "Sets the q or s of the filter.", 0.0, Min(0.0))
        };
        
        static constexpr CoeffType sCoefficients
        {
            Coeff("a1", "A1 Coefficients"),
            Coeff("a2", "A2 Coefficients"),
            Coeff("b0", "B0 Coefficients"),
            Coeff("b1", "B1 Coefficients"),
            Coeff("b2", "B2 Coefficients")
        };
        
        static constexpr ModeType sModes
        {
            Mode("lowpass", "low pass", &BiquadCoeff::lpf),
            Mode("bandpass", "band pass", &BiquadCoeff::bpf),
            Mode("highpass", "high pass", &BiquadCoeff::hpf),
            Mode("peaknotch", "peak notch", &BiquadCoeff::bell),
            Mode("resonant", "resonant", &BiquadCoeff::resonant),
            Mode("notch", "band reject", &BiquadCoeff::notch),
            Mode("lowshelf", "low shelf", &BiquadCoeff::lowshelf),
            Mode("highshelf", "high shelf", &BiquadCoeff::highshelf)
        };
    };
}

using FrameLib_BiquadCoeff = FrameLib_Filter<FrameLib_Filters::BiquadCoeff>;

#endif
