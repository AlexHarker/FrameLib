
#ifndef FRAMELIB_CONTEXT_H
#define FRAMELIB_CONTEXT_H

#include "FrameLib_Global.h"

// The Context Object (used to define non-connectable areas in the host environment)
// This acts as a proxy to the global object with a suitable reference to the context

class FrameLib_Context
{
    // Type definition for concision
    
    typedef FrameLib_Global Global;
    
    // Non-copyable template class for retaining reference counted pointers using RAII
    
    template <class T, T *(Global::*getMethod)(void *), void(Global::*releaseMethod)(void *)> class ManagedPointer
    {
        
    public:
        
        ManagedPointer(const FrameLib_Context &context) : mGlobal(context.mGlobal), mReference(context.mReference)
        {
            mPointer = (mGlobal->*getMethod)(mReference);
        }
        
        ~ManagedPointer()
        {
            release();
        }
        
        // Release
        
        void release()
        {
            if (mGlobal)
                (mGlobal->*releaseMethod)(mReference);
            mPointer = NULL;
            mGlobal = NULL;
            mReference  = NULL;
        }
        
        // Pointer  / Bool Conversion
        
        T *operator->()         { return mPointer; }
        operator bool() const   { return mPointer != NULL; }
        
    private:
        
        // Deleted
        
        ManagedPointer(const ManagedPointer&);
        ManagedPointer& operator=(const ManagedPointer&);
        
        // Member Variables
        
        T *mPointer;
        FrameLib_Global *mGlobal;
        void *mReference;
    };

public:
    
    // Constructor - the reference should be a suitable reference address in the host environment
    
    FrameLib_Context(FrameLib_Global *global, void *reference) : mGlobal(global), mReference(reference) {}
    
    // Construct one of these objects to retain a relevant object
    
    typedef ManagedPointer<FrameLib_LocalAllocator, &Global::getAllocator, &Global::releaseAllocator>               Allocator;
    typedef ManagedPointer<FrameLib_ConnectionQueue, &Global::getConnectionQueue, &Global::releaseConnectionQueue>  ConnectionQueue;
    typedef ManagedPointer<FrameLib_DSPQueue, &Global::getDSPQueue, &Global::releaseDSPQueue>                       DSPQueue;

private:
    
    FrameLib_Global *mGlobal;
    void *mReference;
};


#endif