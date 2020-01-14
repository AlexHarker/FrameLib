
#include "FrameLib_Interval.h"

FrameLib_Interval::FrameLib_Interval(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy) : FrameLib_Scheduler(context, proxy, &sParamInfo, 1, 1)
{
    mParameters.addDouble(kInterval, "interval", 64, 0);
    mParameters.setMin(0);
    
    mParameters.addEnum(kUnits, "units", 1);
    mParameters.addEnumItem(kSamples, "samples");
    mParameters.addEnumItem(kMS, "ms");
    mParameters.addEnumItem(kSeconds, "seconds");
    mParameters.addEnumItem(kHz, "hz");
    
    mParameters.addBool(kOn, "on", true, 2);

    mParameters.addBool(kSwitchable, "switchable", false);
    mParameters.setInstantiation();
    
    mParameters.set(serialisedParameters);
    
    mSwitchable = mParameters.getBool(kSwitchable);
    
    setParameterInput(0);
    
    calculateInterval();
}

// Info

std::string FrameLib_Interval::objectInfo(bool verbose)
{
    return formatInfo("Schedules frames at regular intervals, which can be adjusted using the interval parameter: The output is an empty frame.",
                   "Schedules frames at regular intervals, which can be adjusted using the interval parameter.", verbose);
}

std::string FrameLib_Interval::inputInfo(unsigned long idx, bool verbose)
{
    return parameterInputInfo(verbose);
}

std::string FrameLib_Interval::outputInfo(unsigned long idx, bool verbose)
{
    return "Empty Trigger Frames";
}

// Parameter Info

FrameLib_Interval::ParameterInfo FrameLib_Interval::sParamInfo;

FrameLib_Interval::ParameterInfo::ParameterInfo()
{
    add("Sets the interval between frames in the units of the units parameter.");
    add("Sets the time units used to set the interval between frames.");
}

// Calculate Interval

void FrameLib_Interval::calculateInterval()
{
    FrameLib_TimeFormat interval = mParameters.getValue(kInterval);
    
    switch (static_cast<Units>(mParameters.getInt(kUnits)))
    {
        case kHz:           interval = hzToSamples(interval);           break;
        case kMS:           interval = msToSamples(interval);           break;
        case kSeconds:      interval = secondsToSamples(interval);      break;
        case kSamples:      break;
    }
    
    if (!interval)
        interval = FrameLib_TimeFormat::smallest();
    
    mInterval = interval;
}

// Object Reset

void FrameLib_Interval::objectReset()
{
    mRemaining = FrameLib_TimeFormat();
    calculateInterval();
}

// Update and Schedule

void FrameLib_Interval::update()
{
    if (mParameters.changed(kInterval) || mParameters.changed(kUnits))
        calculateInterval();
}

FrameLib_Interval::SchedulerInfo FrameLib_Interval::schedule(bool newFrame, bool noAdvance)
{
    bool on = mParameters.getBool(kOn) || !mSwitchable;
    bool timeRemaining = mRemaining.greaterThanZero();
    
    FrameLib_TimeFormat inputTime = getInputTime();
    FrameLib_TimeFormat now = getCurrentTime();
    FrameLib_TimeFormat interval = timeRemaining ? mRemaining : mInterval;
    
    // Calculate if we can schedule up to the next new frame
    
    bool complete = !mSwitchable || (on && (now + interval) < inputTime);
    
    // Adjust the interval if we need to track input / block updates or if not allowed to update
    
    interval = complete ? interval : std::min(inputTime, getBlockEndTime()) - now;
    interval = noAdvance ? FrameLib_TimeFormat() : interval;
    
    // Update the remaining time
    
    if ((on && timeRemaining) || noAdvance)
        mRemaining -= interval;
    else if (on)
        mRemaining = mInterval - interval;
    else
        mRemaining = FrameLib_TimeFormat();
    
    return SchedulerInfo(interval, on && !timeRemaining, complete);
}
