
#ifndef FRAMELIB_CHAIN_H
#define FRAMELIB_CHAIN_H

#include "FrameLib_DSP.h"

class FrameLib_Chain final : public FrameLib_Scheduler
{
    // Parameter Enums and Info

    enum ParameterList { kUnits, kTimeMode, kMode };
    enum Units { kSamples, kMS, kSeconds };
    enum TimeModes { kAbsolute, kRelative, kInterval };
    enum Modes { kReplace, kAdd, kAppend };

    struct ParameterInfo : public FrameLib_Parameters::Info { ParameterInfo(); };

public:
    
    // Constructor
    
    FrameLib_Chain(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy);
    
    // Info
    
    std::string objectInfo(bool verbose) override;
    std::string inputInfo(unsigned long idx, bool verbose) override;
    std::string outputInfo(unsigned long idx, bool verbose) override;
    
private:

    // Convert Time
    
    FrameLib_TimeFormat convertTime(double time, Units units);

    // Object Reset
    
    void objectReset() override;
    
    // Schedule
    
    SchedulerInfo schedule(bool newFrame, bool noAdvance) override;
    
    // Data
    
    static ParameterInfo sParamInfo;
    
    AutoArray<FrameLib_TimeFormat> mTimes;
    unsigned long mSize;
    unsigned long mPosition;
};

#endif
