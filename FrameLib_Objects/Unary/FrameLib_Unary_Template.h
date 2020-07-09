
#ifndef FRAMELIB_UNARY_TEMPLATE_H
#define FRAMELIB_UNARY_TEMPLATE_H

#include "FrameLib_DSP.h"

// Unary (Operator Version)

template <typename Op>
class FrameLib_UnaryOp final : public FrameLib_Processor
{
public:
    
    // Constructor
    
    FrameLib_UnaryOp(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy)
    : FrameLib_Processor(context, proxy, nullptr, 1, 1)
    {
        mParameters.set(serialisedParameters);
    }
    
    // Info
    
    std::string objectInfo(bool verbose) override
    {
        return formatInfo("Calculates the # of each value in the input frame: "
                          "The output is a frame of the same length as the input.",
                          "Calculates the # of each value in the input frame.", getOpString(), verbose);
    }

    std::string inputInfo(unsigned long idx, bool verbose) override     { return "Input"; }
    std::string outputInfo(unsigned long idx, bool verbose) override    { return "Output"; }

private:
    
    // Process
    
    void process() override
    {
        Op op;
        unsigned long size;
        const double *input = getInput(0, &size);
        
        requestOutputSize(0, size);
        allocateOutputs();

        double *output = getOutput(0, &size);
        
        for (unsigned long i = 0; i < size; i++)
            output[i] = op(input[i]);
    }
    
    // Description (specialise to change description)

    const char *getOpString() { return "<unary operation>"; }
};

// Unary Functor

template <double func(double)>
struct Unary_Functor
{
    double operator()(double x) { return func(x); }
};

// Unary (Function Version)

template <double func(double)>
using FrameLib_Unary = FrameLib_UnaryOp<Unary_Functor<func>>;

#endif
