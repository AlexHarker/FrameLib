
#include "FrameLib_DSPQueue.h"
#include "FrameLib_DSP.h"

void FrameLib_DSPQueue::process(FrameLib_DSP *object)
{
    while (object)
    {
        object->dependenciesReady();
        FrameLib_DSP *newObject = object->mNextInThread;
        object->mNextInThread = NULL;
        object = newObject;
    }
    //mQueueSize--;
}

void FrameLib_DSPQueue::start(FrameLib_DSP *object)
{
    mInQueue++;
    mWorker1.signal();
    mWorker2.signal();
    object->mNextInThread = NULL;
    //mQueueSize++;
    process(object);
    serviceQueue();
}

void FrameLib_DSPQueue::add(FrameLib_DSP *object)
{
    //mQueueSize++;
    object->mNextInThread = NULL;
    OSAtomicFifoEnqueue(&mQueue, &object->mQueueItem, offsetof(QueueItem, mNext));
}


void FrameLib_DSPQueue::serviceQueue()
{
    while (true)
    {
        while (QueueItem *next = (QueueItem *) OSAtomicFifoDequeue(&mQueue, offsetof(QueueItem, mNext)))
            process(next->mThis);
    
        // FIX - quick reliable and non-contentious exit strategies are needed here...
        
        //if (mQueueSize == 0)
        if (--mInQueue == 0)
            return;
        
        // FIX - how long is a good time to yield for in a high performance thread?
        
        // Keep pointless contention down and give way to other threads?
        
        struct timespec a;
        a.tv_sec = 0;
        a.tv_nsec = 1000;
        nanosleep(&a,NULL);
        
        mInQueue++;
    }
}
