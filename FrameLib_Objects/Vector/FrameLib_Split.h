
#ifndef FRAMELIB_SPLIT_H
#define FRAMELIB_SPLIT_H

#include "FrameLib_DSP.h"

// FIX - consider multi-output version later

class FrameLib_Split final : public FrameLib_Processor
{
    // Parameter Enums and Info

    enum ParameterList { kSplit, kUnits };
    enum Units { kSamples, kRatio };

    struct ParameterInfo : public FrameLib_Parameters::Info { ParameterInfo(); };

public:

    // Constructor
    
    FrameLib_Split(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy);
    
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
