
#ifndef FRAMELIB_IFFT_H
#define FRAMELIB_IFFT_H

#include "FrameLib_DSP.h"
#include "../../FrameLib_Dependencies/SpectralProcessor.hpp"

// FIX - review FFTSetup

class FrameLib_iFFT final : public FrameLib_Processor
{
    // Parameter Enums and Info

    enum ParameterList { kMaxLength, kNormalise, kMode };
    enum Modes { kReal, kComplex, kFullSpectrum };

    struct ParameterInfo : public FrameLib_Parameters::Info { ParameterInfo(); };

public:

    // Constructor

    FrameLib_iFFT(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy);    
    
    // Info
    
    std::string objectInfo(bool verbose) override;
    std::string inputInfo(unsigned long idx, bool verbose) override;
    std::string outputInfo(unsigned long idx, bool verbose) override;

private:

    // Process
    
    void process() override;

    // Param Info
    
    static ParameterInfo sParamInfo;
    
    // Instantiation Params
    
    Modes mMode;
    bool mNormalise;
    
    // Processor
    
    spectral_processor<double, FrameLib_DSP::Allocator> mProcessor;
};

#endif
