
#include <assert.h>

#include "FrameLib_Threading.h"

#ifdef __APPLE__

// FIX - TODO - Check windows

// Thread Mac OS implementation

Thread::Thread(PriorityLevel priority, ThreadFunctionType *threadFunction, void *arg) : mThreadFunction(threadFunction), mArg(arg), mValid(true)
{
    pthread_attr_t threadAttributes;
    sched_param schedulingParameters;
    
    // Get default attributes and scheduling parameters
    
    pthread_attr_init(&threadAttributes);
    pthread_attr_getschedparam(&threadAttributes, &schedulingParameters);
    
    // Set detach state and policy
    
    pthread_attr_setdetachstate(&threadAttributes, PTHREAD_CREATE_JOINABLE);
    pthread_attr_setschedpolicy(&threadAttributes, (priority == kAudioPriority) ? SCHED_FIFO : SCHED_OTHER);
    
    // Set the priority of the thread before we create it
    
    switch (priority)
    {
        case kAudioPriority:        schedulingParameters.sched_priority = 75;       break;
        case kHighPriority:         schedulingParameters.sched_priority = 52;       break;
        case kMediumPriority:       schedulingParameters.sched_priority = 31;       break;
        case kLowPriority:          schedulingParameters.sched_priority = 15;       break;
    }
    
    // Set the scheduling attributes and creat the thread
    
    pthread_attr_setschedparam(&threadAttributes, &schedulingParameters);
    pthread_create(&mInternal, &threadAttributes, threadStart, this);
}

Thread::~Thread()
{
    assert(!mValid && "Thread not closed before deletion");
}

void Thread::close()
{
    if (mValid)
    {
        mValid = false;
        OSMemoryBarrier();
        
        // Wait for thread to join before we allow the program to continue
        
        pthread_join(mInternal, NULL);
    }
}

void *Thread::threadStart(void *arg)
{
    static_cast<Thread *>(arg)->call();
    
    return NULL;
}


// Semaphore Mac OS implementation

Semaphore::Semaphore(long maxCount) : mValid(true)
{
    semaphore_create(mach_task_self(), &mInternal, SYNC_POLICY_FIFO, 0);
}

Semaphore::~Semaphore()
{
    assert(!mValid && "Semaphore not closed before deletion");
    semaphore_destroy(mach_task_self(), mInternal);
}

void Semaphore::close()
{
    if (mValid)
    {
        mValid = false;
        OSMemoryBarrier();
        semaphore_signal_all(mInternal);
    }
}

void Semaphore::signal(long n)
{
    OSMemoryBarrier();
    for (long i = 0; i < n; i++)
        semaphore_signal(mInternal);
}

bool Semaphore::wait()
{
    if (mValid)
        semaphore_wait(mInternal);
    
    return mValid;
}

#else

// Thread Windows OS implementation

Thread::Thread(PriorityLevel priority, ThreadFunctionType *threadFunction, void *arg) : mThreadFunction(threadFunction), mArg(arg), mValid(true)
{
    // Create thread
    
    mInternal = CreateThread(NULL, 0, threadStart, this, 0, NULL);
    
    // Set priority
    
    switch (priority)
    {
        case kAudioPriority:        SetThreadPriority(mInternal, THREAD_PRIORITY_TIME_CRITICAL);        break;
        case kHighPriority:         SetThreadPriority(mInternal, THREAD_PRIORITY_HIGHEST);              break;
        case kMediumPriority:       SetThreadPriority(mInternal, THREAD_PRIORITY_NORMAL);               break;
        case kLowPriority:          SetThreadPriority(mInternal, THREAD_PRIORITY_LOWEST);               break;
    }
}

Thread::~Thread()
{
    assert(!mValid && "Thread not closed before deletion");
    CloseHandle(mInternal);
}

void Thread::close()
{
    if (mValid)
    {
        mValid = false;
        MemoryBarrier();
        
        // Wait for thread to join before we allow the program to continue
        
        WaitForSingleObject(mInternal, INFINITE);
    }
}

DWORD WINAPI Thread::threadStart(LPVOID arg)
{
    static_cast<Thread *>(arg)->call();
    
    return 0;
}


// Semaphore Windows OS implementation

Semaphore::Semaphore(long maxCount) : mValid(true)
{
    mInternal = CreateSemaphore(NULL, 0, maxCount, NULL);
}

Semaphore::~Semaphore()
{
    assert(!mValid && "Semaphore not closed before deletion");
    CloseHandle(mInternal);
}

void Semaphore::close()
{
    if (mValid)
    {
        mValid = false;
        MemoryBarrier();
        
        // Signal until the count is zero (only realiable way to signal all waiting threads
        
        for (long releaseCount = 1; releaseCount; --releaseCount)
            ReleaseSemaphore(mInternal, 1, &releaseCount);
    }
}

void Semaphore::signal(long n)
{
    // N.B. - signalling is unsafe after the semaphore has been closed
    
    MemoryBarrier();
    ReleaseSemaphore(mInternal, n, NULL);
}

bool Semaphore::wait()
{
    if (mValid)
        WaitForSingleObject(mInternal, INFINITE);

    return mValid;
}

#endif


// Triggerable Thread

TriggerableThread::~TriggerableThread()
{
    mSemaphore.close();
    mThread.close();
}

void TriggerableThread::threadEntry(void *thread)
{
    static_cast<TriggerableThread *>(thread)->threadClassEntry();
}

void TriggerableThread::threadClassEntry()
{
    while(mSemaphore.wait()) doTask();
}


// Delegate Thread

DelegateThread::~DelegateThread()
{
    mSemaphore.close();
    mThread.close();
}

bool DelegateThread::signal()
{
    if (mSignaled || !mFlag.compareAndSwap(0, 1))
        return false;
    mSignaled = true;
    mSemaphore.signal(1);
    return true;
}

bool DelegateThread::completed()
{
    if (!mSignaled)
        return false;
    while (!mFlag.compareAndSwap(2, 0))
    {
        if (mFlag.compareAndSwap(0, 0))
            return false;
    }
    mSignaled = false;
    return true;
}

void DelegateThread::threadEntry(void *thread)
{
    static_cast<DelegateThread *>(thread)->threadClassEntry();
}

void DelegateThread::threadClassEntry()
{
    while (mSemaphore.wait())
    {
        doTask();
        mFlag.compareAndSwap(1, 2);
    }
}
