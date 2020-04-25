
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

SallenAndKey::Coefficients SallenAndKey::calculateCoefficients(double freq, double reson, double samplingRate)
{
    Coefficients coeff;
    
    const double g = tan(M_PI * (freq / samplingRate));
    const double gp1 = 1.0 + g;
    
    coeff.k = 2.0 * reson;
    coeff.a0 = 1.0 / ((gp1 * gp1) - (g * coeff.k));
    coeff.a1 = coeff.k * coeff.a0;
    coeff.a2 = gp1 * coeff.a0;
    coeff.a3 = g * coeff.a2;
    coeff.a4 = 1.0 / gp1;
    coeff.a5 = g * coeff.a4;
    
    return coeff;
}

double SallenAndKey::process(double x, const Coefficients& coeff)
{
    v1 = (coeff.a1 * ic2eq) + (coeff.a2 * ic1eq) + (coeff.a3 * x);
    v2 = (coeff.a4 * ic2eq) + (coeff.a5 * v1);
    
    ic1eq = (2 * (v1 - (coeff.k * v2))) - ic1eq;
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
