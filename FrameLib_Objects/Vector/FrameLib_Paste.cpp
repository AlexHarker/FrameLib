
#include "FrameLib_Paste.h"

// Constructor

FrameLib_Paste::FrameLib_Paste(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy)
: FrameLib_Processor(context, proxy, &sParamInfo, 4, 1)
{
    mParameters.addEnum(kMode, "mode", 0);
    mParameters.addEnumItem(kRequestedLength, "requested");
    mParameters.addEnumItem(kInLength, "input");
    
    mParameters.addInt(kLength, "length", 1, 1);
    mParameters.setMin(0);
    
    mParameters.set(serialisedParameters);
    
    setParameterInput(3);
}

// Info

std::string FrameLib_Paste::objectInfo(bool verbose)
{
    return formatInfo("Pastes values into a output frame according to a provided ordering map: "
                      "Values at the input are pasted to the output index specified by the map input. "
                      "Where two or more values are pasted to the same output index they will be summed. "
                      "Values for which no map is provided, or for which the map is out of range are ignored. "
                      "Output indices to which no values are pasted will be set to zero. "
                      "The length of the output is dependent on the mode parameter. "
                      "Output length may be set by parameter or based on that of the trigger input. ",
                      "Pastes values into a output frame according to a provided ordering map.", verbose);
}

std::string FrameLib_Paste::inputInfo(unsigned long idx, bool verbose)
{
    switch (idx)
    {
        case 0:
            return formatInfo("Trigger Input - triggers output", "Trigger Input", verbose);
            break;
        case 1:
            return formatInfo("Values Input - values to paste", "Values Input", verbose);
            break;
        case 2:
            return formatInfo("Map Input - map for ordering", "Map Input", verbose);
            break;
            
        default:
            return parameterInputInfo(verbose);
    }
}

std::string FrameLib_Paste::outputInfo(unsigned long idx, bool verbose)
{
    return formatInfo("Output", idx);
}

// Parameter Info

FrameLib_Paste::ParameterInfo FrameLib_Paste::sParamInfo;

FrameLib_Paste::ParameterInfo::ParameterInfo()
{
    add("Controls how the output length is determined: "
        "requested - the output length is set by the length parameter. "
        "input - the output length follows the length of the trigger input.");
    add("Sets the requested output length in the units specified by the units parameter.");
}

// Process

void FrameLib_Paste::process()
{
    // Get Input
    
    unsigned long sizeIn, sizeMap, sizeOut;
    
    const double *input = getInput(1, &sizeIn);
    const double *map = getInput(2, &sizeMap);

    // Assume that the size should be the same as the trigger input
    
    getInput(0, &sizeOut);
    
    if (mParameters.getEnum<Modes>(kMode) == kRequestedLength)
        sizeOut = mParameters.getInt(kLength);
    
    // Assign outputs
    
    requestOutputSize(0, sizeOut);
    allocateOutputs();
        
    double *output = getOutput(0, &sizeOut);
    
    sizeMap = sizeOut ? std::min(sizeMap, sizeIn) : 0;

    // Zero
    
    zeroVector(output, sizeOut);
    
    // Paste to outputs
    
    for (unsigned long i = 0; i < sizeMap; i++)
    {
        unsigned long idx = static_cast<unsigned long>(map[i]);
        
        if (idx >= 0 && idx < sizeOut)
            output[idx] += input[i];
    }
}
