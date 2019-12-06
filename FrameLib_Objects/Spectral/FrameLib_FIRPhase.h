
#ifndef FRAMELIB_FIRPHASE_H
#define FRAMELIB_FIRPHASE_H

#include "FrameLib_DSP.h"
#include "../../FrameLib_Dependencies/SpectralProcessor.hpp"

// FIX - add zero padding (why not do this prior to FFT with the pad object?)

class FrameLib_FIRPhase final : public FrameLib_Processor
{
    // Parameter Enums and Info

    enum ParameterList { kMaxLength, kPhase, kTimeFactor };
    
    struct ParameterInfo : public FrameLib_Parameters::Info { ParameterInfo(); };

public:

    // Constructor

    FrameLib_FIRPhase(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy);
    
    // Info
    
    std::string objectInfo(bool verbose) override;
    std::string inputInfo(unsigned long idx, bool verbose) override;
    std::string outputInfo(unsigned long idx, bool verbose) override;

private:

    // Process
    
    void process() override;
    
    // Data

    static ParameterInfo sParamInfo;
    
    // Processor
    
    spectral_processor<double, FrameLib_DSP::Allocator> mProcessor;
};

#endif
