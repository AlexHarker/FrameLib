
#include "FrameLib_Deltas.h"
#include <numeric>

// Constructor

FrameLib_Deltas::FrameLib_Deltas(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy)
: FrameLib_Processor(context, proxy, &sParamInfo, 2, 1)
{
    mParameters.addEnum(kMode, "mode", 0);
    mParameters.addEnumItem(kPad_In, "pad_in");
    mParameters.addEnumItem(kPad_Out, "pad_out");
    mParameters.addEnumItem(kWrap, "wrap");
    mParameters.addEnumItem(kDrop, "drop");

    mParameters.addEnum(kDirection, "direction", 1);
    mParameters.addEnumItem(kForward, "forward");
    mParameters.addEnumItem(kBackward, "backward");

    mParameters.addEnum(kPaddingLocation, "padding_location", 2);
    mParameters.addEnumItem(kStart, "start");
    mParameters.addEnumItem(kEnd, "end");

    mParameters.addDouble(kPaddingValue, "padding_value", 0, 3);

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
    //TODO: FIX THESE
    add("mode: Sets the behaviour of the algorithm for the initial element.");
    add("direction: Sets the comparison direction of each element.");
    add("padding_location: ");
    add("padding_value: ");
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

    double padVal = mParameters.getValue(kPaddingValue);

    if (inSize == 1)
    {
        switch (mParameters.getEnum<Modes>(kMode))
        {
            case kPad_In:       output[0] = input[0] - padVal;  break;
            case kPad_Out:      output[0] = padVal;             break;
            case kWrap:         output[0] = 0;                  break;
            case kDrop:         break;
        }
        return;
    }

    bool reverse = mParameters.getEnum<Directions>(kDirection) == kBackward;

    if (drop)
    {
        if (!reverse)
        {
            std::adjacent_difference(input + 1, input + inSize, output);
            output[0] = input[1] - input[0];
        }
        else if (reverse)
        {
            std::adjacent_difference(input + 1, input + inSize, output,
                [](const double& a, const double& b)
                { return b - a; });
            output[0] = input[0] - input[1];
        }
        return;
    }

    unsigned long padLoc = mParameters.getEnum<PaddingLocations>(kPaddingLocation) == kEnd ? inSize - 1 : 0;

    if (!reverse && !padLoc)
    {
        std::adjacent_difference(input, input + inSize, output);
        pad(input, output, padLoc, outSize, padVal, reverse);
    }
    else if (!reverse && padLoc)
    {
        std::adjacent_difference(input + 1, input + inSize, output);
        output[0] = input[1] - input[0];
        pad(input, output, padLoc, outSize, padVal, reverse);
    }
    else if (reverse && padLoc)
    {
        std::adjacent_difference(input + 1, input + inSize, output,
            [](const double& a, const double& b)
            { return b - a; });
        output[0] = input[0] - input[1];
        pad(input, output, padLoc, outSize, padVal, reverse);
    }
    else if (reverse && !padLoc)
    {
        std::adjacent_difference(input, input + inSize, output,
            [](const double& a, const double& b)
            { return b - a; });
        pad(input, output, padLoc, outSize, padVal, reverse);
    }
}

void FrameLib_Deltas::pad(const double* input, double* output, const unsigned long& padLoc, const unsigned long& outSize, const double& padVal, bool reverse)
{   
    unsigned long wrapLoc = padLoc ? 0 : outSize - 1;

    switch (mParameters.getEnum<Modes>(kMode))
    {
        case kPad_In:
            output[padLoc] = reverse == static_cast<bool>(padLoc) ?
                input[padLoc] - padVal : padVal - input[padLoc];
            break;
        case kWrap:
            output[padLoc] = reverse != static_cast<bool>(padLoc) ?
                input[wrapLoc] - input[padLoc] : input[padLoc] - input[wrapLoc];
            break;
        case kPad_Out:
            output[padLoc] = padVal;
            break;
        default:
            break;
    }
}