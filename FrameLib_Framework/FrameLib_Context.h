
#ifndef FRAMELIB_CONTEXT_H
#define FRAMELIB_CONTEXT_H

#include "FrameLib_Types.h"
#include "FrameLib_Global.h"
#include "FrameLib_Errors.h"

/**
 
 \class FrameLib_Context
 
 \ingroup Hosting

 \brief a class used to represent distinct non-connectable areas in the host environment.
 
 The context acts as a proxy to FrameLib_Global, and contains a suitable pointer reference to the context in the host environment. Resources for each context are held in the global object, and the context is passed as a parameter when creating any FrameLib object.
 
 */

class FrameLib_Context
{    
    using Global = FrameLib_Global;
    
    // Comparisions
    
    friend bool operator == (const FrameLib_Context& a, const FrameLib_Context& b)
    {
        return a.mGlobal == b.mGlobal && a.mReference == b.mReference;
    }
    
    friend bool operator != (const FrameLib_Context& a, const FrameLib_Context& b)
    {
        return !(a == b);
    }
    
    // Non-copyable template class for retaining reference counted pointers using RAII
    
    template <class T, T *(Global::*getMethod)(void *), void(Global::*releaseMethod)(void *)>
    class ManagedPointer
    {
        
    public:
        
        // Constructor / Destructor
        
        ManagedPointer(const FrameLib_Context &context) : mGlobal(context.mGlobal), mReference(context.mReference)
        {
            mPointer = (mGlobal->*getMethod)(mReference);
        }
        
        ~ManagedPointer()
        {
            release();
        }
        
        // Non-copyable
        
        ManagedPointer(const ManagedPointer&) = delete;
        ManagedPointer& operator=(const ManagedPointer&) = delete;
        
        // Release
        
        void release()
        {
            if (mGlobal)
                (mGlobal->*releaseMethod)(mReference);
            mPointer = nullptr;
            mGlobal = nullptr;
            mReference = nullptr;
        }
        
        // Pointer  / Bool Conversion
        
        T *operator->()         { return mPointer; }
        operator bool() const   { return mPointer != nullptr; }
        
    private:
        
        // Member Variables
        
        T *mPointer;
        FrameLib_Global *mGlobal;
        void *mReference;
    };

public:
    
    // Constructor - the reference should be a suitable reference address in the host environment
    
    FrameLib_Context(FrameLib_Global *global, void *reference) : mGlobal(global), mReference(reference) {}
    
    // Construct one of these objects to retain a relevant object
    
    using Allocator = ManagedPointer<FrameLib_LocalAllocator, &Global::getAllocator, &Global::releaseAllocator>;
    using ProcessingQueue = ManagedPointer<FrameLib_ProcessingQueue, &Global::getProcessingQueue, &Global::releaseProcessingQueue>;

    // Get the global as a FrameLib_ErrorReporter from the context
    
    operator FrameLib_ErrorReporter&() { return *mGlobal; }
    
private:
    
    // Member Variables

    FrameLib_Global *mGlobal;
    void *mReference;
};


#endif
