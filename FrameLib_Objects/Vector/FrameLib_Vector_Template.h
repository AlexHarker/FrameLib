
#ifndef FRAMELIB_VECTOR_TEMPLATE_H
#define FRAMELIB_VECTOR_TEMPLATE_H

#include "FrameLib_DSP.h"
#include "FrameLib_Info.h"
#include <functional>

template <double func(double *, unsigned long) > class FrameLib_Vector : public FrameLib_Processor, private FrameLib_Info
{
    
public:
    
    // Constructor
    
    FrameLib_Vector(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner) : FrameLib_Processor(context, 1, 1) {}
    
    // Info
    
    const char *objectInfo(bool verbose)
    {
        return getInfo("Calculates the # of the input frame: The result is a single value.",
                       "Calculates the # of the input frame.", getOpString(), verbose);
    }
    
    const char *inputInfo(unsigned long idx, bool verbose)      { return "Input"; }
    const char *outputInfo(unsigned long idx, bool verbose)     { return "Result"; }

protected:
    
    // Process
    
    void process()
    {
        unsigned long sizeIn, sizeOut;
        
        requestOutputSize(0, 1);
        allocateOutputs();
        
        double *output = getOutput(0, &sizeOut);
        double *input = getInput(0, &sizeIn);
        
        if (sizeOut)
            output[0] = func(input, sizeIn);
    }
    
    // Operator description (specialise to change description)

    const char *getOpString() { return "<vector operation>"; }
};

#endif
