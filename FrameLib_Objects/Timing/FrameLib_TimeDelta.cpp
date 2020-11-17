
#include "FrameLib_TimeDelta.h"

FrameLib_TimeDelta::FrameLib_TimeDelta(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy)
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

std::string FrameLib_TimeDelta::objectInfo(bool verbose)
{
    return formatInfo("Calculates the time interval between consecutive incoming frames: "
                      "Time is reported in the specified units. "
                      "Output is a single value.",
                      "Outputs the time interval between consecutive incoming frames.", verbose);
}

std::string FrameLib_TimeDelta::inputInfo(unsigned long idx, bool verbose)
{
    return formatInfo("Trigger Input - triggers output", "Trigger Input", verbose);
}

std::string FrameLib_TimeDelta::outputInfo(unsigned long idx, bool verbose)
{
    return "Output";
}

// Parameter Info

FrameLib_TimeDelta::ParameterInfo FrameLib_TimeDelta::sParamInfo;

FrameLib_TimeDelta::ParameterInfo::ParameterInfo()
{
    add("Sets the time units used to for output.");
}

// Calculate Multiplier

void FrameLib_TimeDelta::calculateMultiplier()
{
    switch (mParameters.getEnum<Units>(kUnits))
    {
        case kSamples:  mMultiplier = FrameLib_TimeFormat(1);                       break;
        case kMS:       mMultiplier = 1000.0 / FrameLib_TimeFormat(mSamplingRate);  break;
        case kSeconds:  mMultiplier = 1.0 / FrameLib_TimeFormat(mSamplingRate);     break;
    }
}

// Object Reset

void FrameLib_TimeDelta::objectReset()
{
    mLastTime = FrameLib_TimeFormat(0);
    calculateMultiplier();
}

// Update and Process

void FrameLib_TimeDelta::update()
{
    if (mParameters.changed(kUnits))
        calculateMultiplier();
}

void FrameLib_TimeDelta::process()
{
    FrameLib_TimeFormat now = getCurrentTime();

    requestOutputSize(0, mLastTime.greaterThanZero() ? 1 : 0);
    
    if (allocateOutputs())
    {
        unsigned long size;
        double *output = getOutput(0, &size);
        
        output[0] = (now - mLastTime) * mMultiplier;
    }
    
    mLastTime = now;

}
