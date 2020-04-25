
#include "FrameLib_OnePole.h"

OnePole::ModeType OnePole::sModes
{{
    Mode("lpf", &OnePole::lpf),
    Mode("hpf", &OnePole::hpf)
}};

OnePole::ParamType OnePole::sParameters
{{
    Param("freq", 500.0, Min(0.0))
    
}};

void OnePole::reset()
{
    y1 = 0.0;
}

OnePole::Coefficients OnePole::calculateCoefficients(double freq, double samplingRate)
{
    return Coefficients(sin((freq * twopi()) / samplingRate));
}

double OnePole::process(double x, const Coefficients& coeff)
{
    double y = y1 + coeff.f0 * (x - y1);
    
    y1 = y;
    
    return y;
}

double OnePole::hpf(double x)
{
    return x - y1;
}

double OnePole::lpf(double x)
{
    return y1;
}
