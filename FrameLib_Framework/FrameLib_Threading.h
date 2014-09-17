
#ifndef FrameLib_THREADING_H
#define FrameLib_THREADING_H

#ifdef __APPLE__
#include <libkern/OSAtomic.h>
typedef int32_t Atomic32;
#define OS_INCREMENT_BARRIER_32(a) OSAtomicIncrement32Barrier(a)
#define OS_DECREMENT_BARRIER_32(a) OSAtomicDecrement32Barrier(a)
#define OS_COMPARE_AND_SWAP_BARRIER_32(loc, comp, exch) OSAtomicCompareAndSwap32Barrier(comp, exch, loc)
#else
#include <windows.h>
typedef volatile long Atomic32;
#define OS_INCREMENT_BARRIER_32(a) InterlockedIncrement((a))
#define OS_DECREMENT_BARRIER_32(a) InterlockedDecrement((a))
#define OS_COMPARE_AND_SWAP_BARRIER_32(loc, comp, exch) (InterlockedCompareExchange(loc, comp, exch) == comp)
#endif

class FrameLib_Atomic32
{
public:
	
	FrameLib_Atomic32()
	{
		mValue = 0;
	}
	
	bool atomicSwap(Atomic32 comparand, Atomic32 exchange)
	{
        return OS_COMPARE_AND_SWAP_BARRIER_32(&mValue, comparand, exchange);
	}
	
	Atomic32 atomicIncrementBarrier()
	{
        return OS_INCREMENT_BARRIER_32(&mValue);
	}
	
	Atomic32 atomicDecrementBarrier()
	{
        return OS_DECREMENT_BARRIER_32(&mValue);
	}
    
private:
	
	Atomic32 mValue;
	
};

// ************************************************************************************** //


class FrameLib_SpinLock
{
	
public:
		
	void acquire()
	{
		while(mAtomicLock.atomicSwap(0, 1) == FALSE);
	}
	
	bool attempt()
	{
		return mAtomicLock.atomicSwap(0, 1);
	}
	
	void release()
	{
		mAtomicLock.atomicSwap(1, 0);
	}
    
private:
	
	FrameLib_Atomic32 mAtomicLock;
	
};



#endif
