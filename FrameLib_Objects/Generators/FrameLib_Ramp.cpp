
#include "FrameLib_Ramp.h"

// Constructor

FrameLib_Ramp::FrameLib_Ramp(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy) : FrameLib_Processor(context, proxy, &sParamInfo, 2, 1)
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
    
    mParameters.addEnum(kScale, "scale");
    mParameters.addEnumItem(kScaleSamples, "count");
    mParameters.addEnumItem(kScaleMS, "ms");
    mParameters.addEnumItem(kScaleSeconds, "seconds");
    mParameters.addEnumItem(kScaleNormalised, "normalised");
    
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
        return formatInfo("Trigger Input - triggers output", "Trigger Input", verbose);
}

std::string FrameLib_Ramp::outputInfo(unsigned long idx, bool verbose)
{
    return "Output";
}

// Parameter Info

FrameLib_Ramp::ParameterInfo FrameLib_Ramp::sParamInfo;

FrameLib_Ramp::ParameterInfo::ParameterInfo()
{
    add("Controls how the output length is determined: "
        "requested - the output frame size is set by the length parameter. "
        "input - the output frame size will match the input size.");
    add("Sets the length of the output when the mode is set to requested. Set in the units specified by the units parameter.");
    add("Sets the units for specified output lengths.");
    add("Sets the scaling of the output ramp.");
}

// Helpers

unsigned long FrameLib_Ramp::getLength()
{
    double time = mParameters.getValue(kLength);
    
    switch (static_cast<Units>(mParameters.getInt(kUnits)))
    {
        case kScaleSamples:  break;
        case kScaleMS:       time = msToSamples(time);       break;
        case kScaleSeconds:  time = secondsToSamples(time);  break;
    }
    
    return roundToUInt(time);
}

// Process

void FrameLib_Ramp::process()
{
    unsigned long sizeIn, sizeOut;
    
    getInput(0, &sizeIn);
    
    sizeOut = ((Modes) mParameters.getInt(kMode)) == kInLength ? sizeIn : getLength();
    requestOutputSize(0, sizeOut);
    allocateOutputs();
    
    double *output = getOutput(0, &sizeOut);
    double multiplier = 1.0;
    
    switch (static_cast<Scales>(mParameters.getInt(kScale)))
    {
        case kScaleMS:              multiplier = 1000.0 / mSamplingRate;                        break;
        case kScaleSeconds:         multiplier = 1.0 / mSamplingRate;                           break;
        case kScaleSamples:         multiplier = 1.0;                                           break;
        case kScaleNormalised:      multiplier = 1.0 / static_cast<double>(sizeOut - 1);        break;
    }

    for (unsigned long i = 0; i < sizeOut; i++)
        output[i] = static_cast<double>(i) * multiplier;
}
