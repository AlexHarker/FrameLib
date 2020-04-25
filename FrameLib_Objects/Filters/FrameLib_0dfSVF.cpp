
#include "FrameLib_0dfSVF.h"

ZeroDelayFeedbackSVF::ModeType ZeroDelayFeedbackSVF::sModes
{{
    Mode("lpf", &ZeroDelayFeedbackSVF::lpf),
    Mode("bpf", &ZeroDelayFeedbackSVF::bpf),
    Mode("hpf", &ZeroDelayFeedbackSVF::hpf)
}};

ZeroDelayFeedbackSVF::ParamType ZeroDelayFeedbackSVF::sParameters
{{
    Param("freq", 500.0, Min(0.0)),
    Param("reson", 500.0, Clip(0.0, 1.0))
}};

void ZeroDelayFeedbackSVF::reset()
{
    s1 = 0.0;
    s2 = 0.0;
    lp = 0.0;
    bp = 0.0;
    hp = 0.0;
}

ZeroDelayFeedbackSVF::Coefficients ZeroDelayFeedbackSVF::calculateCoefficients(double freq, double reson, double samplingRate)
{
    Coefficients coeff;
    double srConst = 0.5 / samplingRate;

    coeff.r = std::min(std::max(1.0 - reson, 0.005), 1.0);
    coeff.g = ((2.0 * samplingRate) * tan((freq * twopi()) * srConst) * srConst);
    
    return coeff;
}

double ZeroDelayFeedbackSVF::process(double x, const Coefficients& coeff)
{
    // Compute highpass then bandpass  by applying 1st integrator to highpass output and update state
    
    const double& g = coeff.g;
    const double& r = coeff.r;
    
    hp = (x - (2.0 * r * s1) - (g * s1) - s2) / (1.0 + (2.0 * r * g) + (g * g));
    bp = g * hp + s1;
    lp = g * bp + s2;
    
    s1 = g * hp + bp;
    s2 = g * bp + lp;
    
    return 0.0;
}

double ZeroDelayFeedbackSVF::hpf(double x)
{
    return hp;
}

double ZeroDelayFeedbackSVF::bpf(double x)
{
    return bp;
}

double ZeroDelayFeedbackSVF::lpf(double x)
{
    return lp;
}
