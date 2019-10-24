
#ifndef FRAMELIB_COMBINETIME_H
#define FRAMELIB_COMBINETIME_H

#include "FrameLib_DSP.h"

class FrameLib_CombineTime final : public FrameLib_Processor
{
    // Parameter Enums and Info
    const static int maxNumIns = 32;
    enum ParameterList { kNumIns };

    struct ParameterInfo : public FrameLib_Parameters::Info { ParameterInfo(); };

public:
    
    // Constructor
    
    FrameLib_CombineTime(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy);
    
    // Info
    
    std::string objectInfo(bool verbose) override;
    std::string inputInfo(unsigned long idx, bool verbose) override;
    std::string outputInfo(unsigned long idx, bool verbose) override;
    
private:
    
    // Process
    
    void process() override;
    
    // Data
    
    long mNumIns;
    static ParameterInfo sParamInfo;
};

#endif
