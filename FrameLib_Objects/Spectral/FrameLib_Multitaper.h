
#ifndef FRAMELIB_MULTITAPER_H
#define FRAMELIB_MULTITAPER_H

#include "FrameLib_DSP.h"
#include "FrameLib_Spectral_Processor.h"

class FrameLib_Multitaper final : public FrameLib_Processor, private Spectral_Processor
{
    // Parameter Enums and Info

    enum ParameterList { kMaxLength, kNumTapers };
    
    struct ParameterInfo : public FrameLib_Parameters::Info { ParameterInfo(); };

public:

    // Constructor

    FrameLib_Multitaper(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy);
    
    // Info
    
    std::string objectInfo(bool verbose) override;
    std::string inputInfo(unsigned long idx, bool verbose) override;
    std::string outputInfo(unsigned long idx, bool verbose) override;

private:
    
    // Helpers
    
    void getWrapped(double &rOut, double&iOut, double *real, double *imag, long size, long pos);
    
    // Process
    
    void process() override;

    // Maximum FFT Size

    unsigned long mMaxFFTSize;
    
    static ParameterInfo sParamInfo;
};

#endif
