
#ifndef FRAMELIB_RINGBUFFER_H
#define FRAMELIB_RINGBUFFER_H

#include "FrameLib_VectorSet.h"

class FrameLib_RingBuffer : private FrameLib_VectorSet
{
    
public:
    
    // Constructor
    
    FrameLib_RingBuffer(FrameLib_Context context) : FrameLib_VectorSet(context), mCounter(0)
    {}
    
public:
    
    unsigned long getNumFrames() const      { return FrameLib_VectorSet::getNumFrames(); }
    unsigned long getFrameLength() const    { return FrameLib_VectorSet::getFrameLength(); }
    
    double *getFrame(unsigned long idx)
    {
        unsigned long countLimit = getNumFrames();
        unsigned long count = mCounter + countLimit - idx;
        
        count = count >= countLimit ? count - countLimit : count;
        
        return FrameLib_VectorSet::getFrame(count);
    }
    
    void reset(double initial = 0.0)
    {
        FrameLib_VectorSet::reset(initial);
        mCounter = 0;
    }
    
    void resize(unsigned long numFrames, unsigned long frameLength, double initial = 0.0)
    {
        FrameLib_VectorSet::resize(numFrames, frameLength, initial);
        mCounter = 0;
    }
    
    void write(const double *frame, unsigned long frameLength)
    {
        if (frameLength == getFrameLength())
        {
            std::copy(frame, frame + frameLength, getFrame(0));
            mCounter = (++mCounter >= getNumFrames()) ? 0 : mCounter;
        }
    }

private:

    unsigned long mCounter;
};


#endif
