
#ifndef FRAMELIB_INTERVAL_H
#define FRAMELIB_INTERVAL_H

#include "FrameLib_DSP.h"

class FrameLib_Interval : public FrameLib_Scheduler, private FrameLib_Info
{
    // Parameter Enums and Info

    enum ParameterList { kInterval, kUnits };
    enum Units { kSamples, kMS, kSeconds };

    struct ParameterInfo : public FrameLib_Parameters::Info { ParameterInfo(); };

public:
    
    // Constructor
    
    FrameLib_Interval(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner);
    
    // Info
    
    const char *objectInfo(bool verbose);
    const char *inputInfo(unsigned long idx, bool verbose);
    const char *outputInfo(unsigned long idx, bool verbose);
    
private:
    
    // Scheduler
    
    SchedulerInfo schedule(bool newFrame, bool noOutput);
    
    // Data

    static ParameterInfo sParamInfo;
};

#endif
