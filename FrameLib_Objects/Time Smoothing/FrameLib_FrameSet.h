
#ifndef FRAMELIB_FRAMESET_H
#define FRAMELIB_FRAMESET_H

#include "FrameLib_Context.h"

class FrameLib_FrameSet
{

public:
    
    // Constructor
    
    FrameLib_FrameSet(FrameLib_Context context) : mFrames(NULL), mNumFrames(0), mFrameLength(0), mAllocator(context)
    {}
    
    ~FrameLib_FrameSet()
    {
        deallocVector(mFrames);
    }
    
    unsigned long getNumFrames() const      { return mNumFrames; }
    unsigned long getFrameLength() const    { return mFrameLength; }
    double *getFrame(unsigned long idx)     { return mFrames + (idx * getFrameLength()); }

    void reset(double initial = 0.0)
    {
        std::fill_n(mFrames, getNumFrames() * getFrameLength(), initial);
    }
    
    void resize(unsigned long numFrames, unsigned long frameLength, double initial = 0.0)
    {
        unsigned long totalSize = frameLength * numFrames;
        
        deallocVector(mFrames);
        mFrames = allocVector(totalSize);
        
        mNumFrames = mFrames ? numFrames : 0;
        mFrameLength = mFrames ? frameLength : 0;
        
        reset(initial);
    }

private:
    
    double *allocVector(unsigned long N)
    {
        return reinterpret_cast<double *>(mAllocator->alloc(sizeof(double) * N));
    }
    
    void deallocVector(double *& ptr)
    {
        mAllocator->dealloc(ptr);
        ptr = NULL;
    }

    double *mFrames;
    unsigned long mNumFrames;
    unsigned long mFrameLength;
    
    FrameLib_Context::Allocator mAllocator;
};



class FrameLib_RingBuffer : private FrameLib_FrameSet
{
    
public:
    
    // Constructor
    
    FrameLib_RingBuffer(FrameLib_Context context) : FrameLib_FrameSet(context), mCounter(0)
    {
    }
    
public:
    
    unsigned long getNumFrames() const      { return FrameLib_FrameSet::getNumFrames(); }
    unsigned long getFrameLength() const    { return FrameLib_FrameSet::getFrameLength(); }
    
    double *getFrame(unsigned long idx)
    {
        unsigned long countLimit = getNumFrames();
        unsigned long count = mCounter + countLimit - idx;
        
        count = count >= countLimit ? count - countLimit : count;
        
        return FrameLib_FrameSet::getFrame(count);
    }
    
    void reset(double initial = 0.0)
    {
        FrameLib_FrameSet::reset(initial);
        mCounter = 0;
    }
    
    void resize(unsigned long numFrames, unsigned long frameLength, double initial = 0.0)
    {
        FrameLib_FrameSet::resize(numFrames, frameLength, initial);
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
