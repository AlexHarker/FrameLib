

#ifndef FRAMELIB_PERBLOCK_H
#define FRAMELIB_PERBLOCK_H

#include "FrameLib_DSP.h"

class FrameLib_PerBlock : public FrameLib_Scheduler
{
    
public:
    
    // Constructor
    
    FrameLib_PerBlock(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner);
    
    // Info
    
    std::string objectInfo(bool verbose);
    std::string inputInfo(unsigned long idx, bool verbose);
    std::string outputInfo(unsigned long idx, bool verbose);
    
private:
    
    // Schedule
    
    SchedulerInfo schedule(bool newFrame, bool noAdvance);
};

#endif
