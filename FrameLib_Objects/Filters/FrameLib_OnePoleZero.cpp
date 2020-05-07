
#include "FrameLib_OnePoleZero.h"

using namespace FrameLib_Filters;

constexpr OnePoleZero::ParamType OnePoleZero::sParameters;
constexpr OnePoleZero::ModeType OnePoleZero::sModes;

// Filter Implementation

void OnePoleZero::operator()(double x)
{
    const double w = x * a0;
    const double y = r1 + w;
    
    y1 = y;
    r1 = w + (y * a1);
}

double OnePoleZero::hpf(double x)
{
    return x - y1;
}

double OnePoleZero::lpf(double x)
{
    return y1;
}

void OnePoleZero::reset()
{
    y1 = 0.0;
    r1 = 0.0;
}

void OnePoleZero::updateCoefficients(double freq, double samplingRate)
{
    const double fc = freq * pi() / (samplingRate * 2.0);
    
    a0 = (2.0 * sin(fc)) / (cos(fc) + sin(fc));
    a1 = 1.0 - (a0 * 2.0);
}
