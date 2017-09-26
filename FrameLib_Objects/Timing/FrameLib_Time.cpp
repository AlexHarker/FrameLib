
#include "FrameLib_Time.h"

FrameLib_Time::FrameLib_Time(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner) : FrameLib_Processor(context, owner, &sParamInfo, 1, 1)
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

std::string FrameLib_Time::objectInfo(bool verbose)
{
    return formatInfo("Outputs the current time in the specified units: Output is a single value.",
                   "Outputs the current time in the specified units.", verbose);
}

std::string FrameLib_Time::inputInfo(unsigned long idx, bool verbose)
{
    return formatInfo("Trigger Input - input frames generate output", "Trigger Input", verbose);
}

std::string FrameLib_Time::outputInfo(unsigned long idx, bool verbose)
{
    return "Output Values";
}

// Parameter Info

FrameLib_Time::ParameterInfo FrameLib_Time::sParamInfo;

FrameLib_Time::ParameterInfo::ParameterInfo()
{
    add("Sets the time units used to for output.");
}

// Calculate Multiplier

void FrameLib_Time::calculateMultiplier()
{
    switch ((Units) (mParameters.getValue(kUnits)))
    {
        case kMS:       mMultiplier = 1000.0 / FrameLib_TimeFormat(mSamplingRate);   break;
        case kSeconds:  mMultiplier = 1.0 / FrameLib_TimeFormat(mSamplingRate);      break;
        case kSamples:  mMultiplier = FrameLib_TimeFormat(1);                       break;
    }
}

// Update and Process

void FrameLib_Time::update()
{
    if (mParameters.changed(kUnits))
        calculateMultiplier();
}

void FrameLib_Time::process()
{
    requestOutputSize(0, 1);
    
    if (allocateOutputs())
    {
        unsigned long size;
        double *output = getOutput(0, &size);
        
        output[0] = (getCurrentTime() - FrameLib_TimeFormat(1)) * mMultiplier;
    }
}
