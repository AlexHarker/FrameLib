
#include "FrameLib_Once.h"

// Info

std::string FrameLib_Once::objectInfo(bool verbose)
{
    return getInfo("Schedules one frame at the begining of time, lasting forever: The output frame is empty.",
                   "Schedules one frame at the begining of time, lasting forever.", verbose);
}

std::string FrameLib_Once::inputInfo(unsigned long idx, bool verbose)
{
    return getInfo("Synchronisation Input - input is ignored, but can be used for synchronsation purposes", "Synchronisation Input", verbose);
}

std::string FrameLib_Once::outputInfo(unsigned long idx, bool verbose)
{
    return "Empty Trigger Frames";
}

// Schedule

FrameLib_Once::SchedulerInfo FrameLib_Once::schedule(bool newFrame, bool noAdvance)
{
    return SchedulerInfo(FL_Limits<FrameLib_TimeFormat>::largest() - getValidTime(), getValidTime() == FrameLib_TimeFormat(1), true);
}
