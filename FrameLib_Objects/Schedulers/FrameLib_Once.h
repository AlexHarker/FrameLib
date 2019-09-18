
#ifndef FRAMELIB_ONCE_H
#define FRAMELIB_ONCE_H

#include "FrameLib_DSP.h"

class FrameLib_Once final : public FrameLib_Scheduler
{

public:
    
    // Constructor
    
    FrameLib_Once(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy)
    : FrameLib_Scheduler(context, proxy, nullptr, 1, 1) {}
    
    // Info
    
    std::string objectInfo(bool verbose) override;
    std::string inputInfo(unsigned long idx, bool verbose) override;
    std::string outputInfo(unsigned long idx, bool verbose) override;
    
private:
    
    // Schedule
    
    SchedulerInfo schedule(bool newFrame, bool noAdvance) override;
};

#endif
