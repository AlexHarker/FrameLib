
#include "FrameLib_Resonant.h"

constexpr Resonant::ParamType Resonant::sParameters;
constexpr Resonant::ModeType Resonant::sModes;

// Filter Implementation

double Resonant::lpf(double x)
{
    x = x * ((scl + r2) + 1.0);
    double y = x - ((scl * y1) + (r2 * y2));
    
    y2 = y1;
    y1 = y;
    
    return y;
}

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
