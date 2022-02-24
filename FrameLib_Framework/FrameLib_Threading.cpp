
#include "FrameLib_Threading.h"

#include <assert.h>

#ifdef __linux__

// Thread Linux OS implementation

FrameLib_Thread::~FrameLib_Thread()
{
    assert(!mValid && "Thread not joined before deletion");
}

void FrameLib_Thread::start(const Priorities& priorities)
{
    // Valid
    
    mValid = true;
    
    // Create thread
    
    pthread_attr_t threadAttributes;
    sched_param schedulingParameters;
    
    // Get default attributes and scheduling parameters
    
    pthread_attr_init(&threadAttributes);
    pthread_attr_getschedparam(&threadAttributes, &schedulingParameters);
    
    // Set detach state and policy
    
    pthread_attr_setdetachstate(&threadAttributes, PTHREAD_CREATE_JOINABLE);
    pthread_attr_setschedpolicy(&threadAttributes, (mPriority == PriorityLevel::Low) ? SCHED_OTHER : priorities.mRTPolicy);
    
    // Set the priority of the thread before we create it
    
    switch (mPriority)
    {
        case PriorityLevel::Audio:      schedulingParameters.sched_priority = priorities.mAudio;      break;
        case PriorityLevel::High:       schedulingParameters.sched_priority = priorities.mHigh;       break;
        case PriorityLevel::Low:        schedulingParameters.sched_priority = priorities.mLow;        break;
    }
    
    // Set the scheduling attributes and create the thread
    
    pthread_attr_setschedparam(&threadAttributes, &schedulingParameters);
    pthread_create(&mInternal, &threadAttributes, threadStart, this);
}

void FrameLib_Thread::join()
{
    if (mValid)
    {
        mValid = false;
        std::atomic_thread_fence(std::memory_order_seq_cst);
        
        // Wait for thread to join before we allow the program to continue
        
        pthread_join(mInternal, nullptr);
    }
}

void *FrameLib_Thread::threadStart(void *arg)
{
    static_cast<FrameLib_Thread *>(arg)->call();
    
    return nullptr;
}

// Semaphore Linux OS implementation

FrameLib_Semaphore::FrameLib_Semaphore(long maxCount) : mValid(true)
{
    sem_init(&mInternal, 0, 0);
}

FrameLib_Semaphore::~FrameLib_Semaphore()
{
    assert(!mValid && "Semaphore not joined before deletion");
    sem_destroy(&mInternal);
}

void FrameLib_Semaphore::close()
{
    if (mValid)
    {
        mValid = false;
        std::atomic_thread_fence(std::memory_order_seq_cst);
        
        // It appears we get the opposite value to the one we want...
    
        int value = 0;
        
        do {
            sem_post(&mInternal);
            sem_getvalue(&mInternal, &value);
        } while (value < 1);
    }
}

void FrameLib_Semaphore::signal(long n)
{
    std::atomic_thread_fence(std::memory_order_seq_cst);
    for (long i = 0; i < n; i++)
        sem_post(&mInternal);
}

bool FrameLib_Semaphore::wait()
{
    if (mValid)
        sem_wait(&mInternal);
    
    return mValid;
}

#elif defined(__APPLE__)

// Thread Mac OS implementation

FrameLib_Thread::~FrameLib_Thread()
{
    assert(!mValid && "Thread not joined before deletion");
}

void FrameLib_Thread::start(const Priorities& priorities)
{
    // Valid
    
    mValid = true;
    
    // Create thread
    
    pthread_attr_t threadAttributes;
    sched_param schedulingParameters;
    
    // Get default attributes and scheduling parameters
    
    pthread_attr_init(&threadAttributes);
    pthread_attr_getschedparam(&threadAttributes, &schedulingParameters);
    
    // Set detach state and policy
    
    pthread_attr_setdetachstate(&threadAttributes, PTHREAD_CREATE_JOINABLE);
    pthread_attr_setschedpolicy(&threadAttributes, (mPriority == PriorityLevel::Low) ? SCHED_OTHER : priorities.mRTPolicy);
    
    // Set the priority of the thread before we create it
    
    switch (mPriority)
    {
        case PriorityLevel::Audio:      schedulingParameters.sched_priority = priorities.mAudio;      break;
        case PriorityLevel::High:       schedulingParameters.sched_priority = priorities.mHigh;       break;
        case PriorityLevel::Low:        schedulingParameters.sched_priority = priorities.mLow;        break;
    }
    
    // Set the scheduling attributes and create the thread
    
    pthread_attr_setschedparam(&threadAttributes, &schedulingParameters);
    pthread_create(&mInternal, &threadAttributes, threadStart, this);
}

void FrameLib_Thread::join()
{
    if (mValid)
    {
        mValid = false;
        std::atomic_thread_fence(std::memory_order_seq_cst);
        
        // Wait for thread to join before we allow the program to continue
        
        pthread_join(mInternal, nullptr);
    }
}

void *FrameLib_Thread::threadStart(void *arg)
{
    static_cast<FrameLib_Thread *>(arg)->call();
    
    return nullptr;
}

// Semaphore Mac OS implementation

FrameLib_Semaphore::FrameLib_Semaphore(long maxCount) : mValid(true)
{
    semaphore_create(mach_task_self(), &mInternal, SYNC_POLICY_FIFO, 0);
}

FrameLib_Semaphore::~FrameLib_Semaphore()
{
    assert(!mValid && "Semaphore not closed before deletion");
    semaphore_destroy(mach_task_self(), mInternal);
}

void FrameLib_Semaphore::close()
{
    if (mValid)
    {
        mValid = false;
        std::atomic_thread_fence(std::memory_order_seq_cst);
        semaphore_signal_all(mInternal);
    }
}

void FrameLib_Semaphore::signal(long n)
{
    std::atomic_thread_fence(std::memory_order_seq_cst);
    for (long i = 0; i < n; i++)
        semaphore_signal(mInternal);
}

bool FrameLib_Semaphore::wait()
{
    if (mValid)
        semaphore_wait(mInternal);
    
    return mValid;
}

#else

// Thread Windows OS implementation

FrameLib_Thread::~FrameLib_Thread()
{
    assert(!mValid && "Thread not joined before deletion");
    CloseHandle(mInternal);
}

void FrameLib_Thread::start(const Priorities& priorities)
{
    // Valid
    
    mValid = true;
    
    // Create thread
    
    mInternal = CreateThread(nullptr, 0, threadStart, this, 0, nullptr);
    
    // Set priority
    
    switch (mPriority)
    {
        case PriorityLevel::Audio:      SetThreadPriority(mInternal, priorities.mAudio);        break;
        case PriorityLevel::High:       SetThreadPriority(mInternal, priorities.mHigh);         break;
        case PriorityLevel::Low:        SetThreadPriority(mInternal, priorities.mLow);          break;
    }
}

void FrameLib_Thread::join()
{
    if (mValid)
    {
        mValid = false;
        std::atomic_thread_fence(std::memory_order_seq_cst);

        // Wait for thread to join before we allow the program to continue
        
        WaitForSingleObject(mInternal, INFINITE);
    }
}

DWORD WINAPI FrameLib_Thread::threadStart(LPVOID arg)
{
    static_cast<FrameLib_Thread *>(arg)->call();
    
    return 0;
}


// Semaphore Windows OS implementation

FrameLib_Semaphore::FrameLib_Semaphore(long maxCount) : mValid(true)
{
    mInternal.mHandle = CreateSemaphore(nullptr, 0, maxCount, nullptr);
    mInternal.mMaxCount = maxCount;
}

FrameLib_Semaphore::~FrameLib_Semaphore()
{
    assert(!mValid && "Semaphore not closed before deletion");
    CloseHandle(mInternal.mHandle);
}

void FrameLib_Semaphore::close()
{
    if (mValid)
    {
        mValid = false;
        std::atomic_thread_fence(std::memory_order_seq_cst);

        // Signal maximum count to ensure all threads are released, and check for completion
        
        for (long n = mInternal.mMaxCount; n > 0; n--)
        {
            if (ReleaseSemaphore(mInternal.mHandle, n, nullptr))
                break;
        }
    }
}

void FrameLib_Semaphore::signal(long n)
{
    // N.B. - signalling is unsafe after the semaphore has been closed
    
    std::atomic_thread_fence(std::memory_order_seq_cst);
    ReleaseSemaphore(mInternal.mHandle, n, nullptr);
}

bool FrameLib_Semaphore::wait()
{
    if (mValid)
        WaitForSingleObject(mInternal.mHandle, INFINITE);

    return mValid;
}

#endif


// Triggerable Thread

void FrameLib_TriggerableThread::join()
{
    mSemaphore.close();
    mThread.join();
}

void FrameLib_TriggerableThread::threadEntry(void *thread)
{
    static_cast<FrameLib_TriggerableThread *>(thread)->threadClassEntry();
}

void FrameLib_TriggerableThread::threadClassEntry()
{
    while (mSemaphore.wait())
        doTask();
}


// Delegate Thread

void FrameLib_DelegateThread::join()
{
    mSemaphore.close();
    mThread.join();
}

bool FrameLib_DelegateThread::signal()
{
    if (mSignaled || !compareAndSwap(mFlag, 0, 1))
        return false;
    mSignaled = true;
    mSemaphore.signal(1);
    return true;
}

bool FrameLib_DelegateThread::completed()
{
    if (!mSignaled || !compareAndSwap(mFlag, 2, 0))
        return false;
   
    mSignaled = false;
    return true;
}

bool FrameLib_DelegateThread::waitForCompletion()
{
    if (!mSignaled)
        return false;
    while (!compareAndSwap(mFlag, 2, 0))
    {
        if (compareAndSwap(mFlag, 0, 0))
            return false;
    }
    mSignaled = false;
    return true;
}

void FrameLib_DelegateThread::threadEntry(void *thread)
{
    static_cast<FrameLib_DelegateThread *>(thread)->threadClassEntry();
}

void FrameLib_DelegateThread::threadClassEntry()
{
    while (mSemaphore.wait())
    {
        doTask();
        compareAndSwap(mFlag, 1, 2);
    }
}

// Triggerable Thread Set

FrameLib_TriggerableThreadSet::FrameLib_TriggerableThreadSet(FrameLib_Thread::PriorityLevel priority, unsigned int size)
: mSemaphore(size)
{
    mThreads.reserve(size);
    
    for (unsigned int i = 0; i < size; i++)
        mThreads.add(new IndexedThread(priority, this, i));
}

void FrameLib_TriggerableThreadSet::start(const Priorities& priorities)
{
    for (auto it = mThreads.begin(); it != mThreads.end(); it++)
        (*it)->start(priorities);
}

void FrameLib_TriggerableThreadSet::join()
{
    mSemaphore.close();
    for (auto it = mThreads.begin(); it != mThreads.end(); it++)
        (*it)->join();
}

void FrameLib_TriggerableThreadSet::threadEntry(void *thread)
{
    IndexedThread *typedThread = static_cast<IndexedThread *>(thread);
    typedThread->mOwner->threadClassEntry(typedThread->mIndex);
}

void FrameLib_TriggerableThreadSet::threadClassEntry(unsigned int index)
{
    while (mSemaphore.wait())
        doTask(index);
}

