
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

class SpinLock
{
    
public:
    
    SpinLock() : mAtomicLock(false) {}
    ~SpinLock() { acquire(); }
    
    bool attempt() { return compareAndSwap(mAtomicLock, false, true); }
	void acquire() { while(attempt() == false); }
	void release() { compareAndSwap(mAtomicLock, true, false); }
    
private:
	
    // Deleted
    
    SpinLock(const SpinLock&) = delete;
    SpinLock& operator=(const SpinLock&) = delete;
    
    std::atomic<bool> mAtomicLock;
};


// A class for holding a lock using RAII

class SpinLockHolder
{
    
public:
    
    SpinLockHolder(SpinLock *lock) : mLock(lock) { if (mLock) mLock->acquire(); }
    ~SpinLockHolder() { if (mLock) mLock->release(); }
    
    void destroy()
    {
        if (mLock)
            mLock->release();
        mLock = nullptr;
    }
    
private:
    
    // Deleted
    
    SpinLockHolder(const SpinLockHolder&) = delete;
    SpinLockHolder& operator=(const SpinLockHolder&) = delete;
    
    SpinLock *mLock;
};


// Lightweight joinable thread

class Thread
{
    
    typedef void ThreadFunctionType(void *);
    
public:
    
    enum PriorityLevel {kLowPriority, kMediumPriority, kHighPriority, kAudioPriority};

    Thread(PriorityLevel priority, ThreadFunctionType *threadFunction, void *arg)
    : mInternal(nullptr), mPriority(priority), mThreadFunction(threadFunction), mArg(arg), mValid(false)
    {}

    ~Thread();

    void start();
    void join();

private:
    
    // Deleted
    
    Thread(const Thread&) = delete;
    Thread& operator=(const Thread&) = delete;
    
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


// Semaphore (note that you should most likely close() before the destructor is called)

class Semaphore
{

public:
    
    Semaphore(long maxCount);
    ~Semaphore();
    
    void close();
    void signal(long n);
    bool wait();
    
private:
    
    // Deleted
    
    Semaphore(const Semaphore&) = delete;
    Semaphore& operator=(const Semaphore&) = delete;
    
    // Data
    
    OS_Specific::OSSemaphoreType mInternal;
    bool mValid;
};


// A thread that can be triggered from another thread but without any built-in mechanism to check progress

class TriggerableThread
{
    
public:

    TriggerableThread(Thread::PriorityLevel priority) : mThread(priority, threadEntry, this), mSemaphore(1) {}
    virtual ~TriggerableThread() {}
    
    // Start and join
    
    void start() { mThread.start(); }
    void join();
    
    // Trigger the thread to do something

    void signal() { mSemaphore.signal(1); };
    
private:
    
    // Deleted
    
    TriggerableThread(const Thread&) = delete;
    TriggerableThread& operator=(const Thread&) = delete;
    
    // threadEntry simply calls threadClassEntry which calls the task handler
    
    static void threadEntry(void *thread);
    void threadClassEntry();
    
    // Override this and provide code for the thread's functionality
    
    virtual void doTask() = 0;
    
    // Data
    
    Thread mThread;
    Semaphore mSemaphore;
};


// A thread to delegate tasks to, which can be then be checked for completion

class DelegateThread
{
    
public:

    DelegateThread(Thread::PriorityLevel priority) : mThread(priority, threadEntry, this), mSemaphore(1), mSignaled(false), mFlag(0) {}
    virtual ~DelegateThread() {}

    // Start and join

    void start() { mThread.start(); }
    void join();
    
    // Signal the thread to do something if it is not busy (returns true if the thread was signalled, false if busy)
    
    bool signal();
    
    // Wait for thread's completion of a task - returns true  first time after a signal / false for subsequent calls/the thread has not been signalled
    
    bool completed();
    
private:
    
    // Deleted
    
    DelegateThread(const DelegateThread&) = delete;
    DelegateThread& operator=(const DelegateThread&) = delete;
    
    // threadEntry simply calls threadClassEntry which calls the task handler
    
    static void threadEntry(void *thread);
    void threadClassEntry();
    
    // Override this and provide code for the thread's functionality

    virtual void doTask() = 0;
    
    // Data

    Thread mThread;
    Semaphore mSemaphore;
    
    bool mSignaled;
    std::atomic<int> mFlag;
};

#endif
