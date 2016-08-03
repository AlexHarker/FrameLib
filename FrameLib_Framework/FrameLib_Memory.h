
#ifndef FRAMELIB_MEMORY_H
#define FRAMELIB_MEMORY_H

#include "tlsf.h"
#include "FrameLib_Threading.h"
#include <vector>

// FIX - do alignment improvements ?? (be better as part of allocator directly)
// FIX - threadsafety?
// FIX - expand the block and do free heuristics differently/for malloc (always free large blocks for instance)
// FIX - cleanup and checks
// FIX - free storage?

class FrameLib_Global_Allocator
{
    
public:
    
    static size_t const initialSize = 1024 * 1024 * 128;

    FrameLib_Global_Allocator()
    {
        mLock.acquire();
        mMemory = malloc(initialSize);
        mTLSF = tlsf_create_with_pool(mMemory, initialSize);
        mLock.release();
    }
    
    ~FrameLib_Global_Allocator()
    {
        tlsf_destroy(mTLSF);
        free(mMemory);
    }

    void acquireLock()
    {
        mLock.acquire();
    }
    
    void releaseLock()
    {
        mLock.release();
    }
    
    void *allocWithoutLock(size_t size, size_t alignment)
    {
        return tlsf_memalign(mTLSF, alignment, size);
    }
    
    void deallocWithoutLock(void *ptr)
    {
        tlsf_free(mTLSF, ptr);
    }
    
    void *allocWithLock(size_t size, size_t alignment)
    {
        mLock.acquire();
        void *ptr = allocWithoutLock(size, alignment);
        mLock.release();
        
        return ptr;
    }
    
    void deallocWithLock(void *ptr)
    {
        mLock.acquire();
        deallocWithoutLock(ptr);
        mLock.release();
    }
    
private:
    
    FrameLib_SpinLock mLock;
    void *mMemory;
    tlsf_t mTLSF;
};


class FrameLib_Local_Allocator
{
 
public:
    
    // N.B. - alignment must be a power of two
    
    static const size_t alignment = 16;

    struct Storage
    {
        
    public:
        
        Storage(const char *name, FrameLib_Local_Allocator *allocator) : mData(NULL), mSize(0), mMaxSize(0), mCount(1), mAllocator(allocator)
        {
            mName = strdup(name);
        }
        
        ~Storage()
        {
            mAllocator->dealloc(mData);
            free(mName);
        }
        
        double *getData()
        {
            return mData;
        }
        
        unsigned long getSize()
        {
            return mSize;
        }
        
        const char *getName()
        {
            return mName;
        }
        
        void resize(unsigned long size)
        {
            double *data;
            
            if (mMaxSize >= size && (size >= (mMaxSize >> 1)))
            {
                mSize = size;
                return;
            }
            
            data = (double *) mAllocator->alloc(size * sizeof(double));
            
            if (data)
            {
                mAllocator->dealloc(mData);
                mData = data;
                mMaxSize = size;
                mSize = size;
            }
        }
                
        void increment()
        {
            mCount++;
        }
        
        unsigned long decrement()
        {
            return --mCount;
        }

        
    private:
        
        double *mData;
        unsigned long mSize;
        unsigned long mMaxSize;
        unsigned long mCount;
        char *mName;
        
        FrameLib_Local_Allocator *mAllocator;
    };

private:

    static const int numLocalFreeBlocks = 16;
    
    struct FreeBlock
    {
        FreeBlock() : mMemory(NULL), mSize(0), mPrev(NULL), mNext(NULL) {}
        
        void *mMemory;
        size_t mSize;
        
        FreeBlock *mPrev;
        FreeBlock *mNext;
    };
    
public:

    FrameLib_Local_Allocator(FrameLib_Global_Allocator *allocator) : mGlobalAllocator(allocator)
    {
        for (unsigned long i = 0; i < (numLocalFreeBlocks - 1); i++)
        {
            mFreeLists[i + 1].mPrev = mFreeLists + i;
            mFreeLists[i].mNext = mFreeLists + i + 1;
        }
        
        mTop = mFreeLists;
        mTail = mFreeLists + (numLocalFreeBlocks - 1);
        mTop->mPrev = NULL;
        mTail->mNext = NULL;
    }
    
    ~FrameLib_Local_Allocator()
    {
        // FIX (now fixed) N.B. - local must be freed before global (look at memory mangement and pointers in general)
        
        clearLocal();
    }
    
    void *alloc(size_t size)
    {
        // N.B. - all memory should be aligned to alignment
        
        size_t maxSize = size << 1;
        
        for (FreeBlock *block = mTop; block && block->mMemory; block = block->mNext)
            if (block->mSize >= size && block->mSize <= maxSize)
                return removeBlock(block);
        
        // FIX - free memory if there is an error...
        
        return mGlobalAllocator->allocWithLock(size, alignment);
    }

    // FIX - THIS IS SLOW - cache mishit?
    
    size_t blockSize(void* ptr)
    {
        return (*(size_t *) (((unsigned char*) ptr) - sizeof(size_t))) & ~(0x3);
    }
    
    void dealloc(void *ptr)
    {
        if (ptr)
        {
            if (mTail->mMemory)
                mGlobalAllocator->deallocWithLock(mTail->mMemory);
        
            mTail->mPrev->mNext = NULL;
            mTail->mNext = mTop;
            mTail->mMemory = ptr;
            mTail->mSize = blockSize(ptr);
            mTop->mPrev = mTail;
            mTail->mNext = mTop;
            mTop = mTail;
            mTail = mTop->mPrev;
            mTop->mPrev = NULL;
        }
    }
    
    void clearLocal()
    {
        mGlobalAllocator->acquireLock();
        
        for (unsigned long i = 0; i < numLocalFreeBlocks; i++)
        {
            if (mFreeLists[i].mMemory)
            {
                mGlobalAllocator->deallocWithoutLock(mFreeLists[i].mMemory);
                mFreeLists[i].mMemory = NULL;
            }
        }
        
        mGlobalAllocator->releaseLock();
    }
    
    static size_t alignSize(size_t x)
    {
        return (x + (alignment - 1)) & ~(alignment - 1);
    }
    
private:
    
    std::vector<Storage *>::iterator findStorage(const char *name)
    {
        std::vector<Storage *>::iterator it;
        
        for (it = mStorage.begin(); it != mStorage.end(); it++)
        {
            if (!strcmp((*it)->getName(), name))
                return it;
        }
        
        return it;
    }

public:
    
    Storage *registerStorage(const char *name)
    {
        std::vector<Storage *>::iterator it = findStorage(name);
        
        if (it != mStorage.end())
        {
            (*it)->increment();
            return *it;
        }
        
        Storage *createdStorage = new Storage(name, this);
        mStorage.push_back(createdStorage);

        return createdStorage;
    }

    void releaseStorage(const char *name)
    {
        std::vector<Storage *>::iterator it = findStorage(name);
        
        if ((*it)->decrement() <= 0)
            mStorage.erase(it);
    }
    
private:
    
    void *removeBlock(FreeBlock *block)
    {
        void *ptr = block->mMemory;
        block->mMemory = NULL;
        
        if (block == mTail)
            return ptr;
        
        if (block == mTop)
        {
            block->mNext->mPrev = NULL;
            mTop = block->mNext;
        }
        else
        {
            block->mNext->mPrev = block->mPrev;
            block->mPrev->mNext = block->mNext;
        }
        
        mTail->mNext = block;
        block->mPrev = mTail;
        mTail = block;
        
        return ptr;
    }
    
    FrameLib_Global_Allocator *mGlobalAllocator;
    
    FreeBlock mFreeLists[numLocalFreeBlocks];
    FreeBlock *mTop;
    FreeBlock *mTail;
    
    std::vector <Storage *> mStorage;
};

#endif
