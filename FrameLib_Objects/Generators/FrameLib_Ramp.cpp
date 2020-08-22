
#include "FrameLib_Ramp.h"

// Constructor

FrameLib_Ramp::FrameLib_Ramp(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy)
: FrameLib_Processor(context, proxy, &sParamInfo, 2, 1)
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
    
    mParameters.addEnum(kScale, "scale", 3);
    mParameters.addEnumItem(kScaleSamples, "count");
    mParameters.addEnumItem(kScaleMS, "ms");
    mParameters.addEnumItem(kScaleSeconds, "seconds");
    mParameters.addEnumItem(kScaleNormalised, "normalised");

    mParameters.addEnum(kEndpoints, "endpoints", 4);
    mParameters.addEnumItem(kBoth, "both");
    mParameters.addEnumItem(kFirst, "first");
    mParameters.addEnumItem(kLast, "last");
    mParameters.addEnumItem(kNone, "none");
    
    mParameters.set(serialisedParameters);
    
    setParameterInput(1);
}

// Info

std::string FrameLib_Ramp::objectInfo(bool verbose)
{
    return formatInfo("Generates a linear ramp across each output frame: "
                      "The scaling parameter is used to set the range of the ramp. "
                      "The length of the output is dependent on the mode. "
                      "Output length may be set by parameter or based on that of the trigger input.",
                      "Generates a linear ramp across each output frame.", verbose);
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
        "requested - the output length is set by the length parameter. "
        "input - the output length follows the length of the trigger input.");
    add("Sets the requested output length in the units specified by the units parameter.");
    add("Sets the units for specified output lengths.");
    add("Sets the scaling of the output ramp. "
        "count - scaled in samples (an interger count from zero). "
        "ms - scaled in milliseconds starting at zero. "
        "seconds - scaled in seconds starting at zero. "
        "normalised - output is normalised according to the edges parameter.");
    add("Sets the included edges when the scaling is set to normalised: "
        "both - the ramp is scaled [0-1]. "
        "first - the ramp is scaled [0-1). "
        "last - the ramp is scaled (0-1]. "
        "none - the ramp is scaled (0-1).");
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
    double offset = 0.0;
    double normalisedScale = 0.0;
    
    switch (static_cast<Scales>(mParameters.getInt(kScale)))
    {
        case kScaleMS:              multiplier = 1000.0 / mSamplingRate;    break;
        case kScaleSeconds:         multiplier = 1.0 / mSamplingRate;       break;
        case kScaleSamples:         multiplier = 1.0;                       break;
        case kScaleNormalised:
        {
            Endpoints ends = static_cast<Endpoints>(mParameters.getInt(kEndpoints));

            switch (ends)
            {
                case kBoth:
                    normalisedScale = static_cast<double>(sizeOut - 1);
                    break;
                
                case kFirst:
                case kLast:
                    normalisedScale = static_cast<double>(sizeOut);
                    break;
                    
                case kNone:
                    normalisedScale = static_cast<double>(sizeOut + 1);
                    break;
            }
            
            multiplier = 1.0 / normalisedScale;
            
            if (ends == kNone || ends == kLast)
                offset = 1.0 / normalisedScale;
        
            break;
        }
    }

    for (unsigned long i = 0; i < sizeOut; i++)
        output[i] = static_cast<double>(i) * multiplier + offset;
}
