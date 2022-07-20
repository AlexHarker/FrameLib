
#ifndef FRAMELIB_POLTOCAR_H
#define FRAMELIB_POLTOCAR_H

#include "FrameLib_Spatial_Convertor_Template.h"

namespace FrameLib_Spatial_Ops
{
    struct PolToCar
    {
        PolToCar(bool degrees)
        : mAngleFactor(degrees ? M_PI / 180.0 : 1.0)
        , mAngleOffset(degrees ? 90.0 : M_PI / 2.0)
        {}
        
        Value2D operator()(const Value2D& v)
        {
            return std::polar<double>(std::max(0.0, v.real()), v.imag() * mAngleFactor);
        }
        
        Value3D operator()(const Value3D& v)
        {
            const double radius = std::max(0.0, std::get<0>(v));
            const double theta = std::get<1>(v) * mAngleFactor;
            const double psi = mAngleFactor * (mAngleOffset - std::get<2>(v));
        
            const double x = radius * sin(psi) * cos(theta);
            const double y = radius * sin(psi) * sin(theta);
            const double z = radius * cos(psi);
            
            return Value3D(x, y, z);
        }
        
        double mAngleFactor;
        double mAngleOffset;
    };
}

// Info Specialisations

template<>
inline std::string FrameLib_Spatial_Convertor<FrameLib_Spatial_Ops::PolToCar>::objectInfo(bool verbose)
{
    return formatInfo("Converts from polar values at the inputs to cartesian values at the outputs: "
                      "Conversion may be in 2D or 3D (using spherical coordinates). "
                      "The outputs are frames of the same length as the longest input. "
                      "Shorter inputs are padded with zeros prior to conversion.",
                      "Converts from polar values at the inputs to cartesian values at the outputs.", verbose);
}

template<>
inline std::string FrameLib_Spatial_Convertor<FrameLib_Spatial_Ops::PolToCar>::inputInfo(unsigned long idx, bool verbose)
{
    switch (idx)
    {
        case 0:     return "Magnitudes (r)";
        case 1:     return m2D ? "Phase Angles (theta)" : "Azimuth Angles (theta)";
        default:    return "Elevation Angles (phi)";
    }
}

template<>
inline std::string FrameLib_Spatial_Convertor<FrameLib_Spatial_Ops::PolToCar>::outputInfo(unsigned long idx, bool verbose)
{
    switch (idx)
    {
        case 0:     return m2D ? "X (Real) Values" : "X Values";
        case 1:     return m2D ? formatInfo("Y (Imaginary) Values", "Y (Imag) Values", verbose) : "Y Values";
        default:    return "Z Values";
    }
}

using FrameLib_Poltocar = FrameLib_Spatial_Convertor<FrameLib_Spatial_Ops::PolToCar>;

#endif
