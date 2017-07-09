
#ifndef FRAMELIB_DSPQUEUE_H
#define FRAMELIB_DSPQUEUE_H

// Forward Declarations

class FrameLib_DSP;


// DSP Queue - this allows the DSP network to queue processing with minimal additional data storage (avoiding recursive calls)

class FrameLib_DSPQueue
{
    
public:
    
    FrameLib_DSPQueue() : mInQueue(false)
    {
        memset((void *) &mQueue, 0, sizeof(OSFifoQueueHead));
    }
    
    void add(FrameLib_DSP *object);
    
private:
    
    // Deleted
    
    FrameLib_DSPQueue(const FrameLib_DSPQueue&);
    FrameLib_DSPQueue& operator=(const FrameLib_DSPQueue&);
    
    bool mInQueue;
    OSFifoQueueHead mQueue;
};

#endif /* FRAMELIB_DSPQUEUE_H */
