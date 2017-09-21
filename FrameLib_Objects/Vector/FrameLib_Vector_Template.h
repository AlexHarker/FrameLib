
#ifndef FRAMELIB_VECTOR_TEMPLATE_H
#define FRAMELIB_VECTOR_TEMPLATE_H

#include "FrameLib_DSP.h"
#include <functional>

template <double func(double *, unsigned long) > class FrameLib_Vector : public FrameLib_Processor
{
    
public:
    
    // Constructor
    
    FrameLib_Vector(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner) : FrameLib_Processor(context, NULL, 1, 1) {}
    
    // Info
    
    std::string objectInfo(bool verbose)
    {
        return formatInfo("Calculates the # of the input frame: The result is a single value.",
                       "Calculates the # of the input frame.", getOpString(), verbose);
    }
    
    std::string inputInfo(unsigned long idx, bool verbose)      { return "Input"; }
    std::string outputInfo(unsigned long idx, bool verbose)     { return "Result"; }

protected:
    
    // Process
    
    void process()
    {
        unsigned long sizeIn, sizeOut;
        double *input = getInput(0, &sizeIn);
        
        // FIX - what is the operation if the input is empty?
        
        requestOutputSize(0, sizeIn ? 1 : 0);
        allocateOutputs();
        
        double *output = getOutput(0, &sizeOut);
        
        if (sizeOut)
            output[0] = func(input, sizeIn);
    }
    
    // Operator description (specialise to change description)

    const char *getOpString() { return "<vector operation>"; }
};

#endif
