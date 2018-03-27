
#ifndef FRAMELIB_FRAMEDELTA_H
#define FRAMELIB_FRAMEDELTA_H

#include "FrameLib_DSP.h"

class FrameLib_FrameDelta : public FrameLib_Processor
{
    
public:
    
    // Constructor
    
    FrameLib_FrameDelta(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner);
    
    ~FrameLib_FrameDelta()
    {
        dealloc(mLastFrame);
    }
    
    // Info
    
    std::string objectInfo(bool verbose);
    std::string inputInfo(unsigned long idx, bool verbose);
    std::string outputInfo(unsigned long idx, bool verbose);
    
private:

    // Object Reset
    
    void objectReset() { mFrameSize = 0; }
    
    // Process
    
    void process();
    
    // Data
    
    double *mLastFrame;
    unsigned long mFrameSize;
};

#endif
