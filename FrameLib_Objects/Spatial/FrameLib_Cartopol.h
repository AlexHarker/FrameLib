
#ifndef FRAMELIB_CARTOPOL_H
#define FRAMELIB_CARTOPOL_H

#include "FrameLib_Spatial_Convertor_Template.h"

namespace FrameLib_Spatial_Ops
{
    struct CarToPol
    {
        Value2D operator()(const Value2D& v)
        {
            return Value2D(abs(v), arg(v) * mAngleFactor);
        }
        
        Value3D operator()(const Value3D& v)
        {
            const double x = std::get<0>(v);
            const double y = std::get<1>(v);
            const double z = std::get<2>(v);
            
            const double radius = sqrt(x * x + y * y + z * z);
            double azimuth = atan2(x, y) * mAngleFactor;
            double elevation = radius ? asin(z / radius) * mAngleFactor : 0.0;
            
            return Value3D(radius, azimuth, elevation);
        }
        
        void prepare(bool degrees)
        {
            mAngleFactor = degrees ? 180.0 / M_PI : 1.0;
        }
        
        double mAngleFactor = 1.0;
    };
}

// Info Specialisations

template<>
inline std::string FrameLib_Spatial_Convertor<FrameLib_Spatial_Ops::CarToPol>::objectInfo(bool verbose)
{
    return formatInfo("Converts from cartesian values at the inputs to polar values at the outputs: "
                      "Conversion may be in 2D or 3D (using spherical coordinates). "
                      "The outputs are frames of the same length as the longest input. "
                      "Shorter inputs are padded with zeros prior to conversion.",
                      "Converts from cartesian values at the inputs to polar values at the outputs.", verbose);
}

template<>
inline std::string FrameLib_Spatial_Convertor<FrameLib_Spatial_Ops::CarToPol>::inputInfo(unsigned long idx, bool verbose)
{
    switch (idx)
    {
        case 0:     return m2D ? "X (Real) Values" : "X Values";
        case 1:     return m2D ? formatInfo("Y (Imaginary) Values", "Y (Imag) Values", verbose) : "Y Values";
        default:    return "Z Values";
    }
}

template<>
inline std::string FrameLib_Spatial_Convertor<FrameLib_Spatial_Ops::CarToPol>::outputInfo(unsigned long idx, bool verbose)
{
    switch (idx)
    {
        case 0:     return "Magnitudes (r)";
        case 1:     return m2D ? "Phase Angles (theta)" : "Azimuth Angles (theta)";
        default:    return "Elevation Angles (phi)";
    }
}

using FrameLib_Cartopol = FrameLib_Spatial_Convertor<FrameLib_Spatial_Ops::CarToPol>;

#endif
