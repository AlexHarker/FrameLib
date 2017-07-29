
#ifndef FRAMELIB_INTERVAL_H
#define FRAMELIB_INTERVAL_H

#include "FrameLib_DSP.h"

class FrameLib_Interval : public FrameLib_Scheduler
{
    // Parameter Enums and Info

    enum ParameterList { kInterval, kUnits };
    enum Units { kSamples, kMS, kSeconds };

public:
    
    // Constructor
    
    FrameLib_Interval(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner);
    
private:
    
    // Scheduler
    
    SchedulerInfo schedule(bool newFrame, bool noOutput);};

#endif
