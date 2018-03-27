
#ifndef FRAMELIB_VECTORSET_H
#define FRAMELIB_VECTORSET_H

#include "FrameLib_Context.h"

class FrameLib_VectorSet
{

public:
    
    // Constructor
    
    FrameLib_VectorSet(FrameLib_Context context) : mFrames(NULL), mNumFrames(0), mFrameLength(0), mAllocator(context)
    {}
    
    ~FrameLib_VectorSet()
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

#endif
