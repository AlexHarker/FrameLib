
#ifndef FRAMELIB_DSPQUEUE_H
#define FRAMELIB_DSPQUEUE_H

#include "FrameLib_Threading.h"

// Forward Declarations

class FrameLib_DSP;


// DSP Queue - this allows the DSP network to queue processing with minimal additional data storage (avoiding recursive calls)

class FrameLib_DSPQueue
{
    class WorkerThread : public TriggerableThread
    {
        
    public:
        
        WorkerThread(FrameLib_DSPQueue *queue) : TriggerableThread(Thread::kHighPriority), mQueue(queue) {}
        
    private:
        
        virtual void doTask()
        {
            mQueue->mInQueue++;
            mQueue->serviceQueue();
        }
    
        FrameLib_DSPQueue *mQueue;
    };
    
public:
    
    FrameLib_DSPQueue() : mWorker1(this), mWorker2(this)
    {
        memset((void *) &mQueue, 0, sizeof(OSFifoQueueHead));
        mWorker1.start();
        mWorker2.start();
    }

    ~FrameLib_DSPQueue()
    {
        mWorker1.join();
        mWorker2.join();
    }

    void process(FrameLib_DSP *object);
    void start(FrameLib_DSP *object);
    void add(FrameLib_DSP *object);
    
private:
    
    // Deleted
    
    FrameLib_DSPQueue(const FrameLib_DSPQueue&);
    FrameLib_DSPQueue& operator=(const FrameLib_DSPQueue&);
    
    void serviceQueue();
    
    WorkerThread mWorker1;
    WorkerThread mWorker2;
    
    FrameLib_Atomic32 mInQueue;
    OSFifoQueueHead mQueue;
};

#endif /* FRAMELIB_DSPQUEUE_H */
