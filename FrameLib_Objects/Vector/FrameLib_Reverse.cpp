
#include "FrameLib_Reverse.h"
#include <algorithm>

// Constructor

FrameLib_Reverse::FrameLib_Reverse(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy)
: FrameLib_Processor(context, proxy, nullptr, 1, 1)
{
    mParameters.set(serialisedParameters);
}

// Info

std::string FrameLib_Reverse::objectInfo(bool verbose)
{
    return formatInfo("Computes the reverse of the input frame: "
                      "The output is the same length as the input.",
                      "Computes the reverse of the input frame.", verbose);
}

std::string FrameLib_Reverse::inputInfo(unsigned long idx, bool verbose)
{
    return "Input";
}

std::string FrameLib_Reverse::outputInfo(unsigned long idx, bool verbose)
{
    return "Output";
}

// Process

void FrameLib_Reverse::process()
{
    unsigned long size;
    const double *input = getInput(0, &size);

    requestOutputSize(0, size);
    allocateOutputs();

    double *output = getOutput(0, &size);

    if (output)
        std::reverse_copy(input, input + size, output);
}
