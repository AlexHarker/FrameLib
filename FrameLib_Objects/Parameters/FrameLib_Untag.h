
#ifndef FRAMELIB_UNTAG_H
#define FRAMELIB_UNTAG_H

#include "FrameLib_DSP.h"

class FrameLib_Untag final : public FrameLib_Processor
{
    const static int maxNumOuts = 10;
    
    // Parameter Enums and Info

    enum ParameterList { kNumOuts, kNames };
    
    struct ParameterInfo : public FrameLib_Parameters::Info { ParameterInfo(); };

public:
    
    // Constructor
    
    FrameLib_Untag(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy);
    
    // Info
    
    std::string objectInfo(bool verbose) override;
    std::string inputInfo(unsigned long idx, bool verbose) override;
    std::string outputInfo(unsigned long idx, bool verbose) override;
    
private:
    
    // Process
    
    void process() override;
    
    // Data
    
    unsigned long mNumOuts;
    
    static ParameterInfo sParamInfo;
};

#endif
