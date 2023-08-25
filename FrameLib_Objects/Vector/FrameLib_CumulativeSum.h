
#ifndef FRAMELIB_CUMULATIVESUM_H
#define FRAMELIB_CUMULATIVESUM_H

#include "FrameLib_DSP.h"

class FrameLib_CumulativeSum final : public FrameLib_Processor
{
    enum ParameterList { };

    struct ParameterInfo : public FrameLib_Parameters::Info { ParameterInfo(); };

public:
    
    // Constructor
    
    FrameLib_CumulativeSum(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy);
    
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
