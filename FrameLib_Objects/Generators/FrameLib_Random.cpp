
#include <FrameLib_Random.h>

// Constructor

FrameLib_Random::FrameLib_Random(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner) : FrameLib_Processor(context, 2, 1)
{
    mParameters.addInt(kLength, "length", 1, 0);
    mParameters.setMin(0);
    
    mParameters.addEnum(kMode, "mode", 1);
    mParameters.addEnumItem(kInLength, "input");
    mParameters.addEnumItem(kRequestedLength, "requested");
    
    mParameters.set(serialisedParameters);
    
    mParameters.setInfo(&sParamInfo);
    
    setParameterInput(1);
}

// Info

std::string FrameLib_Random::objectInfo(bool verbose)
{
    return getInfo("Generates frames of random values in the range [0-1]: The size of the output is dependent on the mode. "
                   "The output size may either be set as a parameter, or be set to match that of the triggering input.",
                   "Generates frames of random values in the range [0-1].", verbose);
}

std::string FrameLib_Random::inputInfo(unsigned long idx, bool verbose)
{
    if (idx)
        return getInfo("Parameter Update - tagged input updates paramaeters", "Parameter Update", verbose);
    else
        return getInfo("Trigger Frame - triggers generation of output", "Trigger Frame", verbose);
}

std::string FrameLib_Random::outputInfo(unsigned long idx, bool verbose)
{
    return "Frame of Random Values";
}

// Parameter Info

FrameLib_Random::ParameterInfo FrameLib_Random::sParamInfo;

FrameLib_Random::ParameterInfo::ParameterInfo()
{
    add("Sets the length of the output when the mode is set to requested.");
    add("Controls how the output length is determined: "
        "input - the output frame size will match the input size. "
        "requested - the output frame size is set by the length parameter.");
}

// Process

void FrameLib_Random::process()
{
    unsigned long sizeIn, sizeOut;
    
    getInput(0, &sizeIn);
    
    sizeOut = ((Modes) mParameters.getInt(kMode)) == kInLength ? sizeIn : mParameters.getInt(kLength);
    requestOutputSize(0, sizeOut);
    allocateOutputs();
    
    double *output = getOutput(0, &sizeOut);
    
    for (unsigned long i = 0; i < sizeOut; i++)
        output[i] = mRandom.randDouble();
}

