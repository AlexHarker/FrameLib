
#ifndef FRAMELIB_KERNELSMOOTH_H
#define FRAMELIB_KERNELSMOOTH_H

#include "FrameLib_DSP.h"
#include "../../FrameLib_Dependencies/KernelSmoother.hpp"

class FrameLib_KernelSmooth final : public FrameLib_Processor
{
    using Smoother = kernel_smoother<double, FrameLib_DSP::Allocator>;
    using Edges = Smoother::EdgeMode;
    
    enum ParameterList { kSmooth, kScale, kSymmetric, kEdges, kMaxFFTSize };
    enum Scales { kSamples, kNormalised };
    
    struct ParameterInfo : public FrameLib_Parameters::Info { ParameterInfo(); };
    
public:
    
    // Constructor

    FrameLib_KernelSmooth(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy);
    
    // Info
    
    std::string objectInfo(bool verbose) override;
    std::string inputInfo(unsigned long idx, bool verbose) override;
    std::string outputInfo(unsigned long idx, bool verbose) override;

private:
    
    // Process
    
    void process() override;
    
    // Data
    
    static ParameterInfo sParamInfo;
    
    Smoother mSmoother;
};

#endif
