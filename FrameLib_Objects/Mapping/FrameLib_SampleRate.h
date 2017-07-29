
#ifndef FRAMELIB_SAMPLERATE_H
#define FRAMELIB_SAMPLERATE_H

#include "FrameLib_DSP.h"

class FrameLib_SampleRate : public FrameLib_Processor
{
    // Parameter Enums and Info

    enum ParameterList { kMode };
    enum Modes { kValue, kNyquist, kSampsToMS, kSampsToSecs, kMSToSamps, kSecsToSamps, kNormToFreq, kHalfNormToFreq, kFreqToNorm, kFreqToHalfNorm };

public:
    
    // Constructor

    FrameLib_SampleRate(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner);
    
private:
    
    // Process
    
    void process();
};

#endif
