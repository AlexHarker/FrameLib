
#include "FrameLib_OnePoleZero.h"

OnePoleZero::ModeType OnePoleZero::sModes
{{
    Mode("lpf", &OnePoleZero::lpf),
    Mode("hpf", &OnePoleZero::hpf)
}};

OnePoleZero::ParamType OnePoleZero::sParameters
{{
    Param("freq", 500.0, Min(0.0))
}};

void OnePoleZero::reset()
{
    y1 = 0.0;
    r1 = 0.0;
}

OnePoleZero::Coefficients OnePoleZero::calculateCoefficients(double freq, double samplingRate)
{
    Coefficients coeff;
    
    const double fc = pi() * freq / samplingRate;
    
    coeff.a0 = (2.0 * sin(fc)) / (cos(fc) + sin(fc));
    coeff.a1 = 1.0 - (coeff.a0 * 2.0);
    
    return coeff;
}

double OnePoleZero::process(double x, const Coefficients& coeff)
{
    const double w = x * coeff.a0;
    const double y = r1 + w;
    
    y1 = y;
    r1 = w + (y * coeff.a1);
    
    return y;
}

double OnePoleZero::hpf(double x)
{
    return x - y1;
}

double OnePoleZero::lpf(double x)
{
    return y1;
}

