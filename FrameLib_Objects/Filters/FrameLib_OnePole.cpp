
#include "FrameLib_OnePole.h"

constexpr OnePole::ParamType OnePole::sParameters;
constexpr OnePole::ModeType OnePole::sModes;

// Filter Implementation

void OnePole::operator()(double x)
{
    double y = y1 + f0 * (x - y1);
    
    y1 = y;
}

double OnePole::hpf(double x)
{
    return x - y1;
}

double OnePole::lpf(double x)
{
    return y1;
}

void OnePole::reset()
{
    y1 = 0.0;
}

void OnePole::updateCoefficients(double freq, double samplingRate)
{
    f0 = sin((freq * twopi()) / samplingRate);
}
