
#include "FrameLib_Percentile.h"
#include "FrameLib_Sort_Functions.h"

// Constructor

FrameLib_Percentile::FrameLib_Percentile(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner) : FrameLib_Processor(context, 1, 1)
{
    mParameters.addDouble(kPercentile, "percentile", 50.0, 0);
    mParameters.setClip(0.0, 100.0);
    
    mParameters.set(serialisedParameters);
    
    mPercentile = mParameters.getValue(kPercentile);
}

// Process

void FrameLib_Percentile::process()
{
    unsigned long sizeIn, sizeOut;
    double *input = getInput(0, &sizeIn);
    
    requestOutputSize(0, sizeIn ? 1 : 0);
    allocateOutputs();
    
    double *output = getOutput(0, &sizeOut);
    double *temp = (double *) mAllocator->alloc(sizeof(double) * (sizeIn + 1));
    
    if (sizeOut && temp)
    {
        sortAscending(temp, input, sizeIn);
        
        // Copy last value
        
        temp[sizeIn] = temp[sizeIn - 1];
        
        // Linearly interpolate output
        
        double position = (mPercentile * (sizeIn - 1) / 100.0);
        unsigned long idx = position;
        double fract = position - idx;
        
        output[0] = input[idx] + fract * (input[idx + 1] - input[idx]);
    }
    
    mAllocator->dealloc(temp);
}
