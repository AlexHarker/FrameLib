
#ifndef FRAMELIB_QUEUES_H
#define FRAMELIB_QUEUES_H

#include "FrameLib_Threading.h"


/**
 
 @defgroup Queues
 
 */


/**
 
 @class FrameLib_NodeBase
 
 @ingroup Queues

 @brief an object to store a pointer the next object in a single linked list
 
 */

template <class T>
class FrameLib_NodeBase { protected: T * mNext = nullptr; };

/**
 
 @class FrameLib_Node
 
 @ingroup Queues

 @brief  a node in a linked list with specific access for a set of "user" types
 
 */

// Non specialised empty template

template <class T, typename... Users>
struct FrameLib_Node {};

// Template specialisation inheriting virtual FrameLib_NodeBase and adding one friend

template <class T, class User>
struct FrameLib_Node<T, User> : virtual FrameLib_NodeBase<T>
{
    friend User;
};

// Template specialisation allowing one or more classes access to FrameLib_NodeBase

template <class T, class MainUser, class ... Users>
struct FrameLib_Node<T, MainUser, Users...>
: FrameLib_Node<T, MainUser>, FrameLib_Node<T, Users...> {};

/**
 
 @class FrameLib_Queue
 
 @ingroup Queues

 @brief a general purpose queue for objects of a given type (which inherit from the inner Node type)
 
 An item can only be in one position in a single queue at a time.
 
 */

template <class T, class...Users>
class FrameLib_Queue
{
    
public:

    using Node = FrameLib_Node<T, FrameLib_Queue, Users...>;

    // Constructor
    
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

    // Determine if the item is already in the queue
    
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
 
 @ingroup Queues

 @brief a single-threaded queue for non-recursive queuing of a specified method
 
 An item can only be in one position in a single queue at a time.
 
 */

template <class T>
class FrameLib_MethodQueue : private FrameLib_Queue<T>
{

public:
    
    typedef void (T::*Method)(FrameLib_MethodQueue *);

    using Node = typename FrameLib_Queue<T>::Node;
    
    // Constructors (default and starting with an object / method)

    FrameLib_MethodQueue() : mFirst(nullptr) {}
    
    FrameLib_MethodQueue(T *object, Method method) : mFirst(nullptr)
    {
        add(object);
        start(method);
    }
    
    // Add an item to the queue
    
    void add(T *object)
    {
        // Do not add if nullptr or re-add if already in queue
        
        if (!object || FrameLib_Queue<T>::queued(object))
            return;

        FrameLib_Queue<T>::push(object);
    }
    
    // Start the queue
    
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

/**
 
 @class FrameLib_LockFreeStack
 
 @ingroup Queues

 @brief a lock-free stack for queuing items in a multithreaded context
 
 An item can only be in one position in a single queue at a time.

 */

template <class T>
class FrameLib_LockFreeStack
{

public:
    
    using Node = typename FrameLib_Queue<T, FrameLib_LockFreeStack>::Node;
    using Pointer = typename FrameLib_LockFreePointer<T>::Pointer;

    // Internal queue type
    
    class Queue : public FrameLib_Queue<T, FrameLib_LockFreeStack>
    {
        friend FrameLib_LockFreeStack;
    };
    
    // Constructor
    
    FrameLib_LockFreeStack() : mHead(Pointer(nullptr, 0)) {}
    
    // Enqueue an item

    void enqueue(T *item)
    {
        assert((!item->Node::mNext) && "Object is already in the queue");

        while (true)
        {
            const Pointer head = mHead.load(std::memory_order_relaxed);
            item->Node::mNext = head.mPointer;
            
            // Attempt to swap head
            
            if (mHead.trySwap(item, head))
                return;
        }
    }
    
    // Enqueue a pre=prepared queue
    
    void enqueue(Queue& queue)
    {
        while (true)
        {
            const Pointer head = mHead.load(std::memory_order_relaxed);
            queue.mTail->Node::mNext = head.mPointer;
            
            // Attempt to swap head
            
            if (mHead.trySwap(queue.mHead, head))
                return;
        }
        
        queue.clear();

    }
    
    // Dequeue one item
    
    T *dequeue()
    {
        while (true)
        {
            // Read head
            
            const Pointer head = mHead.load(std::memory_order_relaxed);
            
            // Empty queue
            
            if (!head.mPointer)
                return nullptr;
            
            // Read the pointer before CAS otherwise another dequeue may free the head
            
            T *item = head.mPointer;
            
            // Attempt to change the head and complete dequeue
            
            if (mHead.trySwap(item->Node::mNext, head))
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


#ifdef __APPLE__

/**
 
 @class FrameLib_AppleLockFreeQueue
 
 @ingroup Queues

 @brief apple-specific lock-free queue for testing purposes only
 
 */

#include <libkern/OSAtomicQueue.h>

template <class T>
class FrameLib_AppleLockFreeQueue
{
    
public:
    
    // A node in the queue
    
    using Node = FrameLib_Node<T, FrameLib_AppleLockFreeQueue>;
    using Queue = FrameLib_Queue<T, FrameLib_AppleLockFreeQueue>;
    
    // Enqueue an item
    
    void enqueue(T *item)
    {
        OSAtomicFifoEnqueue(&mQueue, static_cast<Node *>(item), offsetof(Node, mNext));
    }
    
    // Enqueue a pre-prepared queue

    void enqueue(Queue& queue)
    {
        while (queue.size())
            enqueue(queue.remove());
    }
    
    // Dequeue an item
    
    T *dequeue()
    {
        Node *node = (Node *) OSAtomicFifoDequeue(&mQueue, offsetof(Node, mNext));
        return node ? static_cast<T *>(node) : nullptr;
    }
    
private:
    
    OSFifoQueueHead mQueue OS_ATOMIC_FIFO_QUEUE_INIT;
};

#endif

#endif /* FRAMELIB_QUEUES_H */
