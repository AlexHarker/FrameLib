
#include "FrameLib_Once.h"

// Constructor

FrameLib_Once::FrameLib_Once(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy)
: FrameLib_Scheduler(context, proxy, nullptr, 1, 1)
{
    mParameters.set(serialisedParameters);
}


// Info

std::string FrameLib_Once::objectInfo(bool verbose)
{
    return formatInfo("Schedules a single frame at the begining of time that lasts forever: "
                      "The output is an empty frame.",
                      "Schedules a single  frame at the begining of time that lasts forever.", verbose);
}

std::string FrameLib_Once::inputInfo(unsigned long idx, bool verbose)
{
    return formatInfo("Synchronisation Input - provided only to aid synchronisation", "Sync Input", verbose);
}

std::string FrameLib_Once::outputInfo(unsigned long idx, bool verbose)
{
    return formatInfo("Trigger Output - outputs empty frames", "Trigger Output", verbose);
}

// Schedule

FrameLib_Once::SchedulerInfo FrameLib_Once::schedule(bool newFrame, bool noAdvance)
{
    return SchedulerInfo(FrameLib_TimeFormat::largest() - getValidTime(), getValidTime() == FrameLib_TimeFormat(1), true);
}
