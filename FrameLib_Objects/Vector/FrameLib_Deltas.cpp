
#include "FrameLib_Deltas.h"

FrameLib_Deltas::FrameLib_Deltas(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy)
: FrameLib_Processor(context, proxy, &sParamInfo, 2, 1)
{
    mParameters.addEnum(kInitial, "initial", 0);
    mParameters.addEnumItem(kZeroDiff, "zerodiff");
    mParameters.addEnumItem(kWrapDiff, "wrapdiff");
    mParameters.addEnumItem(kZero, "zero");

    mParameters.addEnum(kCompare, "compare", 1);
    mParameters.addEnumItem(kRight, "right");
    mParameters.addEnumItem(kLeft, "left");

    mParameters.set(serialisedParameters);

    setParameterInput(1);
}

std::string FrameLib_Deltas::objectInfo(bool verbose)
{
    return formatInfo("Long Info: "
                      "Details line 1. "
                      "Details line 2.",
                      "Short Info.", verbose);
}

std::string FrameLib_Deltas::inputInfo(unsigned long idx, bool verbose)
{
    switch (idx)
    {
        case 0:
            return formatInfo("Input Info - details", "Input Info", verbose);
    
        default:
            return parameterInputInfo(verbose);
    }
}

std::string FrameLib_Deltas::outputInfo(unsigned long idx, bool verbose)
{
    return "Output";
}

// Parameter Info

FrameLib_Deltas::ParameterInfo FrameLib_Deltas::sParamInfo;

FrameLib_Deltas::ParameterInfo::ParameterInfo()
{
    add("Sets the behaviour of the algorithm for the initial element.");
    add("Sets the comparison direction of each element");
}

void FrameLib_Deltas::process()
{
    unsigned long sizeOut = 0;
    
    requestOutputSize(0, sizeOut);
    
    if (allocateOutputs())
    {
        // Create Outputs
    }
}
