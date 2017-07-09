
#ifndef FrameLib_THREADING_H
#define FrameLib_THREADING_H

#ifdef __APPLE__

#include <libkern/OSAtomic.h>
#include <pthread.h>
#include <mach/semaphore.h>
#include <mach/task.h>

// Mac OS specific definitions

typedef volatile int32_t Atomic32;

static inline int32_t increment32(Atomic32 *a) { return OSAtomicIncrement32Barrier(a); }
static inline int32_t decrement32(Atomic32 *a) { return OSAtomicDecrement32Barrier(a); }
static inline int32_t add32(Atomic32 *a, int32_t b) { return OSAtomicAdd32Barrier(b, a); }
static inline bool compareAndSwap32(Atomic32 *loc, int32_t comp, int32_t exch) { return OSAtomicCompareAndSwap32Barrier(comp, exch, loc); }

typedef void * volatile AtomicPtr;

static inline bool compareAndSwapPtr(AtomicPtr *loc, void *comp, void *exch) { return OSAtomicCompareAndSwapPtrBarrier(comp, exch, loc); }
static inline void *swapPtr(AtomicPtr *loc, void *swap)
{
    void *ptr = *loc;
    
    while (!compareAndSwapPtr(loc, ptr, swap))
        ptr = *loc;
    
    return ptr;
}

typedef pthread_t OSThreadType;
typedef semaphore_t OSSemaphoreType;
typedef void *OSThreadFunctionType(void *arg);

#else

// Windows OS specific definitions

#include <windows.h>

typedef volatile long Atomic32;

static inline long increment32(Atomic32 *a) { return InterlockedIncrement(a); }
static inline long decrement32(Atomic32 *a) { return InterlockedDecrement(a); }
static inline long add32(Atomic32 *a, long b) { return InterlockedAdd(a, b); }
static inline bool compareAndSwap32(Atomic32 *loc, long comp, long exch) { return InterlockedCompareExchange(loc, exch, comp) == comp; }

typedef volatile PVOID AtomicPtr;

static inline bool compareAndSwapPtr(AtomicPtr *loc, void *comp, void *exch) { return InterlockedCompareExchangePointer(loc, exch, comp) == comp; }
static inline void *swapPtr(AtomicPtr *loc, void *swap) { return InterlockedExchangePointer(loc, swap); }

typedef HANDLE OSThreadType;
typedef HANDLE OSSemaphoreType;
typedef DWORD WINAPI OSThreadFunctionType(LPVOID arg);

#endif

// An atomic 32 bit integer

class FrameLib_Atomic32
{
    
public:

    FrameLib_Atomic32(int32_t value) : mValue(value) {}
    FrameLib_Atomic32() : mValue(0)	{}
	
	bool compareAndSwap(int32_t comparand, int32_t exchange) { return compareAndSwap32(&mValue, comparand, exchange); }
    
    int32_t operator += (const int32_t& a)      { return add32(&mValue, a); }
    
    int32_t operator ++ ()                      { return increment32(&mValue); }
    int32_t operator ++ (int)                   { return operator++() - 1; }
    int32_t operator -- ()                      { return decrement32(&mValue); }
    int32_t operator -- (int)                   { return operator--() + 1; }
    
private:
	
    // Deleted
    
    FrameLib_Atomic32(const FrameLib_Atomic32&);
    FrameLib_Atomic32& operator=(const FrameLib_Atomic32&);
    
	Atomic32 mValue;
};


// An atomic pointer

template <class T> class FrameLib_AtomicPtr
{
    
public:
    
    FrameLib_AtomicPtr()	{ mValue = NULL; }
    
    bool compareAndSwap(T *comparand, T *exchange) { return compareAndSwapPtr(&mValue, comparand, exchange); }
    T *swap(T *exchange) { return (T *) swapPtr(&mValue, exchange); }
    T *clear() { return swap(NULL); }
    
private:
    
    // Deleted
    
    FrameLib_AtomicPtr(const FrameLib_AtomicPtr&);
    FrameLib_AtomicPtr& operator=(const FrameLib_AtomicPtr&);
    
    AtomicPtr mValue;
};


// A spinlock that can be locked, attempted or acquired

class FrameLib_SpinLock
{

public:
    
    FrameLib_SpinLock() {}
    ~FrameLib_SpinLock() { acquire(); }
    
    bool attempt() { return mAtomicLock.compareAndSwap(0, 1); }
	void acquire() { while(attempt() == false); }
	void release() { mAtomicLock.compareAndSwap(1, 0); }
    
private:
	
    // Deleted
    
    FrameLib_SpinLock(const FrameLib_SpinLock&);
    FrameLib_SpinLock& operator=(const FrameLib_SpinLock&);
    
	FrameLib_Atomic32 mAtomicLock;
};


// Lightweight joinable thread

class Thread
{
    
    typedef void ThreadFunctionType(void *);
    
public:
    
    enum PriorityLevel {kLowPriority, kMediumPriority, kHighPriority, kAudioPriority};

    Thread(PriorityLevel priority, ThreadFunctionType *threadFunction, void *arg)
    : mInternal(NULL), mPriority(priority), mThreadFunction(threadFunction), mArg(arg), mValid(false)
    {}

    ~Thread();

    void start();
    void join();

private:
    
    // Deleted
    
    Thread(const Thread&);
    Thread& operator=(const Thread&);
    
    // threadStart is a quick OS-style wrapper to call the object which calls the relevant static function
    
    static OSThreadFunctionType threadStart;
    void call() { mThreadFunction(mArg); }
    
    // Data
    
    OSThreadType mInternal;
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
    
    Semaphore(const Semaphore&);
    Semaphore& operator=(const Semaphore&);
    
    // Data
    
    OSSemaphoreType mInternal;
    bool mValid;
};


// A thread that can be triggered from another thread but without any built-in mechanism to check progress

class TriggerableThread
{
    
public:

    TriggerableThread(Thread::PriorityLevel priority) : mThread(priority, threadEntry, this), mSemaphore(1) {}
    
    // Start and join
    
    void start() { mThread.start(); }
    void join();
    
    // Trigger the thread to do something

    void signal() { mSemaphore.signal(1); };
    
private:
    
    // Deleted
    
    TriggerableThread(const Thread&);
    TriggerableThread& operator=(const Thread&);
    
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

    DelegateThread(Thread::PriorityLevel priority) : mThread(priority, threadEntry, this), mSemaphore(1), mSignaled(false) {}
    
    // Start and join

    void start() { mThread.start(); }
    void join();
    
    // Signal the thread to do something if it is not busy (returns true if the thread was signalled, false if busy)
    
    bool signal();
    
    // Wait for thread's completion of a task - returns true  first time after a signal / false for subsequent calls/the thread has not been signalled
    
    bool completed();
    
private:
    
    // Deleted
    
    DelegateThread(const DelegateThread&);
    DelegateThread& operator=(const DelegateThread&);
    
    // threadEntry simply calls threadClassEntry which calls the task handler
    
    static void threadEntry(void *thread);
    void threadClassEntry();
    
    // Override this and provide code for the thread's functionality

    virtual void doTask() = 0;
    
    // Data

    Thread mThread;
    Semaphore mSemaphore;
    
    bool mSignaled;
    FrameLib_Atomic32 mFlag;
};

#endif
