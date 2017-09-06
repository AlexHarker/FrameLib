
#include "FrameLib_DSPQueue.h"
#include "FrameLib_DSP.h"

void FrameLib_DSPQueue::add(FrameLib_DSP *object)
{
    assert((!object->mNext || mTop == object) && "Object is already in the queue and not at the top");

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
