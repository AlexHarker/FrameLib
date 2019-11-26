
#ifndef FRAMELIB_TIMEDELTA_H
#define FRAMELIB_TIMEDELTA_H

#include "FrameLib_DSP.h"

class FrameLib_TimeDelta final : public FrameLib_Processor
{
    // Parameter Enums and Info

    enum ParameterList { kUnits };
    enum Units { kSamples, kMS, kSeconds };

    struct ParameterInfo : public FrameLib_Parameters::Info { ParameterInfo(); };

public:
    
    // Constructor
    
    FrameLib_TimeDelta(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy);
    
    // Info
    
    std::string objectInfo(bool verbose) override;
    std::string inputInfo(unsigned long idx, bool verbose) override;
    std::string outputInfo(unsigned long idx, bool verbose) override;
    
private:
    
    // Calculate Multiplier
    
    void calculateMultiplier();
    
    // Object Reset
    
    void objectReset() override;
    
    // Update and Process
    
    void update() override;
    void process() override;
    
    // Data

    FrameLib_TimeFormat mMultiplier;
    FrameLib_TimeFormat mLastTime;
    
    static ParameterInfo sParamInfo;
};

#endif
