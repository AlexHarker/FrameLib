
#include "FrameLib_AccumPoint.h"
#include "../../FrameLib_Dependencies/Statistics.hpp"

// Constructor

FrameLib_AccumPoint::FrameLib_AccumPoint(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy) : FrameLib_Processor(context, proxy, &sParamInfo, 2, 1)
{
    mParameters.addDouble(kPercentage, "percentage", 50.0, 0);
    mParameters.setClip(0.0, 100.0);
    
    mParameters.set(serialisedParameters);
    
    setParameterInput(1);
}

// Info

std::string FrameLib_AccumPoint::objectInfo(bool verbose)
{
    return formatInfo("Calculates the point in a vector at which the sum reaches a given percentage of the total sum: The output is a single value.",
                   "Calculates the point in a vector at which the sum reaches a given percentage of the total sum.", verbose);
}

std::string FrameLib_AccumPoint::inputInfo(unsigned long idx, bool verbose)
{
    if (idx)
        return parameterInputInfo(verbose);
    else
        return "Input Frame";
}

std::string FrameLib_AccumPoint::outputInfo(unsigned long idx, bool verbose)
{
    return "Percentile Output Point";
}

// Parameter Info

FrameLib_AccumPoint::ParameterInfo FrameLib_AccumPoint::sParamInfo;

FrameLib_AccumPoint::ParameterInfo::ParameterInfo()
{
    add("Sets the percentile of the accumlation for the calculation [0-100].");
}

// Process

void FrameLib_AccumPoint::process()
{
    unsigned long sizeIn, sizeOut;
    const double *input = getInput(0, &sizeIn);
    
    requestOutputSize(0, sizeIn ? 1 : 0);
    allocateOutputs();
    
    double *output = getOutput(0, &sizeOut);
    
    if (sizeOut)
        output[0] = statPDFPercentile(input, mParameters.getValue(kPercentage), sizeIn);
}
