
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

template <class T, class TopUser>
class FrameLib_NodeBase { protected: T * mNext = nullptr; };

/**
 
 @class FrameLib_Node
 
 @ingroup Queues

 @brief  a node in a linked list with specific access for a set of "user" types
 
 */

// Non specialised template inheriting virtual FrameLib_NodeBase that will be called for a single user

template <class T, class TopUser, typename... Users>
struct FrameLib_Node : virtual FrameLib_NodeBase<T, TopUser>
{
    friend TopUser;
};

// Template specialisation inheriting virtual FrameLib_NodeBase and adding an additional friend

template <class T, class TopUser, class User>
struct FrameLib_Node<T, TopUser, User> : virtual FrameLib_NodeBase<T, TopUser>
{
    friend TopUser;
    friend User;
};

// Template specialisation allowing more than two classes access to FrameLib_NodeBase

template <class T, class TopUser, class User, class ... OtherUsers>
struct FrameLib_Node<T, TopUser, User, OtherUsers...>
: FrameLib_Node<T, TopUser, User>, FrameLib_Node<T, TopUser, OtherUsers...> {};


/**
 
 @class FrameLib_Queue
 
 @ingroup Queues

 @brief a general purpose queue for objects of a given type (which inherits from the inner Node type)
 
 An item can only be in one position in a single queue at a time.
 
 */

template <class T, class TopUser, class...OtherUsers>
class FrameLib_Queue
{
    
public:

    using Node = FrameLib_Node<T, TopUser, FrameLib_Queue, OtherUsers...>;

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
            static_cast<Node *>(mTail)->mNext = item;
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
        mHead = static_cast<Node *>(item)->mNext;
        if (--mSize == 0)
            mTail = nullptr;
        static_cast<Node *>(item)->mNext = nullptr;
        
        return item;
    }
    
    T *peek() const
    {
        return mHead;
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
        return static_cast<Node *>(item)->mNext || item == mTail;
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
class FrameLib_MethodQueue : private FrameLib_Queue<T, FrameLib_MethodQueue<T>>
{

public:
    
    typedef void (T::*Method)(FrameLib_MethodQueue *);

    using Node = typename FrameLib_Queue<T, FrameLib_MethodQueue>::Node;
    using Queue = FrameLib_Queue<T, FrameLib_MethodQueue>;
    
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
        
        if (!object || Queue::queued(object))
            return;

        Queue::push(object);
    }
    
    // Start the queue
    
    void start(Method method)
    {
        assert(!mFirst && "Can't restart queue");
        
        mFirst = Queue::mHead;
        
        while (T *object = Queue::pop())
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
    
    // Push an item to the head

    void push(T *item)
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
    
    // Push a pre-prepared queue to the head
    
    void push(Queue& queue)
    {
        while (true)
        {
            const Pointer head = mHead.load(std::memory_order_relaxed);
            static_cast<Node *>(queue.mTail)->mNext = head.mPointer;
            
            // Attempt to swap head
            
            if (mHead.trySwap(queue.mHead, head))
                return;
        }
        
        queue.clear();
    }
    
    // Pop one item from the head
    
    T *pop()
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
            
            if (mHead.trySwap(static_cast<Node *>(item)->mNext, head))
            {
                static_cast<Node *>(item)->mNext = nullptr;
                return item;
            }
        }
        
        return nullptr;
    }
    
private:
    
    FrameLib_LockFreePointer<T> mHead;
};

#endif /* FRAMELIB_QUEUES_H */
