
#ifndef FRAMELIB_LOCKFREE_H
#define FRAMELIB_LOCKFREE_H

#include <atomic>

#include "FrameLib_Threading.h"

template <class T>
class FrameLib_LockFreeStack
{
    
public:
    
    // A node in the stack
    
    struct Node
    {
        Node(T *owner)
        : mOwner(owner), mNextInThread(nullptr), mNext(nullptr) {}
        
        T *mOwner;
        T *mNextInThread;
        Node *mNext;
    };
    
private:
    
    // A counted pointer
    
    struct Pointer
    {
        Pointer() : mPointer(nullptr), mCount(0) {}
        Pointer(Node *node, uintptr_t count) : mPointer(node), mCount(count) {}
        
        bool operator==(const Pointer& a) const
        {
            return a.mPointer == mPointer && a.mCount == mCount;
        }
        
        // This structure must not have padding bits, so we use a pointer sized mCount
        
        Node *mPointer;
        uintptr_t mCount;
    };
    
public:
    
    FrameLib_LockFreeStack() : mHead(Pointer(nullptr, 0)) {}
    
    void enqueue(Node *node)
    {        
        while (true)
        {
            const Pointer head = mHead.load(std::memory_order_relaxed);
            node->mNext = head.mPointer;
            
            // Attempt to swap head
            
            if (compareAndSwap(mHead, head, Pointer{node, head.mCount + 1}))
                return;
        }
    }
    
    T *dequeue()
    {
        while (true)
        {
            // Read head
            
            const Pointer head = mHead.load(std::memory_order_relaxed);
            
            // Empty queue
            
            if (!head.mPointer)
                return nullptr;
            
            // Read the pointer before CAS otherwise another dequeue may free the next node
            
            Node *node = head.mPointer;
            
            // Attempt to change the head and complete dequeue
            
            if (compareAndSwap(mHead, head, Pointer(node->mNext, head.mCount + 1)))
            {
                node->mNext = nullptr;
                return node->mOwner;
            }
        }
        
        return nullptr;
    }
    
    std::atomic<Pointer> mHead;
};


#endif /* FRAMELIB_LOCKFREE_H */
