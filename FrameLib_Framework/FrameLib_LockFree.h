
#ifndef FRAMELIB_LOCKFREE_H
#define FRAMELIB_LOCKFREE_H

#include <atomic>

#include "FrameLib_Threading.h"

#include <libkern/OSAtomicQueue.h>

template <class T>
class FrameLib_LockFreeStack
{
    
public:
    
    // A node in the stack
    
    struct Node
    {
        Node(T *owner)
        : mOwner(owner), mNext(nullptr) {}
        
        T *mOwner;
        Node *mNext;
    };
    
    // A pre-pared linked list of nodes to be enqued together
    
    class NodeList
    {
        friend class FrameLib_LockFreeStack;
        
    public:
        
        NodeList() : mHead(nullptr), mTail(nullptr), mSize(0) {}
        
        void add(Node* node)
        {
            assert(!node->mNext && "Node is already in a queue");
            
            if (mSize)
            {
                mTail->mNext = node;
                mTail = node;
            }
            else
                mHead = mTail = node;
            
            mSize++;
        }
        
        Node *remove()
        {
            if (!mSize)
                return nullptr;
            
            Node *node = mHead;
            mHead = node->mNext;
            mSize--;
            
            node->mNext = nullptr;
            
            return node;
        }
        
        unsigned int size() const { return mSize; }
        
    private:

        void clear()
        {
            mHead = mTail = nullptr;
            mSize = 0;
        }

        unsigned int mSize;
        Node *mHead;
        Node *mTail;
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
    
    void enqueue(NodeList& nodeList)
    {
        Node *listHead = nodeList.mHead;
        Node *listTail = nodeList.mTail;
        
        nodeList.clear();
        
        while (true)
        {
            const Pointer head = mHead.load(std::memory_order_relaxed);
            listTail->mNext = head.mPointer;
            
            // Attempt to swap head
            
            if (compareAndSwap(mHead, head, Pointer{listHead, head.mCount + 1}))
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
    
private:
    
    std::atomic<Pointer> mHead;
};

// N.B. - the current implementation of a lock-free queue is taken from:
// Michael, Maged M., and Michael L. Scott.
// Simple, Fast, and Practical Non-Blocking and Blocking Concurrent Queue Algorithms.
// No. TR-600. ROCHESTER UNIV NY DEPT OF COMPUTER SCIENCE, 1995.


template <class T>
class FrameLib_LockFreeQueue
{
    
public:
    
    struct Node
    {
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
        
        Node(T *owner)
        : mOwner(owner), mNext(Pointer()) {}
        
        T *mOwner;
        std::atomic<Pointer> mNext;
    };

    using NodePointer = typename Node::Pointer;
    
    FrameLib_LockFreeQueue() : mDummyNode(nullptr), mHead(NodePointer(&mDummyNode, 0)), mTail(NodePointer(&mDummyNode, 0)) {}
    
    void enqueue(Node *node)
    {
        node->mNext = NodePointer();
        
        while (true)
        {
            const NodePointer tail = mTail;
            const NodePointer next = tail.mPointer->mNext;
            
            // Check that tail has not changed
            
            if (tail == mTail)
            {
                if (!next.mPointer)
                {
                    // If tail was pointing to the last node then we can attempt to enqueue
                    
                    if (compareAndSwap(tail.mPointer->mNext, next, NodePointer(node, next.mCount + 1)))
                    {
                        // Try to update tail to the inserted node
                        
                        compareAndSwap(mTail, tail, NodePointer(node, tail.mCount + 1));
                        return;
                    }
                }
                else
                {
                    // Try to advance tail if falling behind
                    
                    compareAndSwap(mTail, tail, NodePointer(next.mPointer, tail.mCount + 1));
                }
            }
        }
    }
    
    T *dequeue()
    {
        while (true)
        {
            // Read head, tail and next
            
            const NodePointer head = mHead.load(std::memory_order_relaxed);
            const NodePointer tail = mTail.load(std::memory_order_relaxed);
            const NodePointer next = head.mPointer->mNext.load(std::memory_order_relaxed);
            
            // Check that head has not changed
            
            if (head == mHead.load(std::memory_order_relaxed))
            {
                // Check if either the queue is empty or tail is falling behind
                
                if (head.mPointer == tail.mPointer)
                {
                    // Check for empty queue or try to advance tail if falling behind
                    
                    if (!next.mPointer)
                        return nullptr;
                    
                    compareAndSwap(mTail, tail, NodePointer(next.mPointer, tail.mCount + 1));
                }
                else
                {
                    // Read the pointer before CAS otherwise another dequeue may free the next node
                    
                    T *object = next.mPointer->mOwner;
                    
                    // Attempt to change the head and complete dequeue
                    
                    if (compareAndSwap(mHead, head, NodePointer(next.mPointer, head.mCount + 1)))
                    {
                        head.mPointer->mNext.store(NodePointer(), std::memory_order_relaxed);
                        return object;
                    }
                }
            }
        }
        
        return nullptr;

    }
    
private:
    
    Node mDummyNode;
    std::atomic<NodePointer> mHead;
    std::atomic<NodePointer> mTail;
};






template <class T>
class FrameLib_AppleLockFreeQueue
{
    
public:
    
    // A node in the queue
    
    struct Node
    {
        Node(T *owner)
        : mOwner(owner), mNext(nullptr) {}
        
        T *mOwner;
        T *mNext;
    };
    
public:
    
    void enqueue(Node *node)
    {
        OSAtomicFifoEnqueue(&mQueue, node, offsetof(Node, mNext));
    }
    
    T *dequeue()
    {
        Node *node = (Node *) OSAtomicFifoDequeue(&mQueue, offsetof(Node, mNext));
        
        return node ? node->mOwner : nullptr;
    }
    
private:
    
    OSFifoQueueHead mQueue OS_ATOMIC_FIFO_QUEUE_INIT;
};

#endif /* FRAMELIB_LOCKFREE_H */
