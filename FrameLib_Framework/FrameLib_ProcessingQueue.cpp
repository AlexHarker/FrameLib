
#include "FrameLib_ProcessingQueue.h"
#include "FrameLib_DSP.h"

#include <algorithm>

// Processing Queue

void FrameLib_ProcessingQueue::add(FrameLib_DSP *object, FrameLib_DSP *addedBy)
{
    assert(object->mInputTime != FrameLib_TimeFormat::largest() && "Object has already reached the end of time");
    assert((!object->mNextInThread) && "Object is already in the queue");
    
    // Try to process this next in this thread, but if that isn't possible add to the queue
    
    if (!addedBy || addedBy->mNextInThread)
    {
        int32_t numItems = ++mNumItems;
        int32_t numWorkersActive = mNumWorkersActive;
        int32_t numWorkersNeeded = numItems - numWorkersActive;
    
        numWorkersNeeded = std::min(numWorkersNeeded, static_cast<int32_t>(mWorkers.size()) - numWorkersActive);
    
        if (numWorkersNeeded > 0)
            mWorkers.signal(numWorkersNeeded);
    
        OSAtomicFifoEnqueue(&mQueue, &object->mQueueItem, offsetof(QueueItem, mNext));
        
        if (!addedBy)
            serviceQueue();
    }
    else
        addedBy->mNextInThread = object;
}

void FrameLib_ProcessingQueue::serviceQueue()
{
    struct timespec a;
    a.tv_sec = 0;
    a.tv_nsec = 100;
    
    mNumWorkersActive++;
    
    while (true)
    {
        while (QueueItem *next = (QueueItem *) OSAtomicFifoDequeue(&mQueue, offsetof(QueueItem, mNext)))
        {
            FrameLib_DSP *object = next->mThis;
            
            while (object)
            {
                object->dependenciesReady();
                FrameLib_DSP *newObject = object->mNextInThread;
                object->mNextInThread = nullptr;
                object = newObject;
            }
            mNumItems--;
        }
        
        // FIX - quick reliable and non-contentious exit strategies are needed here...
        
        if (mNumItems == 0)
        {
            mNumWorkersActive--;
            return;
        }
        
        // FIX - how long is a good time to yield for in a high performance thread?
        
        // Keep pointless contention down and give way to other threads?
        
        nanosleep(&a, nullptr);
    }
}

// FIX - old version - need to add timeout to the above
/*
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
}*/
