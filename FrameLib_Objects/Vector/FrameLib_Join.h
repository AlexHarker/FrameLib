
#ifndef FRAMELIB_JOIN_H
#define FRAMELIB_JOIN_H

#include "FrameLib_DSP.h"

class FrameLib_Join final : public FrameLib_Processor
{
    static constexpr int maxNumIns = 32;

    // Parameter Enums and Info

    enum ParameterList { kNumIns, kTriggers };
    
    struct ParameterInfo : public FrameLib_Parameters::Info { ParameterInfo(); };

public:

    // Constructor

    FrameLib_Join(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy);
    
    // Info
    
    std::string objectInfo(bool verbose) override;
    std::string inputInfo(unsigned long idx, bool verbose) override;
    std::string outputInfo(unsigned long idx, bool verbose) override;

private:
    
    // Process
    
    void process() override;
    
    // Data
    
    static ParameterInfo sParamInfo;
    
    unsigned long mNumIns;
};

#endif
