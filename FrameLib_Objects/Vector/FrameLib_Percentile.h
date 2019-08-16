
#ifndef FRAMELIB_PERCENTILE_H
#define FRAMELIB_PERCENTILE_H

#include "FrameLib_DSP.h"

class FrameLib_Percentile final : public FrameLib_Processor
{
    // Parameter Enums and Info

    enum ParameterList { kPercentile };
    
    struct ParameterInfo : public FrameLib_Parameters::Info { ParameterInfo(); };

public:
    
    // Constructor
    
    FrameLib_Percentile(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy);
    
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
