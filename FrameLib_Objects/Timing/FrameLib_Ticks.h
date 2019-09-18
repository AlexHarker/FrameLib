
#ifndef FRAMELIB_TICKS_H
#define FRAMELIB_TICKS_H

#include "FrameLib_DSP.h"

class FrameLib_Ticks final : public FrameLib_Processor
{
    enum ParameterList {kLimit, kSetValue, kLimitMode, kRunMode};
    enum ModesLimit {kProgress, kRestart};
    enum ModesRun {kRun, kPause, kStop};
    

    struct ParameterInfo : public FrameLib_Parameters::Info { ParameterInfo(); };

public:
    
    // Constructor
    
    FrameLib_Ticks(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy);
    
    // Info
    
    std::string objectInfo(bool verbose) override;
    std::string inputInfo(unsigned long idx, bool verbose) override;
    std::string outputInfo(unsigned long idx, bool verbose) override;
    
private:
    unsigned long counter;
    unsigned long previousLimit;
    unsigned long previousValue;
    bool valueInPause;
    bool limitInPause;
    
    // Process
    
    void process() override;
    
    static ParameterInfo sParamInfo;
};

#endif
