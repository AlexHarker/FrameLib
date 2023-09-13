
#include "FrameLib_Cumulative.h"
#include <numeric>

// Constructor

FrameLib_Cumulative::FrameLib_Cumulative(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy)
    : FrameLib_Processor(context, proxy, nullptr, 1, 1)
{
    mParameters.set(serialisedParameters);
}

// Info

std::string FrameLib_Cumulative::objectInfo(bool verbose)
{
    return formatInfo("Computes the cumulative sum of the input frame: "
                      "The output is the same length as the input."
                      "Computes the cumulative sum of the input frame.", verbose);
}

std::string FrameLib_Cumulative::inputInfo(unsigned long idx, bool verbose)
{
    return "Input";
}

std::string FrameLib_Cumulative::outputInfo(unsigned long idx, bool verbose)
{
    return "Output";
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
