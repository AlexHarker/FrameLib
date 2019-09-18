
#ifndef FRAMELIB_FUTURE_H
#define FRAMELIB_FUTURE_H

#include "FrameLib_DSP.h"

class FrameLib_Future final : public FrameLib_Scheduler
{
    // Parameter Enums and Info

    enum ParameterList { kTime, kUnits, kMode };
    enum Units { kSamples, kMS, kSeconds };

    struct ParameterInfo : public FrameLib_Parameters::Info { ParameterInfo(); };

public:
    
    // Constructor
    
    FrameLib_Future(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy);
    
    // Info
    
    std::string objectInfo(bool verbose) override;
    std::string inputInfo(unsigned long idx, bool verbose) override;
    std::string outputInfo(unsigned long idx, bool verbose) override;
    
private:

    // Calculate Time
    
    void calculateTime();
    
    // Update and schedule
    
    void update() override;
    SchedulerInfo schedule(bool newFrame, bool noAdvance) override;
    
    // Data
    
    static ParameterInfo sParamInfo;
    
    FrameLib_TimeFormat mTime;
};

#endif
