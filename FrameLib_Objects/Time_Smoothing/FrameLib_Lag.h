
#ifndef FRAMELIB_LAG_H
#define FRAMELIB_LAG_H

#include "FrameLib_DSP.h"
#include "FrameLib_RingBuffer.h"

class FrameLib_Lag final : public FrameLib_Processor, private FrameLib_RingBuffer
{
    // Parameter Enums and Info

    enum ParameterList { kMaxFrames, kNumFrames };

    struct ParameterInfo : public FrameLib_Parameters::Info { ParameterInfo(); };

public:
    
    // Constructor
    
    FrameLib_Lag(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy);
    
    // Info
    
    std::string objectInfo(bool verbose) override;
    std::string inputInfo(unsigned long idx, bool verbose) override;
    std::string outputInfo(unsigned long idx, bool verbose) override;
    
private:

    // Object Reset
    
    void objectReset() override { FrameLib_RingBuffer::reset(); }
    
    // Process
    
    void process() override;
    
    // Data
    
    static ParameterInfo sParamInfo;
};

#endif
