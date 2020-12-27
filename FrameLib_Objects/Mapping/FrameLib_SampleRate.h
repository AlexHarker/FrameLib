
#ifndef FRAMELIB_SAMPLERATE_H
#define FRAMELIB_SAMPLERATE_H

#include "FrameLib_DSP.h"

class FrameLib_SampleRate final : public FrameLib_Processor
{
    // Parameter Enums and Info

    enum ParameterList { kMode,  kNormalisation };
    enum Modes { kRate, kNyquist, kSampsToMS, kSampsToSecs, kMSToSamps, kSecsToSamps, kNormToFreq, kFreqToNorm };
    enum Normalisations { kNormFull, kNormNyquist };
    
    struct ParameterInfo : public FrameLib_Parameters::Info { ParameterInfo(); };

public:
    
    // Constructor

    FrameLib_SampleRate(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy);
    
    // Info
    
    std::string objectInfo(bool verbose) override;
    std::string inputInfo(unsigned long idx, bool verbose) override;
    std::string outputInfo(unsigned long idx, bool verbose) override;
    
private:
    
    // Process
    
    void process() override;
    
    // Data
    
    static ParameterInfo sParamInfo;
};

#endif
