
#ifndef FRAMELIB_PROCESSINGQUEUE_H
#define FRAMELIB_PROCESSINGQUEUE_H

#include "FrameLib_Types.h"

// Forward Declarations

class FrameLib_DSP;


// DSP Queue - this allows the DSP network to queue processing with minimal additional data storage (avoiding recursive calls)

class FrameLib_ProcessingQueue
{
    
public:
    
    FrameLib_ProcessingQueue() : mTop(NULL), mTail(NULL) {}
    
    void add(FrameLib_DSP *object);
    
private:
    
    // Deleted
    
    FrameLib_ProcessingQueue(const FrameLib_ProcessingQueue&);
    FrameLib_ProcessingQueue& operator=(const FrameLib_ProcessingQueue&);
    
    FrameLib_DSP *mTop;
    FrameLib_DSP *mTail;
};

#endif /* FRAMELIB_PROCESSINGQUEUE_H */
