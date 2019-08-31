
#ifndef FRAMELIB_GLOBAL_H
#define FRAMELIB_GLOBAL_H

#include "FrameLib_Types.h"
#include "FrameLib_Errors.h"
#include "FrameLib_Memory.h"
#include "FrameLib_ProcessingQueue.h"
#include "FrameLib_Threading.h"

#include <memory>
#include <vector>

/**

 @class FrameLib_Global
 
 @ingroup Hosting

 @brief a class for containing and managing FrameLib's global resources.
 
 The global object represents a self-contained FremeLib environment with a single memory-management and error reporting system. The host environment needs to maintain at least one global object in order to create FrameLib networks. Global objects are not constructed/deleted directly, but managed via calls to the static get()/release() methods which take a handle and maintain an internal reference count. The global object can be used as a singleton via a globally-accessible handle, or if preferred separate networks may use individual global objects, in which case they share no resources.
 
 The global objects manages memory allocation, error reporting and the resources for scheduling DSP. FrameLib objects do not deal directly with the global object - it is the responsibility of the host to create an manage these objects, and they are identified to FrameLib objects via a FrameLib_Context object.
 
*/

class FrameLib_Global : public FrameLib_ErrorReporter
{
    friend class FrameLib_Context;
    friend class FrameLib_ProcessingQueue;

private:

    template <class T>
    class PointerSet
    {
        /**
         
         @class CountablePointer
         
         @brief a simple reference counted pointer with a reference address.
         
         */
                
        struct CountablePointer
        {
            CountablePointer(T* object, void *reference) : mObject(object), mReference(reference), mCount(1) {}
            
            std::unique_ptr<T> mObject;
            void *mReference;
            long mCount;
        };
        
    public:
        
        PointerSet(FrameLib_Global& global) : mGlobal(global) {}
        
        // Get an object given a reference pointer
        
        T *get(void *reference)
        {
            for (auto it = mPointers.begin(); it != mPointers.end(); it++)
            {
                if (it->mReference == reference)
                {
                    it->mCount++;
                    return it->mObject.get();
                }
            }
            
            T *object = new T(mGlobal);
            mPointers.push_back(CountablePointer(object, reference));
            return object;
        }
        
        // Release a pre-existing object by reference address
        
        void release(void *reference)
        {
            for (auto it = mPointers.begin(); it != mPointers.end(); it++)
            {
                if (it->mReference == reference)
                {
                    if (--it->mCount < 1)
                        mPointers.erase(it);
                    
                    return;
                }
            }
        }
        
    private:
        
        // Internal Pointers
        
        FrameLib_Global& mGlobal;
        std::vector<CountablePointer> mPointers;
    };
    
public:
        
    /** Get a pointer to a FrameLib_Global object
    
     If the handle points to a nullptr, on return it will point to a valid FrameLib_Global object. Otherwise the reference count of the global object will be incremented. If a new global object is created it will use the object pointed to by notifier to report errors to the host.
     
     @param global a handle to a FrameLib_Global object.
     @param priorities a set of values to set threading priorities.
     @param notifier a pointer to a class that extends FrameLib_ErrorReporter::HostNotifier.

     @sa release()
     
     */
    
    static FrameLib_Global *get(FrameLib_Global **global, FrameLib_Thread::Priorities priorities, FrameLib_ErrorReporter::HostNotifier *notifier = nullptr);
    
    /** Release a FrameLib_Global object
     
     If the handle points a valid FrameLib_Global object then its reference count will be decremented. If the count becomes zero the object will be deleted and the contents of the handle replaced with a nullptr.
     
     @param global a handle to a FrameLib_Global object.

     @sa get()
     */
    
    static void release(FrameLib_Global **global);
        
private:
    
    // Constructor / Destructor
    
    FrameLib_Global(FrameLib_Thread::Priorities priorities, FrameLib_ErrorReporter::HostNotifier *notifier)
    : FrameLib_ErrorReporter(notifier), mAllocator(priorities, *this), mContextAllocators(*this), mProcessingQueues(*this), mPriorities(priorities), mCount(0) {}
    ~FrameLib_Global() {};
    
    // Non-copyable
    
    FrameLib_Global(const FrameLib_Global&) = delete;
    FrameLib_Global& operator=(const FrameLib_Global&) = delete;
    
    // Reference Counting / Auto-deletion
    
    void increment();
    FrameLib_Global *decrement();
    
    // Conversion to allocator for initialisation of context and local allocators
    
    operator FrameLib_GlobalAllocator& () { return mAllocator; }
    
    // Get thread priorities
    
    const FrameLib_Thread::Priorities& getPriorities() { return mPriorities; }
    
    // Member Variables
    
    // Global Memory Allocator
    
    FrameLib_GlobalAllocator mAllocator;
    
    // Context-specific Resources
    
    PointerSet<FrameLib_ContextAllocator> mContextAllocators;
    PointerSet<FrameLib_ProcessingQueue> mProcessingQueues;
    
    // Thread Priorities
    
    FrameLib_Thread::Priorities mPriorities;
    
    // Lock and Reference Count
    
    FrameLib_SpinLock mLock;
    long mCount;
};

#endif
