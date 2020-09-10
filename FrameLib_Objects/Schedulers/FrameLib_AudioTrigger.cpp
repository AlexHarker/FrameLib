
#include "FrameLib_AudioTrigger.h"
#include <algorithm>

// Constructor

FrameLib_AudioTrigger::FrameLib_AudioTrigger(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy)
: FrameLib_Scheduler(context, proxy, nullptr, 0, 1, 1)
{
    objectReset();
    mParameters.set(serialisedParameters);
}

// Info

std::string FrameLib_AudioTrigger::objectInfo(bool verbose)
{
    return formatInfo("Translates non-zero samples into trigger frames: "
                      "The output is an empty frame. "
                      "Any non-zero audio sample triggers a frame.",
                      "Translates non-zero samples into trigger frames.", verbose);
}

std::string FrameLib_AudioTrigger::outputInfo(unsigned long idx, bool verbose)
{
    return "Empty Trigger Frames";
}

std::string FrameLib_AudioTrigger::audioInfo(unsigned long idx, bool verbose)
{
    return formatInfo("Audio Input (non-zero triggers frames)", "Audio Input", verbose);
}

// Object Reset, Block Process and Process

void FrameLib_AudioTrigger::objectReset()
{
    if (mMaxBlockSize != bufferSize())
        mBuffer.resize(mMaxBlockSize);
    
    std::fill_n(mBuffer.begin(), bufferSize(), false);
    
    mBlockSize = 0;
}

void FrameLib_AudioTrigger::blockProcess(const double * const *ins, double **outs, unsigned long blockSize)
{    
    // Safety
    
    if (blockSize > bufferSize())
        return;
    
    for (unsigned long i = 0; i < blockSize; i++)
        mBuffer[i] = ins[0][i];
    
    mBlockSize = blockSize;
}

FrameLib_AudioTrigger::SchedulerInfo FrameLib_AudioTrigger::schedule(bool newFrame, bool noAdvance)
{
    unsigned long offset = static_cast<unsigned long>(getCurrentTime().intVal() - getBlockStartTime().intVal());
    unsigned long i;
    
    // Find next trigger
    
    for (i = offset + 1; i < mBlockSize; i++)
        if (mBuffer[i])
            break;
    
    return SchedulerInfo(i - offset, mBuffer[offset], i < mBlockSize);
}
