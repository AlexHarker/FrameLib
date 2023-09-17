
#ifndef FRAMELIB_CONVOLVE_H
#define FRAMELIB_CONVOLVE_H

#include "FrameLib_DSP.h"
#include "../../FrameLib_Dependencies/HISSTools_Library/include/spectral_processor.hpp"

class FrameLib_Convolve final : public FrameLib_Processor
{
    using Edges = htl::spectral_processor<double, FrameLib_DSP::Allocator>::edge_mode;
    
    // Parameter Enums and Info

    enum ParameterList { kMaxLength, kMode, kEdges };
    enum Modes { kReal, kComplex };
    
    struct ParameterInfo : public FrameLib_Parameters::Info { ParameterInfo(); };

public:

    // Constructor
    
    FrameLib_Convolve(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy);
    
    // Info
    
    std::string objectInfo(bool verbose) override;
    std::string inputInfo(unsigned long idx, bool verbose) override;
    std::string outputInfo(unsigned long idx, bool verbose) override;

private:

    // Process
    
    void process() override;

private:
    
    // Data
    
    Modes mMode;

    static ParameterInfo sParamInfo;
    
    // Processor
    
    htl::spectral_processor<double, FrameLib_DSP::Allocator> mProcessor;
};

#endif
