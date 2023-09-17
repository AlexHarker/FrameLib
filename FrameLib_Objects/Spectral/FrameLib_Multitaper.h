
#ifndef FRAMELIB_MULTITAPER_H
#define FRAMELIB_MULTITAPER_H

#include "FrameLib_DSP.h"
#include "../../FrameLib_Dependencies/HISSTools_Library/include/spectral_processor.hpp"

class FrameLib_Multitaper final : public FrameLib_Processor
{
    // Parameter Enums and Info

    enum ParameterList { kMaxLength, kNumTapers, kNormalise, kOutput, kWeighting };
    enum Outputs { kPower, kAmplitude };
    enum Weightings { kUniform, kLinear, kParabolic };
    
    struct ParameterInfo : public FrameLib_Parameters::Info { ParameterInfo(); };

public:

    // Constructor

    FrameLib_Multitaper(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy);
    
    // Info
    
    std::string objectInfo(bool verbose) override;
    std::string inputInfo(unsigned long idx, bool verbose) override;
    std::string outputInfo(unsigned long idx, bool verbose) override;

private:
    
    // Helpers
    
    void getWrapped(double &rOut, double&iOut, double *real, double *imag, long size, long pos);
    double getWeight(unsigned long k, unsigned long nTapers);
    
    // Process
    
    void process() override;
    
    static ParameterInfo sParamInfo;
    
    // Processor
    
    htl::spectral_processor<double, FrameLib_DSP::Allocator> mProcessor;
};

#endif
