
#ifndef FRAMELIB_RAMP_H
#define FRAMELIB_RAMP_H

#include "FrameLib_DSP.h"

class FrameLib_Ramp : public FrameLib_Processor
{
    // Parameter Enums and Info
    
    enum ParameterList { kMode, kLength, kUnits, kScale };
    enum Modes { kRequestedLength, kInLength };
    enum Units { kSamples, kMS, kSeconds };
    enum Scales { kScaleSamples, kScaleMS, kScaleSeconds, kScaleNormalised };

    struct ParameterInfo : public FrameLib_Parameters::Info { ParameterInfo(); };

public:
    
    // Constructor
    
    FrameLib_Ramp(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy);
    
    // Info
    
    std::string objectInfo(bool verbose);
    std::string inputInfo(unsigned long idx, bool verbose);
    std::string outputInfo(unsigned long idx, bool verbose);

private:
    
    // Helpers
    
    unsigned long getLength();

    // Process
    
    void process();
    
    // Data
    
    static ParameterInfo sParamInfo;
};

#endif
