
#ifndef FRAMELIB_STREAMID_H
#define FRAMELIB_STREAMID_H

#include "FrameLib_DSP.h"

class FrameLib_StreamID final : public FrameLib_Processor
{
    public:
    
    // Constructor
    
    FrameLib_StreamID(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy);
    
    // Info
    
    std::string objectInfo(bool verbose) override;
    std::string inputInfo(unsigned long idx, bool verbose) override;
    std::string outputInfo(unsigned long idx, bool verbose) override;
    
    // Stream Awareness
    
    void setStream(void *streamOwner, unsigned long stream) override;
    
private:

    // Process

    void process() override;
    
    unsigned long mStream;
};

#endif
