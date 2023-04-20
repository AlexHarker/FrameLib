
#include "FrameLib_##CLASS##.h"

FrameLib_##CLASS##::FrameLib_##CLASS##(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy)
: FrameLib_Processor(context, proxy, &sParamInfo, 1, 1)
{
    mParameters.set(serialisedParameters);
}

std::string FrameLib_##CLASS##::objectInfo(bool verbose)
{
    return formatInfo("Long Info: "
                      "Details line 1. "
                      "Details line 2.",
                      "Short Info.", verbose);
}

std::string FrameLib_##CLASS##::inputInfo(unsigned long idx, bool verbose)
{
    switch (idx)
    {
        case 0:
            return formatInfo("Input Info - details", "Input Info", verbose);
    
        default:
            return parameterInputInfo(verbose);
    }
}

std::string FrameLib_##CLASS##::outputInfo(unsigned long idx, bool verbose)
{
    return "Output";
}

// Parameter Info

FrameLib_##CLASS##::ParameterInfo FrameLib_##CLASS##::sParamInfo;

FrameLib_##CLASS##::ParameterInfo::ParameterInfo()
{
    //add("Parameter Info.");
}

void FrameLib_##CLASS##::process()
{
    unsigned long sizeOut = 0;
    
    requestOutputSize(0, sizeOut);
    
    if (allocateOutputs())
    {
        // Create Outputs
    }
}
