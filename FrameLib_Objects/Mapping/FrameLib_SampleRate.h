
#ifndef FRAMELIB_SAMPLERATE_H
#define FRAMELIB_SAMPLERATE_H

#include "FrameLib_DSP.h"

class FrameLib_SampleRate : public FrameLib_Processor
{
    // Parameter Enums and Info

    enum ParameterList { kMode };
    enum Modes { kValue, kNyquist, kSampsToMS, kSampsToSecs, kMSToSamps, kSecsToSamps, kNormToFreq, kHalfNormToFreq, kFreqToNorm, kFreqToHalfNorm };

    struct ParameterInfo : public FrameLib_Parameters::Info { ParameterInfo(); };

public:
    
    // Constructor

    FrameLib_SampleRate(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy);
    
    // Info
    
    std::string objectInfo(bool verbose);
    std::string inputInfo(unsigned long idx, bool verbose);
    std::string outputInfo(unsigned long idx, bool verbose);
    
private:
    
    // Process
    
    void process();
    
    // Data
    
    static ParameterInfo sParamInfo;
};

#endif
