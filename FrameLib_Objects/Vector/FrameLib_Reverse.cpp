
#include "FrameLib_Reverse.h"
#include <algorithm>

// Constructor

FrameLib_Reverse::FrameLib_Reverse(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy) : FrameLib_Processor(context, proxy, nullptr, 1, 1)
{}

// Info

std::string FrameLib_Reverse::objectInfo(bool verbose)
{
    return "Reverses the order of a single frame's contents.";
}

std::string FrameLib_Reverse::inputInfo(unsigned long idx, bool verbose)
{
    return "Input Frame";
}

std::string FrameLib_Reverse::outputInfo(unsigned long idx, bool verbose)
{
    return "Reversed Frame";
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
        std::reverse_copy(input, input+size, output);
}
