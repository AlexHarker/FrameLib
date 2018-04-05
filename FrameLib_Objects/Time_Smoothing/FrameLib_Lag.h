
#ifndef FRAMELIB_LAG_H
#define FRAMELIB_LAG_H

#include "FrameLib_DSP.h"
#include "FrameLib_RingBuffer.h"

class FrameLib_Lag : public FrameLib_Processor, private FrameLib_RingBuffer
{
    // Parameter Enums and Info

    enum ParameterList { kMaxFrames, kNumFrames };

    struct ParameterInfo : public FrameLib_Parameters::Info { ParameterInfo(); };

public:
    
    // Constructor
    
    FrameLib_Lag(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy);
    
    // Info
    
    std::string objectInfo(bool verbose);
    std::string inputInfo(unsigned long idx, bool verbose);
    std::string outputInfo(unsigned long idx, bool verbose);
    
private:

    // Object Reset
    
    void objectReset() { FrameLib_RingBuffer::reset(); }
    
    // Process
    
    void process();
    
    // Data
    
    static ParameterInfo sParamInfo;
};

#endif
