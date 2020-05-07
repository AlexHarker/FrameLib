
#include "FrameLib_OnePole.h"

using namespace FrameLib_Filters;

constexpr OnePole::ParamType OnePole::sParameters;
constexpr OnePole::ModeType OnePole::sModes;

// Filter Implementation

void OnePole::operator()(double x)
{
    const double z = (x - ((f0 - 1.0) * z1)) * (1.0 / (f0 + 1.0));
    
    lp = (z + z1) * f0;
    hp = z - z1;
    
    z1 = z;
}

double OnePole::hpf(double x)
{
    return hp;
}

double OnePole::lpf(double x)
{
    return lp;
}

void OnePole::reset()
{
    lp = 0.0;
    hp = 0.0;
    z1 = 0.0;
}

void OnePole::updateCoefficients(double freq, double samplingRate)
{
    f0 = tan(freq * (pi() / samplingRate));
}
