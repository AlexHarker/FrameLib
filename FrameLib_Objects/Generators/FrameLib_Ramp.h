
#ifndef FRAMELIB_RAMP_H
#define FRAMELIB_RAMP_H

#include "FrameLib_DSP.h"

class FrameLib_Ramp final : public FrameLib_Processor
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
    
    std::string objectInfo(bool verbose) override;
    std::string inputInfo(unsigned long idx, bool verbose) override;
    std::string outputInfo(unsigned long idx, bool verbose) override;

private:
    
    // Helpers
    
    unsigned long getLength();

    // Process
    
    void process() override;
    
    // Data
    
    static ParameterInfo sParamInfo;
};

#endif
