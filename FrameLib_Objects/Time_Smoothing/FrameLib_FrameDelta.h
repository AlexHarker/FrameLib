
#ifndef FRAMELIB_FRAMEDELTA_H
#define FRAMELIB_FRAMEDELTA_H

#include "FrameLib_DSP.h"

class FrameLib_FrameDelta final : public FrameLib_Processor
{
    
public:
    
    // Constructor
    
    FrameLib_FrameDelta(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy);
    
    ~FrameLib_FrameDelta()
    {
        dealloc(mLastFrame);
    }
    
    // Info
    
    std::string objectInfo(bool verbose) override;
    std::string inputInfo(unsigned long idx, bool verbose) override;
    std::string outputInfo(unsigned long idx, bool verbose) override;
    
private:

    // Object Reset
    
    void objectReset() override { mFrameSize = 0; }
    
    // Process
    
    void process() override;
    
    // Data
    
    double *mLastFrame;
    unsigned long mFrameSize;
};

#endif
