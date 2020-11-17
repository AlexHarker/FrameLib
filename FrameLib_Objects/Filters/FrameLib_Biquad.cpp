
#include "FrameLib_Biquad.h"

using namespace FrameLib_Filters;

constexpr Biquad::Description Biquad::sDescription;
constexpr Biquad::ParamType Biquad::sParameters;
constexpr Biquad::ModeType Biquad::sModes;

// Filter Implementation

double Biquad::process(double x)
{
    const double y = (x * b0) + (x1 * b1) + (x2 * b2) - (y1 * a1) - (y2 * a2);
    
    x2 = x1;
    x1 = x;
    y2 = y1;
    y1 = y;
    
    return y;
}

void Biquad::reset()
{
    x1 = 0.0;
    x2 = 0.0;
    y1 = 0.0;
    y2 = 0.0;
}

void Biquad::updateCoefficients(double A1, double A2, double B0, double B1, double B2, double)
{
    a1 = A1;
    a2 = A2;
    b0 = B0;
    b1 = B1;
    b2 = B2;
}
