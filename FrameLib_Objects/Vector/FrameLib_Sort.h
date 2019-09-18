
#ifndef FRAMELIB_SORT_H
#define FRAMELIB_SORT_H

#include "FrameLib_DSP.h"

class FrameLib_Sort final : public FrameLib_Processor
{
    // Parameter Enums and Info

    enum ParameterList { kOrder, kOutputIndices };
    enum Orders { kUp, kDown };

    struct ParameterInfo : public FrameLib_Parameters::Info { ParameterInfo(); };

public:
    
    // Constructor
    
    FrameLib_Sort(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy);
    
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
