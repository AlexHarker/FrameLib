
#ifndef FRAMELIB_DSPQUEUE_H
#define FRAMELIB_DSPQUEUE_H

// Forward Declarations

class FrameLib_DSP;


// DSP Queue - this allows the DSP network to queue processing with minimal additional data storage (avoiding recursive calls)

class FrameLib_DSPQueue
{
    
public:
    
    FrameLib_DSPQueue() : mTop(NULL), mTail(NULL) {}
    
    void add(FrameLib_DSP *object);
    
private:
    
    FrameLib_DSP *mTop;
    FrameLib_DSP *mTail;
};

#endif /* FRAMELIB_DSPQUEUE_H */
