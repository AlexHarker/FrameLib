
#ifndef FRAMELIB_CONSTANT_H
#define FRAMELIB_CONSTANT_H

#include "FrameLib_DSP.h"

class FrameLib_Constant final : public FrameLib_Processor
{
    // Parameter Enums and Info

    enum ParameterList { kConstant, kMultiplier };
    enum Constants { kPI, kE, kSqrt2, kInvPI, kInvE, kInvSqrt2, kEpsilon };

    struct ParameterInfo : public FrameLib_Parameters::Info { ParameterInfo(); };

public:
    
    // Constructor
    
    FrameLib_Constant(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy);
    
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
