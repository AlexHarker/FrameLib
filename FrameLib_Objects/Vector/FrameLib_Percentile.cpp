
#include "FrameLib_Percentile.h"
#include "FrameLib_Sort_Functions.h"

// Constructor

FrameLib_Percentile::FrameLib_Percentile(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner) : FrameLib_Processor(context, 1, 1)
{
    mParameters.addDouble(kPercentile, "percentile", 50.0, 0);
    mParameters.setClip(0.0, 100.0);
    
    mParameters.setInfo(&sParamInfo);

    mParameters.set(serialisedParameters);
    
    mPercentile = mParameters.getValue(kPercentile);
}

// Info

const char *FrameLib_Percentile::objectInfo(bool verbose)
{
    return getInfo("Calculates any percentile (e.g. the median at 50%) of an input frame: The output is a single value.",
                   "Calculates any percentile (e.g. the median at 50%) of an input frame.", verbose);
}

const char *FrameLib_Percentile::inputInfo(unsigned long idx, bool verbose)
{
    return "Input Frame";
}

const char *FrameLib_Percentile::outputInfo(unsigned long idx, bool verbose)
{
    return "Percentile Output Value";
}

// Parameter Info

FrameLib_Percentile::ParameterInfo FrameLib_Percentile::sParamInfo;

FrameLib_Percentile::ParameterInfo::ParameterInfo()
{
    add("Sets the percentile to calculate [0-100].");
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
