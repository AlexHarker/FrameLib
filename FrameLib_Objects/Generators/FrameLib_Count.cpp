
#include "FrameLib_Count.h"

// Constructor

FrameLib_Count::FrameLib_Count(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner) : FrameLib_Processor(context, &sParamInfo, 2, 1)
{
    mParameters.addInt(kLength, "length", 1, 0);
    mParameters.setMin(0);
    mParameters.addEnum(kMode, "mode", 1);
    mParameters.addEnumItem(kInLength, "input");
    mParameters.addEnumItem(kRequestedLength, "requested");
    
    mParameters.set(serialisedParameters);
        
    setParameterInput(1);
}

// Info

std::string FrameLib_Count::objectInfo(bool verbose)
{
    return getInfo("Generates frames consisting of the integers in counting order from 0 to (size - 1): The size of the output is dependent on the mode. "
                   "The output size may either be set as a parameter, or be set to match that of the triggering input.",
                   "Generates frames consisting of the integers in counting order from 0 to (size - 1).", verbose);
}

std::string FrameLib_Count::inputInfo(unsigned long idx, bool verbose)
{
    if (idx)
        return getInfo("Parameter Update - tagged input updates paramaeters", "Parameter Update", verbose);
    else
        return getInfo("Trigger Frame - triggers generation of output", "Trigger Frame", verbose);
}

std::string FrameLib_Count::outputInfo(unsigned long idx, bool verbose)
{
    return "Output Frame";
}

// Parameter Info

FrameLib_Count::ParameterInfo FrameLib_Count::sParamInfo;

FrameLib_Count::ParameterInfo::ParameterInfo()
{
    add("Sets the length of the output when the mode is set to requested.");
    add("Controls how the output length is determined: "
        "input - the output frame size will match the input size. "
        "requested - the output frame size is set by the length parameter.");
}

// Process

void FrameLib_Count::process()
{
    unsigned long sizeIn, sizeOut;
    
    getInput(0, &sizeIn);
    
    sizeOut = ((Modes) mParameters.getInt(kMode)) == kInLength ? sizeIn : mParameters.getInt(kLength);
    requestOutputSize(0, sizeOut);
    allocateOutputs();
    
    double *output = getOutput(0, &sizeOut);
    
    for (unsigned long i = 0; i < sizeOut; i++)
        output[i] = (double) i;
}
