
#include "FrameLib_SallenAndKey.h"

using namespace FrameLib_Filters;

constexpr SallenAndKey::Description SallenAndKey::sDescription;
constexpr SallenAndKey::ParamType SallenAndKey::sParameters;
constexpr SallenAndKey::ModeType SallenAndKey::sModes;

// Filter Implementation

void SallenAndKey::operator()(double x)
{
    // Compute outputs
    
    v1 = (a1 * ic2eq) + (a2 * ic1eq) + (a3 * x);
    v2 = (a4 * ic2eq) + (a5 * v1);
    
    // Update state
    
    ic1eq = (2 * (v1 - (k * v2))) - ic1eq;
    ic2eq = (2 * v2) - ic2eq;
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

void SallenAndKey::reset()
{
    v1 = 0.0;
    v2 = 0.0;
    ic1eq = 0.0;
    ic2eq = 0.0;
}

void SallenAndKey::updateCoefficients(double freq, double reson, double samplingRate)
{
    const double g = tan(freq * pi() / samplingRate);
    const double gp1 = 1.0 + g;
    
    k = 2.0 * reson;
    a0 = 1.0 / ((gp1 * gp1) - (g * k));
    a1 = k * a0;
    a2 = gp1 * a0;
    a3 = g * a2;
    a4 = 1.0 / gp1;
    a5 = g * a4;
}
