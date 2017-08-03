
#include "FrameLib_AudioTrigger.h"
#include <algorithm>

// Constructor

FrameLib_AudioTrigger::FrameLib_AudioTrigger(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner) : FrameLib_Scheduler(context, NULL, 0, 1, 1)
{
    objectReset();
}

// Info

std::string FrameLib_AudioTrigger::objectInfo(bool verbose)
{
    return getInfo("Translates non-zero samples into trigger frames: The output is an empty vector. Any non-zero audio sample triggers a frame.",
                   "Translates non-zero samples into trigger frames.", verbose);
}

std::string FrameLib_AudioTrigger::outputInfo(unsigned long idx, bool verbose)
{
    return "Empty Trigger Frames";
}

std::string FrameLib_AudioTrigger::audioInfo(unsigned long idx, bool verbose)
{
    return getInfo("Audio Input (non-zero triggers frames)", "Audio Input", verbose);
}

// Object Reset, Block Process and Process

void FrameLib_AudioTrigger::objectReset()
{
    if (mMaxBlockSize != bufferSize())
        mBuffer.resize(mMaxBlockSize);
    
    std::fill_n(mBuffer.begin(), bufferSize(), false);
    
    mBlockSize = 0;
}

void FrameLib_AudioTrigger::blockProcess(double **ins, double **outs, unsigned long blockSize)
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
    unsigned long time = getCurrentTime().intVal();
    unsigned long offset = time - getBlockStartTime().intVal();
    unsigned long i, j;
    
    // Find next trigger
    
    for (i = offset; i < mBlockSize; i++)
        if (mBuffer[i])
            break;
    
    // Is there another trigger in this block?
    
    for (j = (i + 1); j < mBlockSize; j++)
        if (mBuffer[j])
            break;
    
    return SchedulerInfo(i == offset ? j - i : i - offset, i == offset && i < mBlockSize, j < mBlockSize);
}
