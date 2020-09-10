
#ifndef FRAMELIB_PATTERN_H
#define FRAMELIB_PATTERN_H

#include "FrameLib_DSP.h"

class FrameLib_Pattern final : public FrameLib_Processor
{
    enum ParameterList { kMode, kDirection, kReset, kIdleMode };
    enum Modes { kRun, kLoop, kPause, kStop };
    enum Direction { kForward, kBackward };
    enum IdleModes { kRepeat, kEmpty };

    struct ParameterInfo : public FrameLib_Parameters::Info { ParameterInfo(); };

public:
    
    // Constructor

    FrameLib_Pattern(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy);
    
    // Info
    
    std::string objectInfo(bool verbose) override;
    std::string inputInfo(unsigned long idx, bool verbose) override;
    std::string outputInfo(unsigned long idx, bool verbose) override;

private:
    
    // Object Reset
    
    void objectReset() override;
    
    // Process
    
    void process() override;
    
    // Data
    
    FrameLib_TimeFormat mLastResetTime;
    
    long mPosition;
    
    static ParameterInfo sParamInfo;
};

#endif
