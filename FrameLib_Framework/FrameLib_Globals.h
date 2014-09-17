
#ifndef FRAMELIB_GLOBALS_H
#define FRAMELIB_GLOBALS_H

#include "FrameLib_Memory.h"
#include "FrameLib_DSP.h"
#include "FrameLib_MultiChannel.h"

struct FrameLib_Global
{
    FrameLib_Global_Allocator *mAllocator;
    
    FrameLib_Global()
    {
        mAllocator = new FrameLib_Global_Allocator;
        mCount = 1;
    }
    
    ~FrameLib_Global()
    {
        delete mAllocator;
    }
    
    void increment()
    {
        ++mCount;
    }
    
    FrameLib_Global *decrement()
    {
        if (--mCount < 1)
        {
            delete this;
            return NULL;
        }
        
        return this;
    }
    
private:
    
    long mCount;
};


struct FrameLib_Common
{
    FrameLib_Local_Allocator *mAllocator;
    FrameLib_MultiChannel::ConnectionQueue *mConnectionQueue;
    FrameLib_DSP::DSPQueue *mDSPQueue;
    
    FrameLib_Common(FrameLib_Global_Allocator *globalAllocator)
    {
        mAllocator = new FrameLib_Local_Allocator(globalAllocator);
        mConnectionQueue = new FrameLib_MultiChannel::ConnectionQueue;
        mDSPQueue = new FrameLib_DSP::DSPQueue(mAllocator);
        
        mCount = 1;
    }
    
    ~FrameLib_Common()
    {
        delete mAllocator;
        delete mConnectionQueue;
        delete mDSPQueue;
    }
    
    void increment()
    {
        ++mCount;
    }
    
    FrameLib_Common *decrement()
    {
        if (--mCount < 1)
        {
            delete this;
            return NULL;
        }
        
        return this;
    }
    
private:
    
    long mCount;
};

#endif
