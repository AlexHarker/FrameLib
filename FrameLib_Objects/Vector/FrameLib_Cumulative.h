
#ifndef FRAMELIB_CUMULATIVE_H
#define FRAMELIB_CUMULATIVE_H

#include "FrameLib_DSP.h"

class FrameLib_Cumulative final : public FrameLib_Processor
{
    // Parameter Enums and Info

    enum ParameterList { kMode };
    enum Modes { kSum, kProduct };

    struct ParameterInfo : public FrameLib_Parameters::Info { ParameterInfo(); };

public:
    
    // Constructor
    
    FrameLib_Cumulative(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy);
    
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
