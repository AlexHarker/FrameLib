
#include "FrameLib_SVF.h"

using namespace FrameLib_Filters;

constexpr SVF::ParamType SVF::sParameters;
constexpr SVF::ModeType SVF::sModes;

// Filter Implementation

void SVF::operator()(double x)
{
    // Compute all modes
    
    hp = (x - (2.0 * r * s1) - (g * s1) - s2) / (1.0 + (2.0 * r * g) + (g * g));
    bp = g * hp + s1;
    lp = g * bp + s2;
    
    // Update state
    
    s1 = g * hp + bp;
    s2 = g * bp + lp;
}

double SVF::hpf(double x)
{
    return hp;
}

double SVF::bpf(double x)
{
    return bp;
}

double SVF::lpf(double x)
{
    return lp;
}

void SVF::reset()
{
    s1 = 0.0;
    s2 = 0.0;
    lp = 0.0;
    bp = 0.0;
    hp = 0.0;
}

void SVF::updateCoefficients(double freq, double reson, double samplingRate)
{
    g = tan(freq * pi() / samplingRate);
    r = std::min(std::max(1.0 - reson, 0.005), 1.0);
}
