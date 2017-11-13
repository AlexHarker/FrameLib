
#ifndef FRAMELIB_CARTOPOL_H
#define FRAMELIB_CARTOPOL_H

#include "FrameLib_Complex_Unary_Template.h"

struct CarToPol
{
    std::complex<double> operator()(const std::complex<double> &x) { return std::complex<double>(abs(x), arg(x)); }
};

class FrameLib_Cartopol : public FrameLib_Complex_UnaryOp<CarToPol>
{
    
public:

    // Constructor
    
    FrameLib_Cartopol(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner) : FrameLib_Complex_UnaryOp(context, serialisedParameters, owner) {}
    
    // Info
    
    std::string objectInfo(bool verbose)
    {
        return formatInfo("Converts from cartesian values at the input to polar values at output: The result is two frames of the same size as the longer of the two inputs. If one input is shorter than the other it is padded with zeros to the size of the other before conversion.",
                          "Converts from cartesian values at the input to polar values at output.", verbose);
    }
    
    std::string inputInfo(unsigned long idx, bool verbose)
    {
        if (idx == 0)
            return formatInfo("Real or X Input", "Real or X Input", verbose);
        else
            return formatInfo("Imaginary or Y Input", "Imag or Y Input", verbose);
    }
    
    std::string outputInfo(unsigned long idx, bool verbose)
    {
        if (idx == 0)
            return formatInfo("Amplitudes (or abs values)", "Amplitudes", verbose);
        else
            return formatInfo("Phases (or args)", "Phases", verbose);
    }
};

#endif
