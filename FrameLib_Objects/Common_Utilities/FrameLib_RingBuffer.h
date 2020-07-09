
#ifndef FRAMELIB_RINGBUFFER_H
#define FRAMELIB_RINGBUFFER_H

#include "FrameLib_VectorSet.h"

class FrameLib_RingBuffer : private FrameLib_VectorSet
{
public:
    
    // Constructor
    
    FrameLib_RingBuffer(FrameLib_DSP *owner) : FrameLib_VectorSet(owner), mCounter(0), mValid(0)
    {}
    
    // N.B. Valid frames are those written since the last resize or reset
    // This allows for code to avoid needing to clear on resize, thus
    
    // Getters
    
    unsigned long getNumFrames() const      { return FrameLib_VectorSet::getNumFrames(); }
    unsigned long getFrameLength() const    { return FrameLib_VectorSet::getFrameLength(); }
    unsigned long getValidFrames() const    { return mValid; }

    double *getFrame(unsigned long idx)
    {
        unsigned long countLimit = getNumFrames();
        unsigned long count = mCounter + countLimit - idx;
        
        count = count >= countLimit ? count - countLimit : count;
        
        return FrameLib_VectorSet::getFrame(count);
    }
    
    // Reset without resetting the underlying memory
    
    void reset()
    {
        mCounter = 0;
        mValid = 0;
    }
    
    // Reset and reset the underlying memory
    
    void reset(double initial)
    {
        reset();
        FrameLib_VectorSet::reset(initial);
    }
    
    // Resize without resetting the underlying memory

    void resize(unsigned long numFrames, unsigned long frameLength)
    {
        FrameLib_VectorSet::resize(numFrames, frameLength);
        reset();
    }
    
    // Resize and reset the underlying memory

    void resize(unsigned long numFrames, unsigned long frameLength, double initial)
    {
        FrameLib_VectorSet::resize(numFrames, frameLength, initial);
        reset();
    }
    
    // Write frame
    
    void write(const double *frame, unsigned long frameLength)
    {
        if (frameLength == getFrameLength())
        {
            std::copy(frame, frame + frameLength, getFrame(0));
            mCounter = (++mCounter >= getNumFrames()) ? 0 : mCounter;
            mValid = std::min(++mValid, getNumFrames());
        }
    }

private:

    unsigned long mCounter;
    unsigned long mValid;
};


#endif
