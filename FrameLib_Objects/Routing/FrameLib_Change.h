
#ifndef FRAMELIB_CHANGE_H
#define FRAMELIB_CHANGE_H

#include "FrameLib_DSP.h"

class FrameLib_Change final : public FrameLib_Processor
{
    // Parameter Enums and Info

    enum ParameterList { kNumIns, kMode };
    enum Modes { kLow, kHigh };
    
    struct ParameterInfo : public FrameLib_Parameters::Info { ParameterInfo(); };

public:

    // Constructor

    FrameLib_Change(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy);
    
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
