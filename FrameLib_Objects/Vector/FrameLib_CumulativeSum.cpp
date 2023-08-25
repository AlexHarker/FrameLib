
#include "FrameLib_CumulativeSum.h"

FrameLib_CumulativeSum::FrameLib_CumulativeSum(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy)
: FrameLib_Processor(context, proxy, &sParamInfo, 1, 1)
{
    mParameters.set(serialisedParameters);
}

std::string FrameLib_CumulativeSum::objectInfo(bool verbose)
{
    return formatInfo("Long Info: "
                      "Details line 1. "
                      "Details line 2.",
                      "Short Info.", verbose);
}

std::string FrameLib_CumulativeSum::inputInfo(unsigned long idx, bool verbose)
{
    switch (idx)
    {
        case 0:
            return formatInfo("Input Info - details", "Input Info", verbose);
    
        default:
            return parameterInputInfo(verbose);
    }
}

std::string FrameLib_CumulativeSum::outputInfo(unsigned long idx, bool verbose)
{
    return "Output";
}

// Parameter Info

FrameLib_CumulativeSum::ParameterInfo FrameLib_CumulativeSum::sParamInfo;

FrameLib_CumulativeSum::ParameterInfo::ParameterInfo()
{
    //add("Parameter Info.");
}

void FrameLib_CumulativeSum::process()
{
    unsigned long sizeOut = 0;
    
    requestOutputSize(0, sizeOut);
    
    if (allocateOutputs())
    {
        // Create Outputs
    }
}
