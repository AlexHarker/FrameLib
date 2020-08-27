
#ifndef FRAMELIB_COMPLEX_UNARY_TEMPLATE_H
#define FRAMELIB_COMPLEX_UNARY_TEMPLATE_H

#include "FrameLib_DSP.h"

#include <complex>

// Complex Unary (Operator Version)

template <typename Op>
class FrameLib_Complex_UnaryOp final : public FrameLib_Processor
{
public:
    
    // Constructor
    
    FrameLib_Complex_UnaryOp(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy)
    : FrameLib_Processor(context, proxy, nullptr, 2, 2)
    {
        mParameters.set(serialisedParameters);
    }
    
    // Info
    
    std::string objectInfo(bool verbose) override
    {
        return formatInfo("Calculates the # of each complex value at the inputs: "
                          "Both input and output are split into real and imaginary parts. "
                          "The outputs are frames of the same length as the longer of the two inputs. "
                          "If inputs are mismatched then the shorter input is padded with zeros.",
                          "Calculates the # of each complex value at the inputs.", getOpString(), verbose);
    }

    std::string inputInfo(unsigned long idx, bool verbose) override
    {
        if (idx == 0)
            return formatInfo("Real Input", "Real Input", verbose);
        else
            return formatInfo("Imaginary Input", "Imag Input", verbose);
    }
    
    std::string outputInfo(unsigned long idx, bool verbose) override
    {
        if (idx == 0)
            return formatInfo("Real Result", "Real Result", verbose);
        else
            return formatInfo("Imaginary Result", "Imag Result", verbose);
    }

private:
    
    // Calculate Split
    
    void calculate(double &rOut, double &iOut, const double &rIn, const double &iIn, Op &op)
    {
        std::complex<double> result = op(std::complex<double>(rIn, iIn));
        rOut = result.real();
        iOut = result.imag();
    }
    
    // Process
    
    void process() override
    {
        Op op;
        unsigned long size, sizeInR, sizeInI;
        const double *inputR = getInput(0, &sizeInR);
        const double *inputI = getInput(1, &sizeInI);

        size = std::max(sizeInR, sizeInI);
        requestOutputSize(0, size);
        requestOutputSize(1, size);
        
        if (allocateOutputs())
        {
            double *outputR = getOutput(0, &size);
            double *outputI = getOutput(1, &size);

            unsigned long sizeCommon = std::min(sizeInR, sizeInI);
            
            for (unsigned long i = 0; i < sizeCommon; i++)
                calculate(outputR[i], outputI[i], inputR[i], inputI[i], op);
            
            if (sizeInR > sizeInI)
            {
                for (unsigned long i = sizeCommon; i < size; i++)
                    calculate(outputR[i], outputI[i], inputR[i], 0.0, op);
            }
            else
            {
                for (unsigned long i = sizeCommon; i < size; i++)
                    calculate(outputR[i], outputI[i], 0.0, inputI[i], op);
            }
        }
    }
    
    // Description (specialise to change description)

    const char *getOpString() { return "<unary operation>"; }
};

// Complex Unary Functor

template <std::complex<double> func(const std::complex<double> &)>
struct Complex_Unary_Functor
{
    std::complex<double> operator()(std::complex<double> x) { return func(x); }
};

// Complex Unary (Function Version)

template <std::complex<double> func(const std::complex<double> &)>
using FrameLib_Complex_Unary = FrameLib_Complex_UnaryOp<Complex_Unary_Functor<func>>;

#endif
