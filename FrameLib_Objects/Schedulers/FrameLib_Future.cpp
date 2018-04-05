
#include "FrameLib_Future.h"

FrameLib_Future::FrameLib_Future(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy) : FrameLib_Scheduler(context, proxy, &sParamInfo, 1, 1)
{
    mParameters.addDouble(kTime, "time", 0, 0);
    mParameters.setMin(0);
    
    mParameters.addEnum(kUnits, "units", 1);
    mParameters.addEnumItem(kSamples, "samples");
    mParameters.addEnumItem(kMS, "ms");
    mParameters.addEnumItem(kSeconds, "seconds");
    
    mParameters.addEnum(kMode, "mode", 2);
    mParameters.addEnumItem(kSamples, "absolute");
    mParameters.addEnumItem(kMS, "relative");
    
    mParameters.set(serialisedParameters);
    
    setParameterInput(0);
    
    calculateTime();
}

// Info

std::string FrameLib_Future::objectInfo(bool verbose)
{
    return formatInfo("Schedules frames in the future at a specified time: Output frames are empty.",
                   "Schedules frames in the future at a specified time.", verbose);
}

std::string FrameLib_Future::inputInfo(unsigned long idx, bool verbose)
{
    return parameterInputInfo(verbose);
}

std::string FrameLib_Future::outputInfo(unsigned long idx, bool verbose)
{
    return "Empty Trigger Frames";
}

// Parameter Info

FrameLib_Future::ParameterInfo FrameLib_Future::sParamInfo;

FrameLib_Future::ParameterInfo::ParameterInfo()
{
    add("Sets the time of the next frame.");
    add("Sets the time units used to set the interval between frames.");
}

void FrameLib_Future::calculateTime()
{
    FrameLib_TimeFormat time = mParameters.getValue(kTime);
    FrameLib_TimeFormat now = getCurrentTime();
    
    switch (static_cast<Units>(mParameters.getValue(kUnits)))
    {
        case kMS:       time *= mSamplingRate / 1000.0;     break;
        case kSeconds:  time *= mSamplingRate;              break;
        case kSamples:  break;
    }
    
    if (mParameters.getInt(kMode))
        time += now;
    else
        time += FrameLib_TimeFormat(1);
   
    mTime = time;
}

void FrameLib_Future::update()
{
    if (mParameters.changed(kTime))
        calculateTime();
}

// Schedule

FrameLib_Future::SchedulerInfo FrameLib_Future::schedule(bool newFrame, bool noAdvance)
{
    FrameLib_TimeFormat now = getCurrentTime();
    
    if (now < mTime)
        return SchedulerInfo(mTime - now, false, true);
    
    FrameLib_TimeFormat inputTime = getInputTime();
    FrameLib_TimeFormat blockEndTime = getBlockEndTime();
    
    if (blockEndTime < inputTime)
        return SchedulerInfo(blockEndTime - now, false, false);
    
    if (now < inputTime)
        return SchedulerInfo(inputTime - now, false, false);

    return SchedulerInfo();
}
