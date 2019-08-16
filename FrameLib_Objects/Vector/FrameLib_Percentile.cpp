
#include "FrameLib_Percentile.h"
#include "FrameLib_Sort_Functions.h"

// Constructor

FrameLib_Percentile::FrameLib_Percentile(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy) : FrameLib_Processor(context, proxy, &sParamInfo, 2, 1)
{
    mParameters.addDouble(kPercentile, "percentile", 50.0, 0);
    mParameters.setClip(0.0, 100.0);
    
    mParameters.set(serialisedParameters);
    
    setParameterInput(1);
}

// Info

std::string FrameLib_Percentile::objectInfo(bool verbose)
{
    return formatInfo("Calculates any percentile (for example the median at 50%) of an input frame: The output is a single value.",
                   "Calculates any percentile (for example the median at 50%) of an input frame.", verbose);
}

std::string FrameLib_Percentile::inputInfo(unsigned long idx, bool verbose)
{
    if (idx)
        return parameterInputInfo(verbose);
    else
        return "Input Frame";
}

std::string FrameLib_Percentile::outputInfo(unsigned long idx, bool verbose)
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
    const double *input = getInput(0, &sizeIn);
    
    requestOutputSize(0, sizeIn ? 1 : 0);
    allocateOutputs();
    
    double *output = getOutput(0, &sizeOut);
    double *temp = alloc<double>(sizeIn + 1);
    
    if (sizeOut && temp)
    {
        sortAscending(temp, input, sizeIn);
        
        // Copy last value
        
        temp[sizeIn] = temp[sizeIn - 1];
        
        // Linearly interpolate output
        
        double position = (mParameters.getValue(kPercentile) * (sizeIn - 1) / 100.0);
        unsigned long idx = truncToUInt(position);
        double fract = position - idx;
        
        output[0] = temp[idx] + fract * (temp[idx + 1] - temp[idx]);
    }
    
    dealloc(temp);
}
