
#include "FrameLib_Resonant.h"

Resonant::ModeType Resonant::sModes
{{
    Mode("lpf", &Resonant::lpf),
    Mode("hpf", &Resonant::hpf)
}};

Resonant::ParamType Resonant::sParameters
{{
    Param("freq", 500.0, Min(0.0)),
    Param("reson", 500.0, Clip(0.0, 1.0))
}};

void Resonant::reset()
{
    y1 = 0.0;
    y2 = 0.0;
}

void Resonant::updateCoefficients(double freq, double reson, double samplingRate)
{
    double frad = cos(freq * pi() * 2.0 / samplingRate);
    double res = 0.882497 * exp(reson * 0.125);
    
    scl = (frad * res) * -2.0;
    r2 = res * res;
}

double Resonant::process(double x)
{
    x = x * ((scl + r2) + 1.0);
    double y = x - ((scl * y1) + (r2 * y2));
    
    y2 = y1;
    y1 = y;
    
    return y;
}

double Resonant::hpf(double x)
{
    return x - y1;
}

double Resonant::lpf(double x)
{
    return y1;
}
