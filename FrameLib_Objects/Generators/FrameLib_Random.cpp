
#include "FrameLib_Random.h"

// Constructor

FrameLib_Random::FrameLib_Random(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy) : FrameLib_Processor(context, proxy, &sParamInfo, 2, 1)
{
    mParameters.addEnum(kMode, "mode", 0);
    mParameters.addEnumItem(kRequestedLength, "requested");
    mParameters.addEnumItem(kInLength, "input");
    
    mParameters.addInt(kLength, "length", 1, 1);
    mParameters.setMin(0);
    
    mParameters.addEnum(kUnits, "units", 2);
    mParameters.addEnumItem(kSamples, "samples");
    mParameters.addEnumItem(kMS, "ms");
    mParameters.addEnumItem(kSeconds, "seconds");
    
    mParameters.set(serialisedParameters);
        
    setParameterInput(1);
}

// Info

std::string FrameLib_Random::objectInfo(bool verbose)
{
    return formatInfo("Generates frames of linearly distributed random values in the range [0-1]: The size of the output is dependent on the mode. "
                   "The output size may either be set as a parameter, or be set to match that of the triggering input.",
                   "Generates frames of linearly distributed random values in the range [0-1].", verbose);
}

std::string FrameLib_Random::inputInfo(unsigned long idx, bool verbose)
{
    if (idx)
        return parameterInputInfo(verbose);
    else
        return formatInfo("Trigger Frame - triggers generation of output", "Trigger Frame", verbose);
}

std::string FrameLib_Random::outputInfo(unsigned long idx, bool verbose)
{
    return "Frame of Random Values";
}

// Parameter Info

FrameLib_Random::ParameterInfo FrameLib_Random::sParamInfo;

FrameLib_Random::ParameterInfo::ParameterInfo()
{
    add("Controls how the output length is determined: "
        "requested - the output frame size is set by the length parameter. "
        "input - the output frame size will match the input size.");
    add("Sets the length of the output when the mode is set to requested. Set in the units specified by the units parameter.");
    add("Sets the units for specified output lengths.");
}

// Helpers

unsigned long FrameLib_Random::getLength()
{
    double time = mParameters.getValue(kLength);
    
    switch (static_cast<Units>(mParameters.getInt(kUnits)))
    {
        case kSamples:  break;
        case kMS:       time = msToSamples(time);       break;
        case kSeconds:  time = secondsToSamples(time);  break;
    }
    
    return roundToUInt(time);
}

// Process

void FrameLib_Random::process()
{
    unsigned long sizeIn, sizeOut;
    
    getInput(0, &sizeIn);
    
    sizeOut = ((Modes) mParameters.getInt(kMode)) == kInLength ? sizeIn : getLength();
    requestOutputSize(0, sizeOut);
    allocateOutputs();
    
    double *output = getOutput(0, &sizeOut);
    
    for (unsigned long i = 0; i < sizeOut; i++)
        output[i] = mRandom.randDouble();
}

