
#ifndef FRAMELIB_FFT_H
#define FRAMELIB_FFT_H

#include "FrameLib_DSP.h"
#include "../../FrameLib_Dependencies/HISSTools_Library/include/spectral_processor.hpp"

class FrameLib_FFT final : public FrameLib_Processor
{
    // Parameter Enums and Info

    enum ParameterList { kMaxLength, kNormalise, kMode };
    enum Modes { kReal, kComplex, kFullSpectrum };

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
    
    Modes mMode;
    bool mNormalise;
    
    // Processor
    
    htl::spectral_processor<double, FrameLib_DSP::Allocator> mProcessor;
};

#endif
