
#ifndef FRAMELIB_INTERVAL_H
#define FRAMELIB_INTERVAL_H

#include "FrameLib_DSP.h"

class FrameLib_Interval final : public FrameLib_Scheduler
{
    // Parameter Enums and Info

    enum ParameterList { kInterval, kUnits };
    enum Units { kSamples, kMS, kSeconds, kHz };

    struct ParameterInfo : public FrameLib_Parameters::Info { ParameterInfo(); };

public:
    
    // Constructor
    
    FrameLib_Interval(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy);
    
    // Info
    
    std::string objectInfo(bool verbose) override;
    std::string inputInfo(unsigned long idx, bool verbose) override;
    std::string outputInfo(unsigned long idx, bool verbose) override;
    
private:
    
    // Calculate Interval
    
    void calculateInterval();
    
    // Object Reset
    
    void objectReset() override { calculateInterval(); }
    
    // Update and Schedule
    
    void update() override;
    SchedulerInfo schedule(bool newFrame, bool noAdvance) override;
    
    // Data

    FrameLib_TimeFormat mInterval;
    
    static ParameterInfo sParamInfo;
};

#endif
