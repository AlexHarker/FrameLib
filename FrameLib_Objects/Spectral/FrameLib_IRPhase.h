
#ifndef FRAMELIB_IRPHASE_H
#define FRAMELIB_IRPHASE_H

#include "FrameLib_DSP.h"
#include "../../FrameLib_Dependencies/HISSTools_IR_Manipulation/HISSTools_IR_Manipulation.h"

// FIX - add zero padding (why not do this prior to FFT with the pad object?)

class FrameLib_IRPhase final : public FrameLib_Processor
{
    // Parameter Enums and Info

    enum ParameterList { kMaxLength, kPhase };
    
    struct ParameterInfo : public FrameLib_Parameters::Info { ParameterInfo(); };

public:

    // Constructor / Destructor

    FrameLib_IRPhase(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy);
    ~FrameLib_IRPhase();
    
    // Info
    
    std::string objectInfo(bool verbose) override;
    std::string inputInfo(unsigned long idx, bool verbose) override;
    std::string outputInfo(unsigned long idx, bool verbose) override;

private:

    // Process
    
    void process() override;
    
    // Data
    
    // FFT Setup

    FFT_SETUP_D mFFTSetup;
    
    // Instantiation Params
    
    unsigned long mMaxFFTSize;
    
    static ParameterInfo sParamInfo;
};

#endif
