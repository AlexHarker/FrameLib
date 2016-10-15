
#ifndef FrameLib_THREADING_H
#define FrameLib_THREADING_H

#ifdef __APPLE__

#include <libkern/OSAtomic.h>

typedef volatile int32_t Atomic32;

static inline Atomic32 increment32(Atomic32 *a) { return OSAtomicIncrement32Barrier(a); }
static inline Atomic32 decrement32(Atomic32 *a) { return OSAtomicDecrement32Barrier(a); }
static inline Atomic32 add32(Atomic32 *a, Atomic32 b) { return OSAtomicAdd32Barrier(b, a); }
static inline bool compareAndSwap32(Atomic32 *loc, Atomic32 comp, Atomic32 exch) { return OSAtomicCompareAndSwap32Barrier(comp, exch, loc); }

typedef void * volatile AtomicPtr;

static inline bool compareAndSwapPtr(AtomicPtr *loc, void *comp, void *exch) { return OSAtomicCompareAndSwapPtrBarrier(comp, exch, loc); }
static inline void *swapPtr(AtomicPtr *loc, void *swap)
{
    void *ptr = *loc;
    
    while (!compareAndSwapPtr(loc, ptr, swap))
        ptr = *loc;
    
    return ptr;
}

#else

#include <windows.h>

typedef volatile long Atomic32;

static inline Atomic32 increment32(Atomic32 *a) { return InterlockedIncrement(a); }
static inline Atomic32 decrement32(Atomic32 *a) { return InterlockedDecrement(a); }
static inline Atomic32 add32(Atomic32 *a, Atomic32 b) { return InterlockedAdd(a, b); }
static inline bool compareAndSwap32(Atomic32 *loc, Atomic32 comp, Atomic32 exch) { return InterlockedCompareExchange(loc, comp, exch) == comp; }

typedef volatile PVOID AtomicPtr;

static inline bool compareAndSwapPtr(AtomicPtr *loc, void *comp, void *exch) { return InterlockedCompareExchangePointer(comp, exch, loc); }
static inline void *swapPtr(AtomicPtr *loc, void *swap) { return InterlockedExchangePointer(loc, swap); }

#endif

// An atomic 32 bit integer

class FrameLib_Atomic32
{
    
public:
	
	FrameLib_Atomic32()	{ mValue = 0; }
	
	bool compareAndSwap(Atomic32 comparand, Atomic32 exchange) { return compareAndSwap32(&mValue, comparand, exchange); }
    
    Atomic32 add(Atomic32 amount) { return add32(&mValue, amount); }
    
	Atomic32 increment() { return increment32(&mValue); }
	Atomic32 decrement() { return decrement32(&mValue); }
    
private:
	
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
    
    AtomicPtr mValue;
};


// A spinlock that can be locked, attempted or acquired

class FrameLib_SpinLock
{

public:
    
    ~FrameLib_SpinLock() { acquire(); }
    
    bool attempt() { return mAtomicLock.compareAndSwap(0, 1); }
	void acquire() { while(attempt() == false); }
	void release() { mAtomicLock.compareAndSwap(1, 0); }
    
private:
	
	FrameLib_Atomic32 mAtomicLock;
};

/////////////////

// Lightweight high priority thread and semaphore (note you should destroy the thread before the semaphore)

#ifdef __APPLE__
#include <pthread.h>
#include <mach/semaphore.h>
#include <mach/task.h>
#else
#include <Windows.h>
#endif

struct Thread
{
    
#ifdef __APPLE__
    typedef void *threadFunc(void *arg);
#else
    DWORD WINAPI threadFunc(LPVOID arg);
#endif
    
    Thread(threadFunc *threadFunction, void *arg);
    ~Thread();
    
private:
    
#ifdef __APPLE__
    pthread_t mPth;
#else
    HANDLE mPth;
    bool mExiting;
#endif
};

struct Semaphore
{
    Semaphore(long size);
    ~Semaphore();
    
    void tick(long n);
    bool wait();
    
private:
    
#ifdef __APPLE__
    task_t mTask;
    semaphore_t mInternal;
#else
    long mSize;
    HANDLE mInternal;
#endif
    
};

struct SignalableThread : private Thread
{
    SignalableThread() : Thread(threadStart, this), mSemaphore(1){}
    virtual ~SignalableThread(){}
    
    void signal() { mSemaphore.tick(1); }
    
protected:
    
    bool wait()   { return mSemaphore.wait(); }
    
private:
    
    static void *threadStart(void *thread)
    {
        ((SignalableThread *) thread)->threadEntry();
        
        return NULL;
    }
    
    void threadEntry()
    {
        while(wait()) doTask();
    }
    
    virtual void doTask() = 0;
    
    Semaphore mSemaphore;
};

struct SyncThread : private SignalableThread
{
    SyncThread() : mSignaled(false) {}
    
    bool signal()
    {
        if (mSignaled || !mFlag.compareAndSwap(0, 1))
            return false;
        mSignaled = true;
        SignalableThread::signal();
        return true;
    }
    
    bool completed()
    {
        if (!mSignaled)
            return false;
        while (!mFlag.compareAndSwap(2, 0));
        mSignaled = false;
        return true;
    }
    
private:
    
    static void *threadStart(void *thread)
    {
        ((SyncThread *) thread)->threadEntry();
        
        return NULL;
    }
    
    void threadEntry()
    {
        while(!wait())
        {
            doTask();
            mFlag.compareAndSwap(1, 2);
        }
    }
    
    virtual void doTask() = 0;
    bool mSignaled;
    FrameLib_Atomic32 mFlag;
};

/////////////

#endif
