
#ifndef FRAMELIB_INTERVAL_H
#define FRAMELIB_INTERVAL_H

#include "FrameLib_DSP.h"

class FrameLib_Interval : public FrameLib_Scheduler
{
    // Parameter Enums and Info

    enum ParameterList { kInterval, kUnits };
    enum Units { kSamples, kMS, kSeconds, kHz };

    struct ParameterInfo : public FrameLib_Parameters::Info { ParameterInfo(); };

public:
    
    // Constructor
    
    FrameLib_Interval(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy);
    
    // Info
    
    std::string objectInfo(bool verbose);
    std::string inputInfo(unsigned long idx, bool verbose);
    std::string outputInfo(unsigned long idx, bool verbose);
    
private:
    
    // Calculate Interval
    
    void calculateInterval();
    
    // Object Reset
    
    void objectReset() { calculateInterval(); }
    
    // Update and Schedule
    
    void update();
    SchedulerInfo schedule(bool newFrame, bool noAdvance);
    
    // Data

    FrameLib_TimeFormat mInterval;
    
    static ParameterInfo sParamInfo;
};

#endif
