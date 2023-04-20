
#ifndef FRAMELIB_##CLASS_UPPER##_H
#define FRAMELIB_##CLASS_UPPER##_H

#include "FrameLib_DSP.h"

class FrameLib_##CLASS## final : public FrameLib_Processor
{
    enum ParameterList { };

    struct ParameterInfo : public FrameLib_Parameters::Info { ParameterInfo(); };

public:
    
    // Constructor
    
    FrameLib_##CLASS##(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy);
    
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
