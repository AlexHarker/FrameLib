
#ifndef FRAMELIB_SHIFT_H
#define FRAMELIB_SHIFT_H

#include "FrameLib_DSP.h"

class FrameLib_Shift final : public FrameLib_Processor
{
    // Parameter Enums and Info

    enum ParameterList { kShift, kPadding, kMode, kUnits };
    enum Modes { kPad, kWrap };
    enum Units { kSamples, kRatio };
    
    struct ParameterInfo : public FrameLib_Parameters::Info { ParameterInfo(); };

public:

    // Constructor

    FrameLib_Shift(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy);
    
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
