
#ifndef FRAMELIB_CHANNEL_H
#define FRAMELIB_CHANNEL_H

#include "FrameLib_DSP.h"

class FrameLib_Channel : public FrameLib_Processor
{
    public:
    
    // Constructor
    
    FrameLib_Channel(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner);
    
    // Info
    
    std::string objectInfo(bool verbose);
    std::string inputInfo(unsigned long idx, bool verbose);
    std::string outputInfo(unsigned long idx, bool verbose);
    
    // Channel Awareness
    
    void setChannel(unsigned long chan);
    
private:

    // Process

    void process();
    
    unsigned long mChan;
};

#endif
