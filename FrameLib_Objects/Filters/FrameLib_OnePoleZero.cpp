
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

void OnePoleZero::updateCoefficients(double freq, double samplingRate)
{
    const double fc = pi() * freq / samplingRate;
    
    a0 = (2.0 * sin(fc)) / (cos(fc) + sin(fc));
    a1 = 1.0 - (a0 * 2.0);
}

double OnePoleZero::process(double x)
{
    const double w = x * a0;
    const double y = r1 + w;
    
    y1 = y;
    r1 = w + (y * a1);
    
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
