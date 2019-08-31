
#ifndef FRAMELIB_CONTEXT_H
#define FRAMELIB_CONTEXT_H

#include "FrameLib_Types.h"
#include "FrameLib_Errors.h"
#include "FrameLib_Global.h"

/**
 
 @class FrameLib_Context
 
 @ingroup Hosting

 @brief a class used to represent distinct non-connectable areas in the host environment.
 
 The context acts as a proxy to FrameLib_Global, and contains a suitable pointer reference to the context in the host environment. Resources for each context are held in the global object, and the context is passed as a parameter when creating any FrameLib object.
 
 */

class FrameLib_Context
{
    /**
     
     @class ManagedPointer
     
     @brief a managed pointer for a context-related resource.

     This is a non-copyable class that uses RAII to update the reference counted pointer in FrameLib_Global
     
     */
    
    template <class T, FrameLib_Global::PointerSet<T> FrameLib_Global::*PointerSet>
    class ManagedPointer
    {
        
    public:
        
        // Constructor / Destructor
        
        ManagedPointer(const FrameLib_Context &context) : mGlobal(context.mGlobal), mReference(context.mReference)
        {
            mPointer = (mGlobal->*PointerSet).get(mReference);
        }
        
        ~ManagedPointer()
        {
            (mGlobal->*PointerSet).release(mReference);
            mPointer = nullptr;
            mGlobal = nullptr;
            mReference = nullptr;
        }
        
        // Non-copyable
        
        ManagedPointer(const ManagedPointer&) = delete;
        ManagedPointer& operator=(const ManagedPointer&) = delete;
    
        // Pointer dereferencing
        
        T *operator->()         { return mPointer; }
        T& operator*()          { return *mPointer; }
        
    private:
        
        // Member Variables
        
        FrameLib_Global *mGlobal;
        void *mReference;
        T *mPointer;
    };

public:
    
    // Constructor - the reference should be a suitable reference address in the host environment
    
    FrameLib_Context(FrameLib_Global *global, void *reference) : mGlobal(global), mReference(reference) {}
    
    // Comparisions
    
    friend bool operator == (const FrameLib_Context& a, const FrameLib_Context& b)
    {
        return a.mGlobal == b.mGlobal && a.mReference == b.mReference;
    }
    
    friend bool operator != (const FrameLib_Context& a, const FrameLib_Context& b)
    {
        return !(a == b);
    }
    
    // Construct one of these objects to retain a relevant object
    
    using Allocator = ManagedPointer<FrameLib_ContextAllocator, &FrameLib_Global::mContextAllocators>;
    using ProcessingQueue = ManagedPointer<FrameLib_ProcessingQueue, &FrameLib_Global::mProcessingQueues>;

    // Get the global as a FrameLib_ErrorReporter from the context
    
    operator FrameLib_ErrorReporter&() { return *mGlobal; }
    
private:
    
    // Member Variables

    FrameLib_Global *mGlobal;
    void *mReference;
};

#endif
