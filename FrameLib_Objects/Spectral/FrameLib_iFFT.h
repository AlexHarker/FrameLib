
#ifndef FRAMELIB_IFFT_H
#define FRAMELIB_IFFT_H

#include "FrameLib_DSP.h"
#include "FrameLib_Spectral_Processor.h"

// FIX - review FFTSetup

class FrameLib_iFFT final : public FrameLib_Processor, private Spectral_Processor
{
    // Parameter Enums and Info

    enum Mode { kReal, kComplex, kFullSpectrum };
    enum ParameterList { kMaxLength, kNormalise, kMode };
    
    struct ParameterInfo : public FrameLib_Parameters::Info { ParameterInfo(); };

public:

    // Constructor

    FrameLib_iFFT(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy);    
    
    // Info
    
    std::string objectInfo(bool verbose) override;
    std::string inputInfo(unsigned long idx, bool verbose) override;
    std::string outputInfo(unsigned long idx, bool verbose) override;

private:

    // Process
    
    void process() override;

    // Instantiation Params
    
    Mode mMode;
    bool mNormalise;
    
    static ParameterInfo sParamInfo;
};

#endif
