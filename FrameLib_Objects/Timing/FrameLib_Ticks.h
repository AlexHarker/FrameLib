
#ifndef FRAMELIB_Ticks_H
#define FRAMELIB_Ticks_H

#include "FrameLib_DSP.h"

class FrameLib_Ticks : public FrameLib_Processor
{
    enum ParameterList {kLimit, kSetValue, kLimitMode, kRunMode};
    enum ModesLimit {kProgress, kRestart};
    enum ModesRun {kRun, kPause, kStop};
    

    struct ParameterInfo : public FrameLib_Parameters::Info { ParameterInfo(); };

public:
    
    // Constructor
    
    FrameLib_Ticks(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner);
    
    // Info
    
    std::string objectInfo(bool verbose);
    std::string inputInfo(unsigned long idx, bool verbose);
    std::string outputInfo(unsigned long idx, bool verbose);
    
private:
    unsigned long counter;
    unsigned long previousLimit;
    unsigned long previousValue;
    bool valueInPause;
    bool limitInPause;
    
    // Process
    
    void process();
    
    static ParameterInfo sParamInfo;
};

#endif
