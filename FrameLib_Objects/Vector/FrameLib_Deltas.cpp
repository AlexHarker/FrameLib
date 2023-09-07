
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
    mParameters.addEnumItem(kDrop, "drop");

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
    unsigned long inSize;
    const double *input = getInput(0, &inSize);
    if (!inSize)
        return;

    bool drop = mParameters.getEnum<Initials>(kInitial) == kDrop;
    unsigned long outSize = inSize - drop;
    if (!outSize)
        return;

    requestOutputSize(0, outSize);
    allocateOutputs();
    double *output = getOutput(0, &outSize);

    if (inSize == 1)
    {
        switch (mParameters.getEnum<Initials>(kInitial))
        {
            case kZeroDiff:     output[0] = input[0];   break;
            case kWrapDiff:     output[0] = 0;          break;
            case kZeroFill:     output[0] = 0;          break;
        }
        return;
    }

    bool reverse = mParameters.getEnum<Compares>(kCompare) == kLeft;
    unsigned long initPos = reverse ? inSize - 1 : 0;

    if (!reverse)
    {
        std::adjacent_difference(input + drop, input + inSize, output);
        if (drop)
            output[0] = input[1] - input[0];
    }
    else
    {
        std::adjacent_difference(input + 1, input + inSize, output,
                                 [](const double& a, const double& b)
                                 { return b - a; });
        output[0] = input[0] - input[1];
        if (!drop)
            output[outSize - 1] = input[outSize - 1];
    }

    // Initial element behaviour

    switch (mParameters.getEnum<Initials>(kInitial))
    {
        case kZeroDiff:     break;
        case kWrapDiff:     output[initPos] = input[initPos] 
                                            - input[initPos ? 0 : outSize - 1]; break;
        case kZeroFill:     output[initPos] = 0;                                break;
        case kDrop:         break;
    }
}
