
#include "FrameLib_Deltas.h"
#include <numeric>

// Constructor

FrameLib_Deltas::FrameLib_Deltas(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy)
: FrameLib_Processor(context, proxy, &sParamInfo, 2, 1)
{
    mParameters.addEnum(kMode, "mode", 0);
    mParameters.addEnumItem(kPadIn, "pad_in", true);
    mParameters.addEnumItem(kPadOut, "pad_out");
    mParameters.addEnumItem(kWrap, "wrap");
    mParameters.addEnumItem(kDrop, "drop");

    mParameters.addEnum(kDirection, "direction", 1);
    mParameters.addEnumItem(kForward, "forward", true);
    mParameters.addEnumItem(kBackward, "backward");

    mParameters.addEnum(kFillPosition, "fill_position", 2);
    mParameters.addEnumItem(kStart, "start", true);
    mParameters.addEnumItem(kEnd, "end");

    mParameters.addDouble(kPadding, "padding", 0, 3);

    mParameters.set(serialisedParameters);

    setParameterInput(1);
}

// Info

std::string FrameLib_Deltas::objectInfo(bool verbose)
{
    return formatInfo("Calculates the differences between adjacent values within the frame: "
                      "Edge element behaviour can be changed with a combination of the mode and padding parameters."
                      "Calculates the differences between adjacent values within the frame.", verbose);
}

std::string FrameLib_Deltas::inputInfo(unsigned long idx, bool verbose)
{
    if (idx)
        return parameterInputInfo(verbose);
    else
        return "Input";
}

std::string FrameLib_Deltas::outputInfo(unsigned long idx, bool verbose)
{
    return "Output";
}

// Parameter Info

FrameLib_Deltas::ParameterInfo FrameLib_Deltas::sParamInfo;

FrameLib_Deltas::ParameterInfo::ParameterInfo()
{
    add("Edge behaviour. Pad_in places the padding value outside the frame. Pad_out places the padding value inside the frame. Wrap compares the first and last elements. Drop outputs a frame with 1 less element.");
    add("Sets the subtraction direction.");
    add("Sets the position of padding value to the start or end of the frame.");
    add("Sets the value used for padding.");
}

// Process

void FrameLib_Deltas::process()
{
    unsigned long inSize;
    const double *input = getInput(0, &inSize);

    if (!inSize)
        return;

    bool drop = mParameters.getEnum<Modes>(kMode) == kDrop;
    unsigned long outSize = inSize - drop;

    if (!outSize)
        return;

    requestOutputSize(0, outSize);
    allocateOutputs();
    double *output = getOutput(0, &outSize);

    double padding = mParameters.getValue(kPadding);

    // Special handling of 1 element inputs

    if (inSize == 1)
    {
        switch (mParameters.getEnum<Modes>(kMode))
        {
            case kPadIn:        output[0] = input[0] - padding;     break;
            case kPadOut:       output[0] = padding;                break;
            case kWrap:         output[0] = 0;                      break;
            case kDrop:         break;
        }
        return;
    }

    // Difference calculations for all parameter permutations

    bool reverse = mParameters.getEnum<Directions>(kDirection) == kBackward;
    unsigned long fillPosition = mParameters.getEnum<PaddingLocations>(kFillPosition) == kEnd ? inSize - 1 : 0;
    bool nudgeInput = (mParameters.getEnum<PaddingLocations>(kFillPosition) == kEnd || drop);

    if (!reverse)
        std::adjacent_difference(input + nudgeInput, input + inSize, output);
    else if (reverse)
        std::adjacent_difference(input + nudgeInput, input + inSize, output,
                                 [](const double& a, const double& b)
                                 { return b - a; });

    if (nudgeInput)
    {
        output[0] = input[!reverse] - input[reverse];

        if (drop)
            return;
    }
    
    // Padding when mode is not set to drop

    unsigned long wrapPosition = fillPosition ? 0 : outSize - 1;

    switch (mParameters.getEnum<Modes>(kMode))
    {
        case kPadIn:
            output[fillPosition] = reverse == static_cast<bool>(fillPosition) ?
                input[fillPosition] - padding : padding - input[fillPosition];
            break;
        case kWrap:
            output[fillPosition] = reverse != static_cast<bool>(fillPosition) ?
                input[wrapPosition] - input[fillPosition] : input[fillPosition] - input[wrapPosition];
            break;
        case kPadOut:
            output[fillPosition] = padding;
            break;
        default:
            break;
    }
}