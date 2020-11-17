

#ifndef FRAMELIB_AUDIOTRIGGER_H
#define FRAMELIB_AUDIOTRIGGER_H

#include "FrameLib_DSP.h"
#include <vector>

class FrameLib_AudioTrigger final : public FrameLib_Scheduler
{
public:
    
    // Constructor
    
    FrameLib_AudioTrigger(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy);
    
    // Info
    
    std::string objectInfo(bool verbose) override;
    std::string outputInfo(unsigned long idx, bool verbose) override;
    std::string audioInfo(unsigned long idx, bool verbose) override;
    
private:
    
    // Helpers
    
    size_t bufferSize() const { return mBuffer.size(); }
    
    // Object Reset and Block Process
    
    void objectReset() override;
    void blockProcess(const double * const *ins, double **outs, unsigned long blockSize) override;
    
    // Schedule
    
    SchedulerInfo schedule(bool newFrame, bool noAdvance) override;
    
    // Data
    
    std::vector<bool> mBuffer;
    unsigned long mBlockSize;
};

#endif
