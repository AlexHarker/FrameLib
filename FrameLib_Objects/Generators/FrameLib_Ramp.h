
#ifndef FRAMELIB_RAMP_H
#define FRAMELIB_RAMP_H

#include "FrameLib_DSP.h"

class FrameLib_Ramp : public FrameLib_Processor
{
    // Parameter Enums and Info
    
    enum ParameterList { kMode, kLength, kScale };
    enum Modes { kRequestedLength, kInLength };
    enum Scales { kSamples, kMS, kSeconds, kNormalised };

    struct ParameterInfo : public FrameLib_Parameters::Info { ParameterInfo(); };

public:
    
    // Constructor
    
    FrameLib_Ramp(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner);
    
    // Info
    
    std::string objectInfo(bool verbose);
    std::string inputInfo(unsigned long idx, bool verbose);
    std::string outputInfo(unsigned long idx, bool verbose);

private:
    
    // Process
    
    void process();
    
    // Data
    
    static ParameterInfo sParamInfo;
};

#endif
