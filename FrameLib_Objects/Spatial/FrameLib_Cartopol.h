
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
    return formatInfo("Converts from cartesian values at the input to polar values at output: The outputs are frames of the same length as the longer of the two inputs. If one input is shorter than the other then it is padded with zeros to the length of the other before conversion.",
                      "Converts from cartesian values at the input to polar values at output.", verbose);
}

template<>
inline std::string FrameLib_Spatial_Convertor<FrameLib_Spatial_Ops::CarToPol>::inputInfo(unsigned long idx, bool verbose)
{
    if (idx == 0)
        return formatInfo("Real or X Input", "Real or X Input", verbose);
    else
        return formatInfo("Imaginary or Y Input", "Imag or Y Input", verbose);
}

template<>
inline std::string FrameLib_Spatial_Convertor<FrameLib_Spatial_Ops::CarToPol>::outputInfo(unsigned long idx, bool verbose)
{
    if (idx == 0)
        return formatInfo("Amplitudes (or abs values)", "Amplitudes", verbose);
    else
        return formatInfo("Phases (or arg values)", "Phases", verbose);
}

using FrameLib_Cartopol = FrameLib_Spatial_Convertor<FrameLib_Spatial_Ops::CarToPol>;

#endif
