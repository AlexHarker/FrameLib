
#include "FrameLib_DSPQueue.h"
#include "FrameLib_DSP.h"

void FrameLib_DSPQueue::add(FrameLib_DSP *object)
{
    if (!mInQueue)
    {
        mInQueue = true;
        object->dependenciesReady();
        while (QueueItem *next = (QueueItem *) OSAtomicFifoDequeue(&mQueue, offsetof(QueueItem, mNext)))
            next->mThis->dependenciesReady();
        mInQueue = false;
    }
    else
        OSAtomicFifoEnqueue(&mQueue, &object->mQueueItem, offsetof(QueueItem, mNext));
}
