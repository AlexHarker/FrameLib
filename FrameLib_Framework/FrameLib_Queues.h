
#ifndef FRAMELIB_QUEUES_H
#define FRAMELIB_QUEUES_H

#include "FrameLib_Threading.h"

#include <atomic>

#include <libkern/OSAtomicQueue.h>

// An item in a single linked list

template <class T>
class FrameLib_Item { protected: T * mNext = nullptr; };

// Basic template

template <class T, typename... Users>
struct FrameLib_SingleLinked {} ;

// Template specialisation inheriting virtual members of FrameLib_Item and adding one friend

template <class T, class User>
struct FrameLib_SingleLinked<T, User> : virtual FrameLib_Item<T>
{
    friend User;
};

// Template specialisation allowing a list of one or more classes access to FrameLib_Item

template <class T, class MainUser, class ... Users>
struct FrameLib_SingleLinked<T, MainUser, Users...>
: FrameLib_SingleLinked<T, MainUser>, FrameLib_SingleLinked<T, Users...> {};

template <class T, class...Users>
class FrameLib_Queue
{
    
public:

    using Node = FrameLib_SingleLinked<T, FrameLib_Queue, Users...>;

    FrameLib_Queue() : mHead(nullptr), mTail(nullptr), mSize(0) {}
    
    // Non-copyable
    
    FrameLib_Queue(const FrameLib_Queue&) = delete;
    FrameLib_Queue& operator=(const FrameLib_Queue&) = delete;
    
    // Push an item to the tail
    
    void push(T* item)
    {
        assert(item && "item is null");
        assert(!queued(item) && "item is already in a queue");
        
        if (!empty())
        {
            mTail->Node::mNext = item;
            mTail = item;
        }
        else
            mHead = mTail = item;
        
        mSize++;
    }
    
    // Pop an item from the head
    
    T *pop()
    {
        if (empty())
            return nullptr;
        
        T *item = mHead;
        mHead = item->Node::mNext;
        if (--mSize == 0)
            mTail = nullptr;
        item->Node::mNext = nullptr;
        
        return item;
    }
    
    // Transfer the contents of another queue to this one (clearing the original)
    
    void transfer(FrameLib_Queue& queue)
    {
        if (empty())
            mHead = queue.mHead;
        else
            mTail->mNext = queue.mHead;
        if (queue.mTail)
            mTail = queue.mTail;
        
        mSize += queue.size();
        queue.clear();
    }
    
    // Get the size
    
    unsigned int size() const { return mSize; }

    // Query if the list is empty
    
    bool empty() const { return !size(); }

protected:

    // Determine if the item is already in a queue
    
    bool queued(T* item) const
    {
        return item->Node::mNext || item == mTail;
    }
    
    // Reset the data structure once the items have been transferred elswhere
    
    void clear()
    {
        mHead = mTail = nullptr;
        mSize = 0;
    }

    T *mHead;
    T *mTail;
    unsigned int mSize;
};

/**
 
 @class FrameLib_MethodQueue
 
 @brief a single-threaded queue for non-recursive queuing of items for processing
 
 An item can only be in one position in a single queue at a time.
 
 */

template <class T>
class FrameLib_MethodQueue : private FrameLib_Queue<T>
{

public:
    
    typedef void (T::*Method)(FrameLib_MethodQueue *);

    using Node = typename FrameLib_Queue<T>::Node;
    
    FrameLib_MethodQueue() : mFirst(nullptr) {}
    
    FrameLib_MethodQueue(T *object, Method method) : mFirst(nullptr)
    {
        add(object);
        start(method);
    }
    
    void add(T *object)
    {
        // Do not add if nullptr or re-add if already in queue
        
        if (!object || FrameLib_Queue<T>::queued(object))
            return;

        FrameLib_Queue<T>::push(object);
    }
    
    void start(Method method)
    {
        assert(!mFirst && "Can't restart queue");
        
        mFirst = FrameLib_Queue<T>::mHead;
        
        while (T *object = FrameLib_Queue<T>::pop())
            (object->*method)(this);
        
        mFirst = nullptr;
    }
    
    T *getFirst() const { return mFirst; }
    
protected:
    
    T *mFirst;
};

// FIX - move to threading header
// A counted pointer

template <class T>
struct FrameLib_CountedPointer
{
    FrameLib_CountedPointer() : FrameLib_CountedPointer(nullptr), mCount(0) {}
    FrameLib_CountedPointer(T *item, uintptr_t count) : mPointer(item), mCount(count) {}
    
    bool operator==(const FrameLib_CountedPointer& a) const
    {
        return a.mPointer == mPointer && a.mCount == mCount;
    }
    
    // This structure must not have padding bits, so we use a pointer sized mCount
    
    T *mPointer;
    uintptr_t mCount;
};

template <class T>
using FrameLib_LockFreePointer = std::atomic<FrameLib_CountedPointer<T>>;

template <class T>
class FrameLib_LockFreeStack
{

public:
    
    using Node = typename FrameLib_Queue<T, FrameLib_LockFreeStack>::Node;
    using Pointer = FrameLib_CountedPointer<T>;

    class Queue : public FrameLib_Queue<T, FrameLib_LockFreeStack>
    {
        friend FrameLib_LockFreeStack;
    };
    
    FrameLib_LockFreeStack() : mHead(Pointer(nullptr, 0)) {}
    
    void enqueue(T *item)
    {
        assert((!item->Node::mNext) && "Object is already in the queue");

        while (true)
        {
            const Pointer head = mHead.load(std::memory_order_relaxed);
            item->Node::mNext = head.mPointer;
            
            // Attempt to swap head
            
            if (compareAndSwap(mHead, head, Pointer{item, head.mCount + 1}))
                return;
        }
    }
    
    void enqueue(Queue& queue)
    {
        T *queueHead = queue.mHead;
        T *queueTail = queue.mTail;
        
        queue.clear();
        
        while (true)
        {
            const Pointer head = mHead.load(std::memory_order_relaxed);
            queueTail->Node::mNext = head.mPointer;
            
            // Attempt to swap head
            
            if (compareAndSwap(mHead, head, Pointer{queueHead, head.mCount + 1}))
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
            
            T *item = head.mPointer;
            
            // Attempt to change the head and complete dequeue
            
            if (compareAndSwap(mHead, head, Pointer(item->Node::mNext, head.mCount + 1)))
            {
                item->Node::mNext = nullptr;
                return item;
            }
        }
        
        return nullptr;
    }
    
private:
    
    FrameLib_LockFreePointer<T> mHead;
};


template <class T>
class FrameLib_AppleLockFreeQueue
{
    
public:
    
    // A node in the queue
    
    using Node = FrameLib_SingleLinked<T, FrameLib_AppleLockFreeQueue>;
    using Queue = FrameLib_Queue<T, FrameLib_AppleLockFreeQueue>;
    
    void enqueue(T *item)
    {
        OSAtomicFifoEnqueue(&mQueue, static_cast<Node *>(item), offsetof(Node, mNext));
    }
    
    void enqueue(Queue& queue)
    {
        while (queue.size())
            enqueue(queue.remove());
    }
    
    T *dequeue()
    {
        Node *node = (Node *) OSAtomicFifoDequeue(&mQueue, offsetof(Node, mNext));
        return node ? static_cast<T *>(node) : nullptr;
    }
    
private:
    
    OSFifoQueueHead mQueue OS_ATOMIC_FIFO_QUEUE_INIT;
};

#endif /* FRAMELIB_LOCKFREE_H */
