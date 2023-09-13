
#include "FrameLib_Cumulative.h"
#include <numeric>

// Constructor

FrameLib_Cumulative::FrameLib_Cumulative(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy)
    : FrameLib_Processor(context, proxy, &sParamInfo, 2, 1)
{
    mParameters.addEnum(kMode, "mode", 0);
    mParameters.addEnumItem(kSum, "sum", true);
    mParameters.addEnumItem(kProduct, "product");

    mParameters.set(serialisedParameters);

    setParameterInput(1);
}

// Info

std::string FrameLib_Cumulative::objectInfo(bool verbose)
{
    return formatInfo("Performs a cumulative function on the input frame: "
                      "The output is the same length as the input."
                      "Performs a cumulative function on the input frame.", verbose);
}

std::string FrameLib_Cumulative::inputInfo(unsigned long idx, bool verbose)
{
    if (idx)
        return parameterInputInfo(verbose);
    else
        return "Input";
}

std::string FrameLib_Cumulative::outputInfo(unsigned long idx, bool verbose)
{
    return "Output";
}

// Parameter Info

FrameLib_Cumulative::ParameterInfo FrameLib_Cumulative::sParamInfo;

FrameLib_Cumulative::ParameterInfo::ParameterInfo()
{
    add("Set the cumulative function.");
}

// Process

void FrameLib_Cumulative::process()
{
    unsigned long size;
    const double *input = getInput(0, &size);

    requestOutputSize(0, size);
    allocateOutputs();

    double *output = getOutput(0, &size);

    if (output)
    {
        std::partial_sum(input, input + size, output);
    }
}
