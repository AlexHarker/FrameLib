
#ifndef FRAMELIB_ONCE_H
#define FRAMELIB_ONCE_H

#include "FrameLib_DSP.h"

class FrameLib_Once : public FrameLib_Scheduler
{

public:
    
    // Constructor
    
    FrameLib_Once(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy)
    : FrameLib_Scheduler(context, proxy, nullptr, 1, 1) {}
    
    // Info
    
    std::string objectInfo(bool verbose);
    std::string inputInfo(unsigned long idx, bool verbose);
    std::string outputInfo(unsigned long idx, bool verbose);
    
private:
    
    // Schedule
    
    SchedulerInfo schedule(bool newFrame, bool noAdvance);
};

#endif
