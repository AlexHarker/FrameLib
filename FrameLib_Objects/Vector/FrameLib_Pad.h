
#ifndef FRAMELIB_PAD_H
#define FRAMELIB_PAD_H

#include "FrameLib_DSP.h"

class FrameLib_Pad final : public FrameLib_Processor
{
    // Parameter Enums and Info

    enum ParameterList { kPadding, kStart, kEnd, kUnits };
    enum Units { kSamples, kRatio };
    
    struct ParameterInfo : public FrameLib_Parameters::Info { ParameterInfo(); };

public:

    // Constructor
    
    FrameLib_Pad(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy);
    
    // Info
    
    std::string objectInfo(bool verbose) override;
    std::string inputInfo(unsigned long idx, bool verbose) override;
    std::string outputInfo(unsigned long idx, bool verbose) override;

private:
    
    // Process
    
    void process() override;
    
    // Data
    
    static ParameterInfo sParamInfo;
};

#endif
