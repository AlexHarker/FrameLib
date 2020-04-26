
#include "FrameLib_SallenAndKey.h"

SallenAndKey::ModeType SallenAndKey::sModes
{{
    Mode("lpf", &SallenAndKey::lpf),
    Mode("bpf", &SallenAndKey::bpf),
    Mode("hpf", &SallenAndKey::hpf)
}};

SallenAndKey::ParamType SallenAndKey::sParameters
{{
    Param("freq", 500.0, Min(0.0)),
    Param("reson", 500.0, Clip(0.0, 1.0))
}};

void SallenAndKey::reset()
{
    v1 = 0.0;
    v2 = 0.0;
    ic1eq = 0.0;
    ic2eq = 0.0;
}

void SallenAndKey::updateCoefficients(double freq, double reson, double samplingRate)
{
    const double g = tan(M_PI * (freq / samplingRate));
    const double gp1 = 1.0 + g;
    
    k = 2.0 * reson;
    a0 = 1.0 / ((gp1 * gp1) - (g * k));
    a1 = k * a0;
    a2 = gp1 * a0;
    a3 = g * a2;
    a4 = 1.0 / gp1;
    a5 = g * a4;
}

double SallenAndKey::process(double x)
{
    v1 = (a1 * ic2eq) + (a2 * ic1eq) + (a3 * x);
    v2 = (a4 * ic2eq) + (a5 * v1);
    
    ic1eq = (2 * (v1 - (k * v2))) - ic1eq;
    ic2eq = (2 * v2) - ic2eq;
    
    return 0.0;
}

double SallenAndKey::hpf(double x)
{
    return x - v2;
}

double SallenAndKey::bpf(double x)
{
    return v1 - v2;
}

double SallenAndKey::lpf(double x)
{
    return v2;
}
