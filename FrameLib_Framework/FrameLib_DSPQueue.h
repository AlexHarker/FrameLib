
#ifndef FRAMELIB_DSPQUEUE_H
#define FRAMELIB_DSPQUEUE_H

#include "FrameLib_Threading.h"

// Forward Declarations

class FrameLib_DSP;


// DSP Queue - this allows the DSP network to queue processing with minimal additional data storage (avoiding recursive calls)

class FrameLib_DSPQueue
{
    class WorkerThreads : public TriggerableThreadSet
    {
        
    public:
        
        WorkerThreads(FrameLib_DSPQueue *queue) : TriggerableThreadSet(Thread::kHighPriority, 7), mQueue(queue) {}
        
    private:
        
        virtual void doTask()
        {
            mQueue->mInQueue++;
            mQueue->serviceQueue();
            mQueue->mInQueue--;
        }
    
        FrameLib_DSPQueue *mQueue;
    };
    
public:
    
    FrameLib_DSPQueue() : mWorkers(this)
    {
        memset((void *) &mQueue, 0, sizeof(OSFifoQueueHead));
        mWorkers.start();
    }

    ~FrameLib_DSPQueue()
    {
        mWorkers.join();
    }

    void process(FrameLib_DSP *object);
    void start(FrameLib_DSP *object);
    void add(FrameLib_DSP *object);
    
private:
    
    // Deleted
    
    FrameLib_DSPQueue(const FrameLib_DSPQueue&);
    FrameLib_DSPQueue& operator=(const FrameLib_DSPQueue&);
    
    void serviceQueue();
    
    WorkerThreads mWorkers;

    FrameLib_Atomic32 mQueueSize;
    FrameLib_Atomic32 mInQueue;
    OSFifoQueueHead mQueue;
};

#endif /* FRAMELIB_DSPQUEUE_H */
