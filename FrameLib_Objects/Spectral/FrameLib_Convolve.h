
#ifndef FRAMELIB_CONVOLVE_H
#define FRAMELIB_CONVOLVE_H

#include "FrameLib_DSP.h"
#include "FrameLib_Spectral_Processor.h"

class FrameLib_Convolve final : public FrameLib_Processor, private Spectral_Processor
{
    // Parameter Enums and Info

    enum ParameterList { kMaxLength, kMode, kEdgeMode };
    enum Mode { kReal, kComplex };
    
    struct ParameterInfo : public FrameLib_Parameters::Info { ParameterInfo(); };

public:

    // Constructor
    
    FrameLib_Convolve(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy);
    
    // Info
    
    std::string objectInfo(bool verbose) override;
    std::string inputInfo(unsigned long idx, bool verbose) override;
    std::string outputInfo(unsigned long idx, bool verbose) override;

private:

    // Process
    
    void process() override;

private:
    
    // Data
    
    Mode mMode;

    static ParameterInfo sParamInfo;
};

#endif
