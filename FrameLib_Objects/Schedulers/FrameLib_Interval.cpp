
#include "FrameLib_Interval.h"

FrameLib_Interval::FrameLib_Interval(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner) : FrameLib_Scheduler(context, &sParamInfo, 1, 1)
{
    mParameters.addDouble(kInterval, "interval", 64, 0);
    mParameters.setMin(0);
    
    mParameters.addEnum(kUnits, "units");
    mParameters.addEnumItem(kSamples, "samples");
    mParameters.addEnumItem(kMS, "ms");
    mParameters.addEnumItem(kSeconds, "seconds");
    
    mParameters.set(serialisedParameters);
    
    setParameterInput(0);
    
    update();
}

// Info

std::string FrameLib_Interval::objectInfo(bool verbose)
{
    return getInfo("Schedules frames at regular intervals, which can be adjusted using the interval parameter: Output frames are empty.",
                   "Schedules frames at regular intervals, which can be adjusted using the interval parameter.", verbose);
}

std::string FrameLib_Interval::inputInfo(unsigned long idx, bool verbose)
{
    return getInfo("Parameter Update - tagged input updates parameters", "Parameter Update", verbose);
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

// Update and Schedule

void FrameLib_Interval::update()
{
    if (mParameters.changed(kInterval))
    {
        FrameLib_TimeFormat interval = mParameters.getValue(kInterval);
    
        switch ((Units) (mParameters.getValue(kUnits)))
        {
            case kMS:
                interval *= mSamplingRate / 1000.0;
                break;
                
            case kSeconds:
                interval *= mSamplingRate;
                break;
                
            case kSamples:
                break;
        }
        
        if (!interval)
            interval = FL_Limits<FrameLib_TimeFormat>::smallest();
        
        mInterval = interval;
    }
}

FrameLib_Interval::SchedulerInfo FrameLib_Interval::schedule(bool newFrame, bool noOutput)
{
    return SchedulerInfo(mInterval, true, true);
}
