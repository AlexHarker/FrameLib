
#ifndef FRAMELIB_POLTOCAR_H
#define FRAMELIB_POLTOCAR_H

#include "FrameLib_Spatial_Convertor_Template.h"

namespace FrameLib_Spatial_Ops
{
    struct PolToCar
    {
        Value2D operator()(const Value2D& v)
        {
            return std::polar<double>(v.real(), v.imag() * mAngleFactor);
        }
        
        Value3D operator()(const Value3D& v)
        {
            const double radius = std::get<0>(v);
            const double psi = std::get<1>(v) * mAngleFactor;
            const double theta = mAngleFactor * (mAngleOffset - std::get<2>(v));
        
            const double x = radius * sin(theta) * sin(psi);
            const double y = radius * sin(theta) * cos(psi);
            const double z = radius * cos(theta);
            
            return Value3D(x, y, z);
        }
        
        void prepare(bool degrees)
        {
            mAngleFactor = degrees ? M_PI / 180.0 : 1.0;
            mAngleOffset = degrees ? 90.0 : M_PI / 2.0;
        }
        
        double mAngleOffset = M_PI / 2.0;
        double mAngleFactor = 1.0;
    };
}

// Info Specialisations

template<>
inline std::string FrameLib_Spatial_Convertor<FrameLib_Spatial_Ops::PolToCar>::objectInfo(bool verbose)
{
    return formatInfo("Converts from polar values at the input to cartesian values at output: The outputs are frames of the same length as the longer of the two inputs. If one input is shorter than the other then it is padded with zeros to the length of the other before conversion.",
                      "Converts from polar values at the input to cartesian values at output.", verbose);
}

template<>
inline std::string FrameLib_Spatial_Convertor<FrameLib_Spatial_Ops::PolToCar>::inputInfo(unsigned long idx, bool verbose)
{
    if (idx == 0)
        return formatInfo("Amplitudes (or abs values)", "Amplitudes", verbose);
    else
        return formatInfo("Phases (or arg values)", "Phases", verbose);
}

template<>
inline std::string FrameLib_Spatial_Convertor<FrameLib_Spatial_Ops::PolToCar>::outputInfo(unsigned long idx, bool verbose)
{
    if (idx == 0)
        return formatInfo("Real or X Output", "Real or X Output", verbose);
    else
        return formatInfo("Imaginary or Y Output", "Imag or Y Output", verbose);
}

using FrameLib_Poltocar = FrameLib_Spatial_Convertor<FrameLib_Spatial_Ops::PolToCar>;

#endif
