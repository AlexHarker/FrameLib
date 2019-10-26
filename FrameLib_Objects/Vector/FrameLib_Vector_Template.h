
#ifndef FRAMELIB_VECTOR_TEMPLATE_H
#define FRAMELIB_VECTOR_TEMPLATE_H

#include "FrameLib_DSP.h"

template <double func(const double *, size_t), bool calcZeroLength = false>
class FrameLib_Vector final : public FrameLib_Processor
{
    
public:
    
    // Constructor
    
    FrameLib_Vector(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy) : FrameLib_Processor(context, proxy, nullptr, 1, 1) {}
    
    // Info
    
    std::string objectInfo(bool verbose) override
    {
        return formatInfo("Calculates the # of the input frame: The output is a single value.",
                       "Calculates the # of the input frame.", getOpString(), verbose);
    }
    
    std::string inputInfo(unsigned long idx, bool verbose) override     { return "Input"; }
    std::string outputInfo(unsigned long idx, bool verbose) override    { return "Result"; }

private:
    
    // Process
    
    void process() override
    {
        unsigned long sizeIn, sizeOut;
        const double *input = getInput(0, &sizeIn);
        
        // FIX - what is the operation if the input is empty?
        
        if (calcZeroLength)
            requestOutputSize(0, 1);
        else
            requestOutputSize(0, sizeIn ? 1 : 0);
        allocateOutputs();
        
        double *output = getOutput(0, &sizeOut);
        
        if (sizeOut)
            output[0] = func(input, sizeIn);
    }
    
    // Operator Description (specialise to change description)

    const char *getOpString() { return "<vector operation>"; }
};

#endif
