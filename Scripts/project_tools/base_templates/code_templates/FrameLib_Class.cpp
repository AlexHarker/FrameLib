
#include "_##OBJECT_CLASS_FILE##_.h"

_##OBJECT_CLASS##_::_##OBJECT_CLASS##_(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy)
: FrameLib_Processor(context, proxy, &sParamInfo, 1, 1)
{
    mParameters.set(serialisedParameters);
}

std::string _##OBJECT_CLASS##_::objectInfo(bool verbose)
{
    return formatInfo("Long Info: "
                      "Details line 1. "
                      "Details line 2.",
                      "Short Info.", verbose);
}

std::string _##OBJECT_CLASS##_::inputInfo(unsigned long idx, bool verbose)
{
    switch (idx)
    {
        case 0:
            return formatInfo("Input Info - details", "Input Info", verbose);
    
        default:
            return parameterInputInfo(verbose);
    }
}

std::string _##OBJECT_CLASS##_::outputInfo(unsigned long idx, bool verbose)
{
    return "Output";
}

// Parameter Info

_##OBJECT_CLASS##_::ParameterInfo _##OBJECT_CLASS##_::sParamInfo;

_##OBJECT_CLASS##_::ParameterInfo::ParameterInfo()
{
    //add("Parameter Info.");
}

void _##OBJECT_CLASS##_::process()
{
    unsigned long sizeOut = 0;
    
    requestOutputSize(0, sizeOut);
    
    if (allocateOutputs())
    {
        // Create Outputs
    }
}
