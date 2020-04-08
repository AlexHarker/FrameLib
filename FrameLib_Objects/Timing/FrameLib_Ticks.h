
#ifndef FRAMELIB_TICKS_H
#define FRAMELIB_TICKS_H

#include "FrameLib_DSP.h"

class FrameLib_Ticks final : public FrameLib_Processor
{
    enum ParameterList { kLimit, kDirection, kReset, kMode, kIdleMode };
    enum Modes { kRun, kLoop, kPause, kStop };
    enum Direction { kUp, kDown };
    enum IdleModes { kRepeat, kEmpty };

    struct ParameterInfo : public FrameLib_Parameters::Info { ParameterInfo(); };

public:
    
    // Constructor
    
    FrameLib_Ticks(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy);
    
    // Info
    
    std::string objectInfo(bool verbose) override;
    std::string inputInfo(unsigned long idx, bool verbose) override;
    std::string outputInfo(unsigned long idx, bool verbose) override;
    
private:
    
    // Object Reset

    void objectReset() override;
    
    // Process
    
    void process() override;

    long mCounter;
    FrameLib_TimeFormat mLastResetTime;

    static ParameterInfo sParamInfo;
};

#endif
