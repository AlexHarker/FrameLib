

#ifndef FRAMELIB_PERBLOCK_H
#define FRAMELIB_PERBLOCK_H

#include "FrameLib_DSP.h"

class FrameLib_PerBlock final : public FrameLib_Scheduler
{
public:
    
    // Constructor
    
    FrameLib_PerBlock(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy);
    
    // Info
    
    std::string objectInfo(bool verbose) override;
    std::string inputInfo(unsigned long idx, bool verbose) override;
    std::string outputInfo(unsigned long idx, bool verbose) override;
    
private:
    
    // Schedule
    
    SchedulerInfo schedule(bool newFrame, bool noAdvance) override;
};

#endif
