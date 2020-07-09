
#ifndef FRAMELIB_VECTORSET_H
#define FRAMELIB_VECTORSET_H

#include "FrameLib_DSP.h"

class FrameLib_VectorSet
{
public:
    
    // Constructor
    
    FrameLib_VectorSet(FrameLib_DSP *owner)
    : mFrames(nullptr)
    , mNumFrames(0)
    , mFrameLength(0)
    , mAllocator(*owner)
    {}
    
    ~FrameLib_VectorSet()
    {
        deallocVector(mFrames);
    }
    
    // Getters
    
    unsigned long getNumFrames() const      { return mNumFrames; }
    unsigned long getFrameLength() const    { return mFrameLength; }
    double *getFrame(unsigned long idx)     { return mFrames + (idx * getFrameLength()); }

    // Reset (resets all frames)
    
    void reset(double initial = 0.0)
    {
        std::fill_n(mFrames, getNumFrames() * getFrameLength(), initial);
    }
    
    // Resize without resetting the memory
    
    void resize(unsigned long numFrames, unsigned long frameLength)
    {
        unsigned long oldSize = mFrameLength * mNumFrames;
        unsigned long totalSize = frameLength * numFrames;
        
        if (oldSize != totalSize)
        {
            deallocVector(mFrames);
            mFrames = allocVector(totalSize);
        }
        
        mNumFrames = mFrames ? numFrames : 0;
        mFrameLength = mFrames ? frameLength : 0;
    }
    
    // Resize and reset the memory
    
    void resize(unsigned long numFrames, unsigned long frameLength, double initial)
    {
        resize(numFrames, frameLength);
        reset(initial);
    }

private:
    
    double *allocVector(unsigned long N)
    {
        return mAllocator.allocate<double>(N);
    }
    
    void deallocVector(double *& ptr)
    {
        mAllocator.deallocate(ptr);
    }

    double *mFrames;
    unsigned long mNumFrames;
    unsigned long mFrameLength;
    
    FrameLib_DSP::Allocator mAllocator;
};

#endif
