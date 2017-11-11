

#ifndef FRAMELIB_AUDIOTRIGGER_H
#define FRAMELIB_AUDIOTRIGGER_H

#include "FrameLib_DSP.h"
#include <vector>

class FrameLib_AudioTrigger : public FrameLib_Scheduler
{

public:
    
    // Constructor
    
    FrameLib_AudioTrigger(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner);
    
    // Info
    
    std::string objectInfo(bool verbose);
    std::string outputInfo(unsigned long idx, bool verbose);
    std::string audioInfo(unsigned long idx, bool verbose);
    
private:
    
    // Helpers
    
    size_t bufferSize() const { return mBuffer.size(); }
    
    // Object Reset and Block Process
    
    void objectReset();
    void blockProcess(const double * const *ins, double **outs, unsigned long blockSize);
    
    // Schedule
    
    SchedulerInfo schedule(bool newFrame, bool noAdvance);
    
    // Data
    
    std::vector<bool> mBuffer;
    unsigned long mBlockSize;
};

#endif
