
#include "FrameLib_Deltas.h"
#include <numeric>

// Constructor

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

// Info

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

// Process

void FrameLib_Deltas::process()
{
    unsigned long size;
    const double *input = getInput(0, &size);

    requestOutputSize(0, size);
    allocateOutputs();

    double *output = getOutput(0, &size);

    if (!output)
    {
        return;
    }

    std::adjacent_difference(input, input + size, output);

    //account for initial element
    switch (mParameters.getEnum<Initials>(kInitial))
    {
    case kZeroDiff:     output[0] = input[0];                   break;
    case kWrapDiff:     output[0] = input[0] - input[size - 1]; break;
    case kZero:         output[0] = 0;                          break;
    }

    //invert difference order
    if (mParameters.getEnum<Compares>(kCompare) == kLeft)
    {
        double temp = output[0];
        for (unsigned long i = 0; i < size; ++i)
        {
            output[i] = output[i + 1] * -1;
        }
        output[size - 1] = temp * -1;

        if (mParameters.getEnum<Initials>(kInitial) == kZeroDiff)
            output[size - 1] = input[size - 1];
    }
}
