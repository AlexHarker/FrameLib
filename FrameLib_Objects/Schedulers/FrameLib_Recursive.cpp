
#include "FrameLib_Recursive.h"

// Time object

FrameLib_Recursive::Time::Time(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy)
: FrameLib_Processor(context, proxy, NULL, 2, 0)
{}

// Info

std::string FrameLib_Recursive::Time::objectInfo(bool verbose)
{
    return formatInfo("Stores times for a connected recursive scheduler: "
                      "The first input takes single values (time till next frame). "
                      "The second input needs to be connected directly to the scheduler.",
                      "Stores times for a connected recursive scheduler.", verbose);
}

std::string FrameLib_Recursive::Time::inputInfo(unsigned long idx, bool verbose)
{
    if (idx)
        return formatInfo("Link Input - connect recursive scheduler", "Link Input", verbose);
    else
        return formatInfo("Time Input - stores scheduling times", "Time Input", verbose);
}

// Time Object Process

void FrameLib_Recursive::Time::process()
{
    unsigned long size = 0;
    const double *input = getInput(0, &size);
    
    if (mOwner)
        mOwner->mInterval = size ? input[0] : 0.0;
}

void FrameLib_Recursive::Time::objectReset()
{
    mOwner = dynamic_cast<FrameLib_Recursive *>(getConnection(1).mObject);
}


// Main object

FrameLib_Recursive::FrameLib_Recursive(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy)
: FrameLib_Scheduler(context, proxy, &sParamInfo, 1, 1)
{
    mParameters.addEnum(kUnits, "units", 0);
    mParameters.addEnumItem(kSamples, "samples");
    mParameters.addEnumItem(kMS, "ms");
    mParameters.addEnumItem(kSeconds, "seconds");
    mParameters.addEnumItem(kHz, "hz");

    mParameters.addBool(kAutoStart, "auto_start", false, 1);
    
    mParameters.set(serialisedParameters);
        
    addParameterInput();
    
    calculateInterval();
}

// Info

std::string FrameLib_Recursive::objectInfo(bool verbose)
{
    return formatInfo("Schedules frames based on downstream calculations: "
                      "A connected next time object is required to store time values downstream. "
                      "The output is an empty trigger frame. "
                      "Trigger frames at the input create output. "
                      "Values sent to the connected object set the time for the next frame.",
                      "Schedules frames based on downstream calculations.", verbose);
}

std::string FrameLib_Recursive::inputInfo(unsigned long idx, bool verbose)
{
    if (idx)
        return parameterInputInfo(verbose);
    else
        return formatInfo("Trigger Input - start scheduling", "Trigger Input", verbose);
}

std::string FrameLib_Recursive::outputInfo(unsigned long idx, bool verbose)
{
    return formatInfo("Trigger Output - outputs empty frames", "Trigger Output", verbose);
}

// Parameter Info

FrameLib_Recursive::ParameterInfo FrameLib_Recursive::sParamInfo;

FrameLib_Recursive::ParameterInfo::ParameterInfo()
{
    add("Sets the time units used for setting the interval.");
    add("Sets whether a frame is scheduled when DSP is started.");
}

// Calculate Interval

FrameLib_TimeFormat FrameLib_Recursive::calculateInterval()
{
    FrameLib_TimeFormat interval = mInterval;
    
    switch (mParameters.getEnum<Units>(kUnits))
    {
        case kSamples:      break;
        case kMS:           interval = msToSamples(interval);           break;
        case kSeconds:      interval = secondsToSamples(interval);      break;
        case kHz:           interval = hzToSamples(interval);           break;
    }
    
    if (!interval)
        return interval;
    
    return interval - FrameLib_TimeFormat(0, 1);
}

// Object Reset

void FrameLib_Recursive::objectReset()
{
    mInterval = 0;
}

// Schedule

FrameLib_Recursive::SchedulerInfo FrameLib_Recursive::schedule(bool newFrame, bool noAdvance)
{
    if (noAdvance)
        return SchedulerInfo();
    
    // FIX - currently can't interrupt a recursively scheduled time...
    
    bool done = !newFrame;
    bool autoStart = mParameters.getBool(kAutoStart);
    bool firstCall = getCurrentTime() == FrameLib_TimeFormat(1, 0);
    bool start = (getInputFrameTime(0) == getCurrentTime()) || (autoStart && firstCall);
    
    newFrame = newFrame || start;
    FrameLib_TimeFormat interval = newFrame ? FrameLib_TimeFormat(0, 1) : calculateInterval();
    
    if (!interval)
    {
        interval = getInputValidTime(0) - getCurrentTime();
        done = false;
    }
    
    // Schedule
    
    return SchedulerInfo(interval, newFrame, done);
}
