
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

 \class FrameLib_Global
 
 \ingroup Hosting

 \brief a class for containing and managing FrameLib's global resources.
 
 The global object represents a self-contained FremeLib environment with a single memory-management and error reporting system. The host environment needs to maintain at least one global object in order to create FrameLib networks. Global objects are not constructed/deleted directly, but managed via calls to the static get()/release() methods which take a handle and maintain an internal reference count. The global object can be used as a singleton via a globally-accessible handle, or if preferred separate networks may use individual global objects, in which case they share no resources.
 
 The global objects manages memory allocation, error reporting and the resources for scheduling DSP. FrameLib objects do not deal directly with the global object - it is the responsibility of the host to create an manage these objects, and they are identified to FrameLib objects via a FrameLib_Context object.
 
*/

class FrameLib_Global : public FrameLib_ErrorReporter
{

    friend class FrameLib_Context;
    
private:

    template <class T> class PointerSet
    {
        // A simple countable pointer with a reference address
        
        struct CountablePointer
        {
            CountablePointer(T* object, void *reference) : mObject(object), mReference(reference), mCount(1) {}
            
            std::unique_ptr<T> mObject;
            void *mReference;
            long mCount;
        };
        
    public:
        
        // Add, find or release an object by reference address
        
        void add(T *object, void *reference);
        T *find(void *reference);
        void release(void *reference);
        
    private:
        
        // Internal Pointers
        
        std::vector<CountablePointer> mPointers;
    };
    
public:
        
    /** Get a pointer to a FrameLib_Global object
    
     If the handle points to a nullptr, on return it will point to a valid FrameLib_Global object. Otherwise the reference count of the global object will be incremented. If a new global object is created it will use the object pointed to by notifier to report errors to the host.
     
     \param global a handle to a FrameLib_Global object.
     \param notifier a pointer to a class that extends FrameLib_ErrorReporter::HostNotifier.

     \sa release()
     
     */
    
    static FrameLib_Global *get(FrameLib_Global **global, FrameLib_ErrorReporter::HostNotifier *notifier = nullptr);
    
    /** Release a FrameLib_Global object
     
     If the handle points a valid FrameLib_Global object then its reference count will be decremented. If the count becomes zero the object will be deleted and the contents of the handle replaced with a nullptr.
     
     \param global a handle to a FrameLib_Global object.

     \sa get()
     */
    
    static void release(FrameLib_Global **global);
        
private:
    
    // Constructor / Destructor
    
    FrameLib_Global(FrameLib_ErrorReporter::HostNotifier *notifier) : FrameLib_ErrorReporter(notifier), mAllocator(*this), mCount(0) {}
    ~FrameLib_Global() {};
    
    // Non-copyable
    
    FrameLib_Global(const FrameLib_Global&) = delete;
    FrameLib_Global& operator=(const FrameLib_Global&) = delete;
    
    // Methods to retrieve common objects

    FrameLib_LocalAllocator *getAllocator(void *reference);
    FrameLib_ProcessingQueue *getProcessingQueue(void *reference);
    
    // Methods to release common objects

    void releaseAllocator(void *reference);
    void releaseProcessingQueue(void *reference);
    
    // Reference Counting / Auto-deletion
    
    void increment();
    FrameLib_Global *decrement();
    
    // Member Variables
    
    // Global Memory Allocator
    
    FrameLib_GlobalAllocator mAllocator;
    
    // Context-specific Resources
    
    PointerSet<FrameLib_LocalAllocator> mLocalAllocators;
    PointerSet<FrameLib_ProcessingQueue> mProcessingQueues;
    
    // Lock and Reference Count
    
    FrameLib_SpinLock mLock;
    long mCount;
};

#endif
