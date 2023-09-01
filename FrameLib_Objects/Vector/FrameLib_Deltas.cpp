
#include "FrameLib_Deltas.h"
#include <numeric>

// Constructor

FrameLib_Deltas::FrameLib_Deltas(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy)
: FrameLib_Processor(context, proxy, &sParamInfo, 2, 1)
{
    mParameters.addEnum(kInitial, "initial", 0);
    mParameters.addEnumItem(kZeroDiff, "zerodiff");
    mParameters.addEnumItem(kWrapDiff, "wrapdiff");
    mParameters.addEnumItem(kZeroFill, "zerofill");

    mParameters.addEnum(kCompare, "compare", 1);
    mParameters.addEnumItem(kRight, "right");
    mParameters.addEnumItem(kLeft, "left");

    mParameters.set(serialisedParameters);

    setParameterInput(1);
}

// Info

std::string FrameLib_Deltas::objectInfo(bool verbose)
{
    return formatInfo("Calculates the differences between adjacent values within the frame: "
                      "The direction of the comparison is set by the compare parameter."
                      "The way the initial element is calculated is set by the initial parameter.",
                      "Calculates the differences between adjacent values within the frame.", verbose);
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

    if (!size)
        return;
    if (size == 1)
    {
        switch (mParameters.getEnum<Initials>(kInitial))
        {
        case kZeroDiff:     output[0] = input[0];   break;
        case kWrapDiff:     break;
        case kZeroFill:     break;
        }
        return;
    }

    unsigned long initPos =
        mParameters.getEnum<Compares>(kCompare) == kRight ? 0 : size - 1;

    if (mParameters.getEnum<Compares>(kCompare) == kRight)
        std::adjacent_difference(input, input + size, output);
    else
    {
        std::adjacent_difference(input + 1, input + size, output,
                                 [](const double& a, const double& b)
                                 { return b - a; });
        output[0] = input[0] - input[1];
        output[size - 1] = input[size - 1];
    }

    //initial element behaviour
    switch (mParameters.getEnum<Initials>(kInitial))
    {
    case kZeroDiff:     break;
    case kWrapDiff:     output[initPos] = input[initPos] 
                                        - input[initPos ? 0 : size - 1];    break;
    case kZeroFill:     output[initPos] = 0;                                break;
    }
}
