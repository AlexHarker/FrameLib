
#include "FrameLib_Global.h"

// The Main Global Class

// Retrieve and release the global object

FrameLib_Global *FrameLib_Global::get(FrameLib_Global **global, FrameLib_ErrorReporter::HostNotifier *notifier)
{
    if (!*global)
        *global = new FrameLib_Global(notifier);

    (*global)->increment();
    return *global;
}

void FrameLib_Global::release(FrameLib_Global **global)
{
    *global = (*global)->decrement();
}

// Reference Counting / Auto-deletion

void FrameLib_Global::increment()
{
    FrameLib_SpinLockHolder lock(&mLock);
    ++mCount;
}

FrameLib_Global *FrameLib_Global::decrement()
{
    FrameLib_SpinLockHolder lock(&mLock);
    
    if (--mCount < 1)
    {
        // N.B. - The spinlock will attempt to lock itself in its deconstructor, so we can't hold the lock
        
        lock.destroy();
        delete this;
        return nullptr;
    }
    
    return this;
}
