
#ifndef FRAMELIB_FFT_H
#define FRAMELIB_FFT_H

#include "FrameLib_DSP.h"
#include "../../FrameLib_Dependencies/SpectralProcessor.hpp"

class FrameLib_FFT final : public FrameLib_Processor
{
    // Parameter Enums and Info

    enum Mode { kReal, kComplex, kFullSpectrum };
    enum ParameterList { kMaxLength, kNormalise, kMode };
    
    struct ParameterInfo : public FrameLib_Parameters::Info { ParameterInfo(); };

public:

    // Constructor

    FrameLib_FFT(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy);
    
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
    
    Mode mMode;
    bool mNormalise;
    
    // Processor
    
    spectral_processor<double, FrameLib_DSP::Allocator> mProcessor;
};

#endif
