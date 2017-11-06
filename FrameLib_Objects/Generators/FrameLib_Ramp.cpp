
#include "FrameLib_Ramp.h"

// Constructor

FrameLib_Ramp::FrameLib_Ramp(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner) : FrameLib_Processor(context, owner, &sParamInfo, 2, 1)
{
    mParameters.addEnum(kMode, "mode", 0);
    mParameters.addEnumItem(kRequestedLength, "requested");
    mParameters.addEnumItem(kInLength, "input");
    mParameters.addInt(kLength, "length", 1, 1);
    mParameters.setMin(0);
    mParameters.addEnum(kScale, "scale", 0);
    mParameters.addEnumItem(kSamples, "count");
    mParameters.addEnumItem(kMS, "ms");
    mParameters.addEnumItem(kSeconds, "seconds");
    mParameters.addEnumItem(kNormalised, "normalised");
    
    mParameters.set(serialisedParameters);
        
    setParameterInput(1);
}

// Info

std::string FrameLib_Ramp::objectInfo(bool verbose)
{
    return formatInfo("Generates frames that consist of linear ramps from [0-x]: "
                      "The value of x depends on the scaling parameter. "
                      "The size of the output is dependent on the mode. "
                      "The output size may either be set as a parameter, or be set to match that of the triggering input.",
                      "Generates frames that consist of linear ramps from [0-x]", verbose);
}

std::string FrameLib_Ramp::inputInfo(unsigned long idx, bool verbose)
{
    if (idx)
        return parameterInputInfo(verbose);
    else
        return formatInfo("Trigger Frame - triggers generation of output", "Trigger Frame", verbose);
}

std::string FrameLib_Ramp::outputInfo(unsigned long idx, bool verbose)
{
    return "Output Frame";
}

// Parameter Info

FrameLib_Ramp::ParameterInfo FrameLib_Ramp::sParamInfo;

FrameLib_Ramp::ParameterInfo::ParameterInfo()
{
    add("Controls how the output length is determined: "
        "requested - the output frame size is set by the length parameter. "
        "input - the output frame size will match the input size.");
    add("Sets the length of the output when the mode is set to requested.");
    add("Sets the scaling of the output ramp.");
}

// Process

void FrameLib_Ramp::process()
{
    unsigned long sizeIn, sizeOut;
    
    getInput(0, &sizeIn);
    
    sizeOut = ((Modes) mParameters.getInt(kMode)) == kInLength ? sizeIn : mParameters.getInt(kLength);
    requestOutputSize(0, sizeOut);
    allocateOutputs();
    
    double *output = getOutput(0, &sizeOut);
    double multiplier = 1.0;
    
    switch (static_cast<Scales>(mParameters.getValue(kScale)))
    {
        case kMS:           multiplier = 1000.0 / mSamplingRate;                        break;
        case kSeconds:      multiplier = 1.0 / mSamplingRate;                           break;
        case kSamples:      multiplier = 1.0;                                           break;
        case kNormalised:   multiplier = 1.0 / static_cast<double>(sizeOut - 1);        break;
    }

    for (unsigned long i = 0; i < sizeOut; i++)
        output[i] = static_cast<double>(i) * multiplier;
}
