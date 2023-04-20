
#include "_##CLASS##_.h"

_##CLASS##_::_##CLASS##_(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy)
: FrameLib_Processor(context, proxy, &sParamInfo, 1, 1)
{
    mParameters.set(serialisedParameters);
}

std::string _##CLASS##_::objectInfo(bool verbose)
{
    return formatInfo("Long Info: "
                      "Details line 1. "
                      "Details line 2.",
                      "Short Info.", verbose);
}

std::string _##CLASS##_::inputInfo(unsigned long idx, bool verbose)
{
    switch (idx)
    {
        case 0:
            return formatInfo("Input Info - details", "Input Info", verbose);
    
        default:
            return parameterInputInfo(verbose);
    }
}

std::string _##CLASS##_::outputInfo(unsigned long idx, bool verbose)
{
    return "Output";
}

// Parameter Info

_##CLASS##_::ParameterInfo _##CLASS##_::sParamInfo;

_##CLASS##_::ParameterInfo::ParameterInfo()
{
    //add("Parameter Info.");
}

void _##CLASS##_::process()
{
    unsigned long sizeOut = 0;
    
    requestOutputSize(0, sizeOut);
    
    if (allocateOutputs())
    {
        // Create Outputs
    }
}
