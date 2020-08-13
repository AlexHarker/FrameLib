
#include "FrameLib_Timer.h"

FrameLib_Timer::FrameLib_Timer(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy)
: FrameLib_Processor(context, proxy, &sParamInfo, 2, 1)
{
    mParameters.addEnum(kUnits, "units", 0);
    mParameters.addEnumItem(kSamples, "samples");
    mParameters.addEnumItem(kMS, "ms");
    mParameters.addEnumItem(kSeconds, "seconds");
    mParameters.setInstantiation();
    
    mParameters.set(serialisedParameters);
    
    setInputMode(0, false, false, false);
    calculateMultiplier();
}

// Info

std::string FrameLib_Timer::objectInfo(bool verbose)
{
    return formatInfo("Calculates the time elapsed between trigger frame inputs and the subsequent reset frame: "
                      "Time is reported in the specified units. "
                      "Output is a single value.",
                      "Calculates the time elapsed between the last reset and each trigger.", verbose);
}

std::string FrameLib_Timer::inputInfo(unsigned long idx, bool verbose)
{
    if (idx)
        return formatInfo("Trigger Input - triggers output", "Trigger Input", verbose);
    else
        return formatInfo("Reset Input - does not trigger output", "Reset Input", verbose);
}

std::string FrameLib_Timer::outputInfo(unsigned long idx, bool verbose)
{
    return "Output";
}

// Parameter Info

FrameLib_Timer::ParameterInfo FrameLib_Timer::sParamInfo;

FrameLib_Timer::ParameterInfo::ParameterInfo()
{
    add("Sets the time units used to for output.");
}

// Calculate Multiplier

void FrameLib_Timer::calculateMultiplier()
{
    switch (static_cast<Units>(mParameters.getInt(kUnits)))
    {
        case kSamples:  mMultiplier = FrameLib_TimeFormat(1);                           break;
        case kMS:       mMultiplier = 1000.0 / FrameLib_TimeFormat(mSamplingRate);      break;
        case kSeconds:  mMultiplier = 1.0 / FrameLib_TimeFormat(mSamplingRate);         break;
    }
}

// Update and Process

void FrameLib_Timer::update()
{
    if (mParameters.changed(kUnits))
        calculateMultiplier();
}

void FrameLib_Timer::process()
{
    FrameLib_TimeFormat previousTime = getInputFrameTime(0);
    
    requestOutputSize(0, previousTime.greaterThanZero() ? 1 : 0);
    
    if (allocateOutputs())
    {
        unsigned long size;
        double *output = getOutput(0, &size);
        
        output[0] = ((getCurrentTime() - previousTime) * mMultiplier);
    }
}
