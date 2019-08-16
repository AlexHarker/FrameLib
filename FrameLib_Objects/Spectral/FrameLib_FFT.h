
#ifndef FRAMELIB_FFT_H
#define FRAMELIB_FFT_H

#include "FrameLib_DSP.h"
#include "../../FrameLib_Dependencies/HISSTools_FFT/HISSTools_FFT.h"

// FIX - add zero padding (why not do this prior to FFT with the pad object?)

class FrameLib_FFT final : public FrameLib_Processor
{
    // Parameter Enums and Info

    enum Mode { kReal, kComplex, kFullSpectrum };
    enum ParameterList { kMaxLength, kNormalise, kMode };
    
    struct ParameterInfo : public FrameLib_Parameters::Info { ParameterInfo(); };

public:

    // Constructor / Destructor

    FrameLib_FFT(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy);
    ~FrameLib_FFT();
    
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
    Mode mMode;
    bool mNormalise;
    
    static ParameterInfo sParamInfo;
};

#endif
