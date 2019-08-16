
#ifndef FRAMELIB_CARTOPOL_H
#define FRAMELIB_CARTOPOL_H

#include "FrameLib_Complex_Unary_Template.h"

struct CarToPol
{
    std::complex<double> operator()(const std::complex<double> &x) { return std::complex<double>(abs(x), arg(x)); }
};

// Info Specialisations

template<>
inline std::string FrameLib_Complex_UnaryOp<CarToPol>::objectInfo(bool verbose)
{
    return formatInfo("Converts from cartesian values at the input to polar values at output: The outputs are frames of the same length as the longer of the two inputs. If one input is shorter than the other it is padded with zeros to the length of the other before conversion.",
                      "Converts from cartesian values at the input to polar values at output.", verbose);
}

template<>
inline std::string FrameLib_Complex_UnaryOp<CarToPol>::inputInfo(unsigned long idx, bool verbose)
{
    if (idx == 0)
        return formatInfo("Real or X Input", "Real or X Input", verbose);
    else
        return formatInfo("Imaginary or Y Input", "Imag or Y Input", verbose);
}

template<>
inline std::string FrameLib_Complex_UnaryOp<CarToPol>::outputInfo(unsigned long idx, bool verbose)
{
    if (idx == 0)
        return formatInfo("Amplitudes (or abs values)", "Amplitudes", verbose);
    else
        return formatInfo("Phases (or args)", "Phases", verbose);
}

using FrameLib_Cartopol = FrameLib_Complex_UnaryOp<CarToPol>;

#endif
