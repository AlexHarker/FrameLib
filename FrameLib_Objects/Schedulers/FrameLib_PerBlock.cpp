
#include "FrameLib_PerBlock.h"

// Constructor

FrameLib_PerBlock::FrameLib_PerBlock(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner) : FrameLib_Scheduler(context, NULL, 1, 1, 0) {}

// Info

std::string FrameLib_PerBlock::objectInfo(bool verbose)
{
    return getInfo("Schedules one frame at the start of each of the hosts audio blocks: The output is an empty vector.",
                   "Schedules one frame at the start of each of the hosts audio blocks.", verbose);
}

std::string FrameLib_PerBlock::inputInfo(unsigned long idx, bool verbose)
{
    return getInfo("Synchronisation Input - input is ignored, but can be used for synchronsation purposes", "Synchronisation Input", verbose);
}

std::string FrameLib_PerBlock::outputInfo(unsigned long idx, bool verbose)
{
    return "Empty Trigger Frames";
}

FrameLib_PerBlock::SchedulerInfo FrameLib_PerBlock::schedule(bool newFrame, bool noAdvance)
{
    return SchedulerInfo(getBlockEndTime() - getCurrentTime(), true, true);
}
