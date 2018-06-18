
#ifndef FrameLib_THREADING_H
#define FrameLib_THREADING_H

#include "FrameLib_Types.h"

#include <atomic>

#ifdef __linux__

// Linux specific definitions

#include <pthread.h>
#include <semaphore.h>

namespace OS_Specific
{
    typedef pthread_t OSThreadType;
    typedef sem_t OSSemaphoreType;
    typedef void *OSThreadFunctionType(void *arg);
}

#elif defined(__APPLE__)

// OSX specific definitions

#include <pthread.h>
#include <mach/mach.h>

namespace OS_Specific
{
    typedef pthread_t OSThreadType;
    typedef semaphore_t OSSemaphoreType;
    typedef void *OSThreadFunctionType(void *arg);
}

#else
    
// Windows OS specific definitions
    
#include <windows.h>

namespace OS_Specific
{
    typedef HANDLE OSThreadType;
    typedef HANDLE OSSemaphoreType;
    typedef DWORD WINAPI OSThreadFunctionType(LPVOID arg);
}

#endif

// Helpers for exchange fixed values with atomic types

template <class T>
bool compareAndSwap(std::atomic<T>& value, T comparand, T exchange)
{
    return value.compare_exchange_strong(comparand, exchange);
}

template <class T>
bool nullSwap(std::atomic<T *>& value, T *exchange)
{
    T *comparand = nullptr;
    return value.compare_exchange_strong(comparand, exchange);
}

// A spinlock that can be locked, attempted or acquired

class FrameLib_SpinLock
{
    
public:
    
    FrameLib_SpinLock() : mAtomicLock(false) {}
    ~FrameLib_SpinLock() { acquire(); }
    
    // Non-copyable
    
    FrameLib_SpinLock(const FrameLib_SpinLock&) = delete;
    FrameLib_SpinLock& operator=(const FrameLib_SpinLock&) = delete;
    
    bool attempt() { return compareAndSwap(mAtomicLock, false, true); }
	void acquire() { while(attempt() == false); }
	void release() { compareAndSwap(mAtomicLock, true, false); }
    
private:
    
    std::atomic<bool> mAtomicLock;
};


// A class for holding a lock using RAII

class FrameLib_SpinLockHolder
{
    
public:
    
    FrameLib_SpinLockHolder(FrameLib_SpinLock *lock) : mLock(lock) { if (mLock) mLock->acquire(); }
    ~FrameLib_SpinLockHolder() { if (mLock) mLock->release(); }
    
    // Non-copyable
    
    FrameLib_SpinLockHolder(const FrameLib_SpinLockHolder&) = delete;
    FrameLib_SpinLockHolder& operator=(const FrameLib_SpinLockHolder&) = delete;
    
    void destroy()
    {
        if (mLock)
            mLock->release();
        mLock = nullptr;
    }
    
private:
    
    FrameLib_SpinLock *mLock;
};


// Lightweight joinable thread

class FrameLib_Thread
{
    
    typedef void ThreadFunctionType(void *);
    
public:
    
    enum PriorityLevel {kLowPriority, kMediumPriority, kHighPriority, kAudioPriority};

    FrameLib_Thread(PriorityLevel priority, ThreadFunctionType *threadFunction, void *arg)
    : mInternal(nullptr), mPriority(priority), mThreadFunction(threadFunction), mArg(arg), mValid(false)
    {}

    ~FrameLib_Thread();

    // Non-copyable
    
    FrameLib_Thread(const FrameLib_Thread&) = delete;
    FrameLib_Thread& operator=(const FrameLib_Thread&) = delete;
    
    void start();
    void join();

private:
    
    // threadStart is a quick OS-style wrapper to call the object which calls the relevant static function
    
    static OS_Specific::OSThreadFunctionType threadStart;
    void call() { mThreadFunction(mArg); }
    
    // Data
    
    OS_Specific::OSThreadType mInternal;
    PriorityLevel mPriority;
    ThreadFunctionType *mThreadFunction;
    void *mArg;
    bool mValid;
};


// FrameLib_Semaphore (note that you should most likely close() before the destructor is called)

class FrameLib_Semaphore
{

public:
    
    FrameLib_Semaphore(long maxCount);
    ~FrameLib_Semaphore();
    
    // Non-copyable

    FrameLib_Semaphore(const FrameLib_Semaphore&) = delete;
    FrameLib_Semaphore& operator=(const FrameLib_Semaphore&) = delete;
    
    void close();
    void signal(long n);
    bool wait();
    
private:
    
    // Data
    
    OS_Specific::OSSemaphoreType mInternal;
    bool mValid;
};


// A thread that can be triggered from another thread but without any built-in mechanism to check progress

class FrameLib_TriggerableThread
{
    
public:

    FrameLib_TriggerableThread(FrameLib_Thread::PriorityLevel priority) : mThread(priority, threadEntry, this), mSemaphore(1) {}
    virtual ~FrameLib_TriggerableThread() {}
    
    // Non-copyable
    
    FrameLib_TriggerableThread(const FrameLib_TriggerableThread&) = delete;
    FrameLib_TriggerableThread& operator=(const FrameLib_TriggerableThread&) = delete;
    
    // Start and join
    
    void start() { mThread.start(); }
    void join();
    
    // Trigger the thread to do something

    void signal() { mSemaphore.signal(1); };
    
private:
    
    // threadEntry simply calls threadClassEntry which calls the task handler
    
    static void threadEntry(void *thread);
    void threadClassEntry();
    
    // Override this and provide code for the thread's functionality
    
    virtual void doTask() = 0;
    
    // Data
    
    FrameLib_Thread mThread;
    FrameLib_Semaphore mSemaphore;
};


// A thread to delegate tasks to, which can be then be checked for completion

class FrameLib_DelegateThread
{
    
public:

    FrameLib_DelegateThread(FrameLib_Thread::PriorityLevel priority) : mThread(priority, threadEntry, this), mSemaphore(1), mSignaled(false), mFlag(0) {}
    virtual ~FrameLib_DelegateThread() {}

    // Non-copyable
    
    FrameLib_DelegateThread(const FrameLib_DelegateThread&) = delete;
    FrameLib_DelegateThread& operator=(const FrameLib_DelegateThread&) = delete;
    
    // Start and join

    void start() { mThread.start(); }
    void join();
    
    // Signal the thread to do something if it is not busy (returns true if the thread was signalled, false if busy)
    
    bool signal();
    
    // Wait for thread's completion of a task - returns true  first time after a signal / false for subsequent calls/the thread has not been signalled
    
    bool completed();
    
private:
    
    // threadEntry simply calls threadClassEntry which calls the task handler
    
    static void threadEntry(void *thread);
    void threadClassEntry();
    
    // Override this and provide code for the thread's functionality

    virtual void doTask() = 0;
    
    // Data

    FrameLib_Thread mThread;
    FrameLib_Semaphore mSemaphore;
    
    bool mSignaled;
    std::atomic<int> mFlag;
};

#endif
