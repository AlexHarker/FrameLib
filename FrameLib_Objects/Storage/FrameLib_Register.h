
#ifndef FRAMELIB_REGISTER_H
#define FRAMELIB_REGISTER_H

#include "FrameLib_DSP.h"

class FrameLib_Register final : public FrameLib_Processor
{
    // Parameter Enums and Info

    enum ParameterList { kMode };
    enum Modes { kStore, kPass };
    
    struct ParameterInfo : public FrameLib_Parameters::Info { ParameterInfo(); };

public:

    // Constructor
    
    FrameLib_Register(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy);
    
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
