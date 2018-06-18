
#include "FrameLib_ProcessingQueue.h"
#include "FrameLib_DSP.h"

void FrameLib_ProcessingQueue::add(FrameLib_DSP *object)
{
    assert(object->mInputTime != FrameLib_TimeFormat::largest() && "Object has already reached the end of time");
    assert((!object->mNext || mTop == object) && "Object is already in the queue and not at the top");

    if (mTimedOut)
        return;
    
    if (!mTop)
    {
        // Queue is empty - add and start processing the queue
        
        mTop = mTail = object;
        
        // Get time
        
        mClock.start();
        unsigned long count = 0;
        
        while (mTop)
        {
            object = mTop;
            object->dependenciesReady();
            mTop = object->mNext;
            object->mNext = nullptr;
            
            // Every so often check whether we're taking too long
            
            if (++count == sProcessPerTimeCheck)
            {
                if (mClock.elapsed() > sMaxTime)
                {
                    mTimedOut = true;
                    
                    // Clear the list
                    
                    while (mTop)
                    {
                        mErrorReporter.reportError(kErrorDSP, mTop->getProxy(), "FrameLib - DSP time out - FrameLib is disabled in this context until this is resolved");
                        object = mTop;
                        mTop = object->mNext;
                        object->mNext = nullptr;
                    }
                }
                count = 0;
            }
        }
        
        mTail = nullptr;
    }
    else
    {
        // Add to the queue (which is already processing)
        
        mTail->mNext = object;
        mTail = object;
    }
}
