

#include "FrameLib_DSPQueue.h"
#include "FrameLib_DSP.h"


// DSP Queue

// FIX - check assumption that an object can only be in the queue once at a time!

void FrameLib_DSPQueue::add(FrameLib_DSP *object)
{
    object->mNext = NULL;
    
    if (!mTop)
    {
        // Queue is empty - add and start processing the queue
        
        mTop = mTail = object;
        
        while (mTop)
        {
            object = mTop;
            object->dependenciesReady();
            mTop = object->mNext;
            object->mNext = NULL;
        }
        
        mTail = NULL;
    }
    else
    {
        // Add to the queue (which is already processing)
        
        mTail->mNext = object;
        mTail = object;
    }
}
