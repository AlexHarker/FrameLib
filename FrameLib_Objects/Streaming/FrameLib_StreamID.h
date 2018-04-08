
#ifndef FRAMELIB_STREAMID_H
#define FRAMELIB_STREAMID_H

#include "FrameLib_DSP.h"

class FrameLib_StreamID : public FrameLib_Processor
{
    public:
    
    // Constructor
    
    FrameLib_StreamID(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy);
    
    // Info
    
    std::string objectInfo(bool verbose);
    std::string inputInfo(unsigned long idx, bool verbose);
    std::string outputInfo(unsigned long idx, bool verbose);
    
    // Stream Awareness
    
    void setStream(void *streamOwner, unsigned long stream);
    
private:

    // Process

    void process();
    
    unsigned long mStream;
};

#endif
