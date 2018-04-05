
#ifndef FRAMELIB_POLTOCAR_H
#define FRAMELIB_POLTOCAR_H

#include "FrameLib_Complex_Unary_Template.h"

struct PolToCar
{
    std::complex<double> operator()(const std::complex<double> &x) { return std::polar<double>(x.real(), x.imag()); }
};

class FrameLib_Poltocar : public FrameLib_Complex_UnaryOp<PolToCar>
{
    
public:

    // Constructor
    
    FrameLib_Poltocar(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy) : FrameLib_Complex_UnaryOp(context, serialisedParameters, proxy) {}
    
    // Info
    
    std::string objectInfo(bool verbose)
    {
        return formatInfo("Converts from polar values at the input to cartesian values at output: The result is two frames of the same size as the longer of the two inputs. If one input is shorter than the other it is padded with zeros to the size of the other before conversion.",
                          "Converts from polar values at the input to cartesian values at output.", verbose);
    }
    
    std::string inputInfo(unsigned long idx, bool verbose)
    {
        if (idx == 0)
            return formatInfo("Amplitudes (or abs values)", "Amplitudes", verbose);
        else
            return formatInfo("Phases (or args)", "Phases", verbose);
    }
    
    std::string outputInfo(unsigned long idx, bool verbose)
    {
        if (idx == 0)
            return formatInfo("Real or X Output", "Real or X Output", verbose);
        else
            return formatInfo("Imaginary or Y Output", "Imag or Y Output", verbose);
    }
};

#endif
