

#ifndef FRAMELIB_AUDIOTRIGGER_H
#define FRAMELIB_AUDIOTRIGGER_H

#include "FrameLib_DSP.h"
#include <vector>

class FrameLib_AudioTrigger : public FrameLib_Scheduler
{
    // Parameter Enums and Info

    enum ParameterList { kMaxLength, kLength, kUnits };
    enum Units { kSamples, kMS, kSeconds };
    
    struct ParameterInfo : public FrameLib_Parameters::Info { ParameterInfo(); };
    
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
    void blockProcess(double **ins, double **outs, unsigned long blockSize);
    
    // Schedule
    
    SchedulerInfo schedule(bool newFrame, bool noAdvance);
    
    // Data
    
    std::vector<bool> mBuffer;
    unsigned long mBlockSize;
};

#endif
