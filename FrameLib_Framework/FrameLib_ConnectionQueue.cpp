
#include "FrameLib_ConnectionQueue.h"
#include "FrameLib_MultiChannel.h"

void FrameLib_ConnectionQueue::add(Item *object)
{
    // Do not re-add if already in queue
    
    if (object->mNext != NULL)
        return;
    
    if (!mTop)
    {
        // Queue is empty - add and start processing the queue
        
        mTop = mTail = object;
        
        while (mTop)
        {
            object = mTop;
            object->inputUpdate();
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