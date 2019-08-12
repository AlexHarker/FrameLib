
#ifndef FRAMELIB_FFT_H
#define FRAMELIB_FFT_H

#include "FrameLib_DSP.h"
#include "FrameLib_Spectral_Processor.h"

// FIX - add zero padding (why not do this prior to FFT with the pad object?)

class FrameLib_FFT final : public FrameLib_Processor, private Spectral_Processor<double, FrameLib_DSP::Allocator>
{
    // Parameter Enums and Info

    enum Mode { kReal, kComplex, kFullSpectrum };
    enum ParameterList { kMaxLength, kNormalise, kMode };
    
    struct ParameterInfo : public FrameLib_Parameters::Info { ParameterInfo(); };

public:

    // Constructor

    FrameLib_FFT(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy);
    
    // Info
    
    std::string objectInfo(bool verbose) override;
    std::string inputInfo(unsigned long idx, bool verbose) override;
    std::string outputInfo(unsigned long idx, bool verbose) override;

private:

    // Process
    
    void process() override;
    
    // Data
    
    // Instantiation Params
    
    Mode mMode;
    bool mNormalise;
    
    static ParameterInfo sParamInfo;
};

#endif
