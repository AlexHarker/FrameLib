
#include "FrameLib_BiquadCoeff.h"

using namespace FrameLib_Filters;

constexpr BiquadCoeff::Description BiquadCoeff::sDescription;
constexpr BiquadCoeff::ParamType BiquadCoeff::sParameters;
constexpr BiquadCoeff::CoeffType BiquadCoeff::sCoefficients;
constexpr BiquadCoeff::ModeType BiquadCoeff::sModes;

// Common intermediate values

BiquadCoeff::Common::Common(double f0, double sr, double Q)
{
    const double w0 = twopi() * f0 / sr;
    cosw0 = cos(w0);
    alpha = sin(w0) / (2.0 * Q);
}

BiquadCoeff::ShelfCommon::ShelfCommon(double f0, double sr, double gain, double S)
{
    const double w0 = twopi() * f0 / sr;
    const double cosw0 = cos(w0);
 
    A = sqrt(gain);
    alphaFactor = sin(w0) * sqrt((gain + 1.0) * (1.0 / S - 1.0) + 2.0 * A);
    Am1 = (A - 1.0);
    Ap1 = (A + 1.0);
    Am1cosw0 = Am1 * cosw0;
    Ap1cosw0 = Ap1 * cosw0;
}

// Filter Implementation

void BiquadCoeff::lpf(double f0, double gain, double Q, double sr, coeff& a1, coeff& a2, coeff& b0, coeff& b1, coeff& b2)
{
    const Common c(f0, sr, Q);
    
    const double aR = 1.0 / (1.0 + c.alpha);
    const double cosw0aR = aR * c.cosw0;

    b0 = (aR - cosw0aR) / 2.0;
    b1 = (aR - cosw0aR);
    b2 = b0;
    a1 = (-2.0 * cosw0aR);
    a2 = aR * (1.0 - c.alpha);
}

void BiquadCoeff::bpf(double f0, double gain, double Q, double sr, coeff& a1, coeff& a2, coeff& b0, coeff& b1, coeff& b2)
{
    const Common c(f0, sr, Q);

    const double aR = 1.0 / (1.0 + c.alpha);
    
    b0 = aR * c.alpha;
    b1 = 0.0;
    b2 = -b0;
    a1 = -(2.0 * aR * c.cosw0);
    a2 = aR - b0;
}

void BiquadCoeff::hpf(double f0, double gain, double Q, double sr, coeff& a1, coeff& a2, coeff& b0, coeff& b1, coeff& b2)
{
    const Common c(f0, sr, Q);
    
    const double aR = 1.0 / (1.0 + c.alpha);
    const double cosw0aR = aR * c.cosw0;
    
    b0 =  (aR + cosw0aR) / 2.0;
    b1 = -(aR + cosw0aR);
    b2 = b0;
    a1 = -(2.0 * cosw0aR);
    a2 = aR * (1.0 - c.alpha);
}

void BiquadCoeff::bell(double f0, double gain, double Q, double sr, coeff& a1, coeff& a2, coeff& b0, coeff& b1, coeff& b2)
{
    const Common c(f0, sr, Q);
    
    const double A = sqrt(gain);
    const double alphaMA = c.alpha * A;
    const double alphaDA = c.alpha / A;
    const double aR =  1.0 / (1.0 + alphaDA);

    b0 = aR *  (1.0 + alphaMA);
    b1 = aR * -(2.0 * c.cosw0);
    b2 = aR *  (1.0 - alphaMA);
    a1 = b1;
    a2 = aR *  (1.0 - alphaDA);
}

void BiquadCoeff::resonant(double f0, double gain, double Q, double sr, coeff& a1, coeff& a2, coeff& b0, coeff& b1, coeff& b2)
{
    const Common c(f0, sr, Q);
    
    const double aR =  1.0 / (1.0 + c.alpha);
    
    b0 = aR * c.alpha * Q;
    b1 = 0.0;
    b2 = -b0;
    a1 = aR * c.cosw0 * -2.0;
    a2 = aR * (1.0 - c.alpha);
}

void BiquadCoeff::notch(double f0, double gain, double Q, double sr, coeff& a1, coeff& a2, coeff& b0, coeff& b1, coeff& b2)
{
    const Common c(f0, sr, Q);

    const double aR = 1.0 / (1.0 + c.alpha);
    
    b0 = aR;
    b1 = aR * (-2.0 * c.cosw0);
    b2 = aR;
    a1 = b1;
    a2 = aR * (1.0 - c.alpha);
}

void BiquadCoeff::lowshelf(double f0, double gain, double S, double sr, coeff& a1, coeff& a2, coeff& b0, coeff& b1, coeff& b2)
{
    const ShelfCommon c(f0, sr, gain, S);

    const double aR = 1.0 / (c.Ap1 + c.Am1cosw0 + c.alphaFactor);
    const double AaR = c.A * aR;
    
    b0 = AaR * (c.Ap1 - c.Am1cosw0 + c.alphaFactor);
    b1 = AaR * (c.Am1 - c.Ap1cosw0) * 2.0;
    b2 = AaR * (c.Ap1 - c.Am1cosw0 - c.alphaFactor);
    a1 =  aR * (c.Am1 + c.Ap1cosw0) * -2.0;
    a2 =  aR * (c.Ap1 + c.Am1cosw0 - c.alphaFactor);
}

void BiquadCoeff::highshelf(double f0, double gain, double S, double sr, coeff& a1, coeff& a2, coeff& b0, coeff& b1, coeff& b2)
{
    const ShelfCommon c(f0, sr, gain, S);
    
    const double aR = 1.0 / (c.Ap1 - c.Am1cosw0 + c.alphaFactor);
    const double AaR = c.A * aR;

    b0 = AaR * (c.Ap1 + c.Am1cosw0 + c.alphaFactor);
    b1 = AaR * (c.Am1 + c.Ap1cosw0) * -2.0;
    b2 = AaR * (c.Ap1 + c.Am1cosw0 - c.alphaFactor);
    a1 =  aR * (c.Am1 - c.Ap1cosw0) * 2.0;
    a2 =  aR * (c.Ap1 - c.Am1cosw0 - c.alphaFactor);
}
