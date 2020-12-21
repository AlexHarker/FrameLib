
#include "FrameLib_Now.h"

FrameLib_Now::FrameLib_Now(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy)
: FrameLib_Processor(context, proxy, &sParamInfo, 1, 1)
{
    mParameters.addEnum(kUnits, "units", 0);
    mParameters.addEnumItem(kSamples, "samples");
    mParameters.addEnumItem(kMS, "ms");
    mParameters.addEnumItem(kSeconds, "seconds");
    mParameters.setInstantiation();
    
    mParameters.set(serialisedParameters);
        
    calculateMultiplier();
}

// Info

std::string FrameLib_Now::objectInfo(bool verbose)
{
    return formatInfo("Outputs the current time: "
                      "Time is reported in the specified units. "
                      "Output is a single value.",
                      "Outputs the current time.", verbose);
}

std::string FrameLib_Now::inputInfo(unsigned long idx, bool verbose)
{
    return formatInfo("Trigger Input - triggers output", "Trigger Input", verbose);
}

std::string FrameLib_Now::outputInfo(unsigned long idx, bool verbose)
{
    return "Output";
}

// Parameter Info

FrameLib_Now::ParameterInfo FrameLib_Now::sParamInfo;

FrameLib_Now::ParameterInfo::ParameterInfo()
{
    add("Sets the time units used for output.");
}

// Calculate Multiplier

void FrameLib_Now::calculateMultiplier()
{
    switch (mParameters.getEnum<Units>(kUnits))
    {
        case kSamples:  mMultiplier = FrameLib_TimeFormat(1);                       break;
        case kMS:       mMultiplier = 1000.0 / FrameLib_TimeFormat(mSamplingRate);  break;
        case kSeconds:  mMultiplier = 1.0 / FrameLib_TimeFormat(mSamplingRate);     break;
    }
}

// Update and Process

void FrameLib_Now::update()
{
    if (mParameters.changed(kUnits))
        calculateMultiplier();
}

void FrameLib_Now::process()
{
    requestOutputSize(0, 1);
    
    if (allocateOutputs())
    {
        unsigned long size;
        double *output = getOutput(0, &size);
        
        output[0] = (getCurrentTime() - FrameLib_TimeFormat(1)) * mMultiplier;
    }
}
