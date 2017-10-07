
#ifndef FRAMELIB_BLOCK_H
#define FRAMELIB_BLOCK_H

#include "FrameLib_Types.h"
#include "FrameLib_Context.h"
#include "FrameLib_Parameters.h"
#include <string>
#include <sstream>
#include <iostream>

template <class T>
class FrameLib_Queueable
{
    
public:
    
    FrameLib_Queueable() : mNext(NULL) {}
    
    class Queue
    {
        typedef void (T::*Method)(Queue *);
        
    public:
        
        Queue() : mMethod(NULL), mTop(NULL), mTail(NULL) {}
        Queue(T *object, Method method) : mMethod(NULL), mTop(NULL), mTail(NULL) { add(object, method); }
        
        void add(T *object, Method method)
        {
            // Do not re-add if already in queue
            
            if (object->FrameLib_Queueable<T>::mNext != NULL)
                return;
            
            if (!mTop)
            {
                // Queue is empty - add and start processing the queue
                
                mMethod = method;
                mFirst = mTop = mTail = object;
                
                while (mTop)
                {
                    object = mTop;
                    (object->*method)(this);
                    mTop = object->FrameLib_Queueable<T>::mNext;
                    object->FrameLib_Queueable<T>::mNext = NULL;
                }
                
                mMethod = NULL;
                mFirst = mTail = NULL;
            }
            else
            {
                assert (method == mMethod && "Cannot add items to open queue for another method");
                
                // Add to the queue (which is already processing)
                
                mTail->FrameLib_Queueable<T>::mNext = object;
                mTail = object;
            }
        }
        
        T *getFirst() const { return mFirst; }
        
    private:
        
        // Deleted
        
        Queue(const Queue&);
        Queue& operator=(const Queue&);
        
        Method mMethod;
        
        T *mFirst;
        T *mTop;
        T *mTail;
    };
    
private:

    T *mNext;
};

// FrameLib_Object

// This abstract template class outlines the basic functionality that objects (blocks / DSP / multichannel must provide)

template <class T>
class FrameLib_Object : public FrameLib_Queueable<T>
{
    
public:

    template <class U>
    struct UntypedConnection
    {
        UntypedConnection() : mObject(NULL), mIndex(0) {}
        UntypedConnection(U *object, unsigned long index) : mObject(object), mIndex(index) {}
        
        U *mObject;
        unsigned long mIndex;
    };
    
private:
    
    typedef UntypedConnection<T> Connection;
    typedef typename std::vector< T *>::iterator ObjectIterator;
    typedef typename std::vector<Connection>::iterator ConnectionIterator;
    typedef typename std::vector<Connection>::const_iterator ConstConnectionIterator;
    
public:

    typedef typename FrameLib_Queueable<T>::Queue Queue;

    // Constructor / Destructor
    
    FrameLib_Object(ObjectType type, FrameLib_Context context, void *owner, T *parent)
    : mType(type), mContext(context), mAllocator(context), mOwner(owner), mParent(parent), mNumIns(0), mNumOuts(0), mNumAudioChans(0), mSupportsOrderingConnections(false), mFeedback(false) {}
    virtual ~FrameLib_Object() {}
   
    // Object Type
    
    ObjectType getType() const                  { return mType; }
    
    // Context
    
    FrameLib_Context getContext() const         { return mContext; }

    // Owner
    
    void *getOwner() const                      { return mOwner; }
    
    // IO Queries
    
    unsigned long getNumIns() const             { return mNumIns; }
    unsigned long getNumOuts() const            { return mNumOuts; }
    unsigned long getNumAudioIns() const        { return getType() != kOutput ? mNumAudioChans : 0; }
    unsigned long getNumAudioOuts() const       { return getType() == kOutput ? mNumAudioChans : 0; }
    unsigned long getNumAudioChans()  const     { return mNumAudioChans; }
    
    // Set Fixed Inputs
    
    virtual void setFixedInput(unsigned long idx, double *input, unsigned long size) = 0;

    // Audio Processing

    // Override to handle audio at the block level (reset called with the audio engine resets)
    
    virtual void blockUpdate(double **ins, double **outs, unsigned long blockSize) = 0;
    virtual void reset(double samplingRate, unsigned long maxBlockSize) = 0;
    
    // Return to host to request to be passed audio
    
    static bool handlesAudio()  { return false; }
    
    // Info

    virtual std::string objectInfo(bool verbose = false)                        { return "No object info available";  }
    virtual std::string inputInfo(unsigned long idx, bool verbose = false)      { return "No input info available";  }
    virtual std::string outputInfo(unsigned long idx, bool verbose = false)     { return "No output info available";  }
    virtual std::string audioInfo(unsigned long idx, bool verbose = false)      { return "No audio channel info available";  }
   
    virtual FrameType inputType(unsigned long idx) const = 0;
    virtual FrameType outputType(unsigned long idx) const = 0;
    
    // N.B. Parameter objects can be queried directly for info
    
    virtual const FrameLib_Parameters *getParameters() const                    { return NULL; }
    
    // Connection 
    
    ConnectionResult addConnection(T *object, unsigned long outIdx, unsigned long inIdx)
    {
        ConnectionResult result = connectionCheck(object);
        Queue queue;
        
        if (result == kConnectSuccess)
        {
            // Store data about connection and reset the dependency count
            
            T *prevObject = mConnections[inIdx].mObject;
            mConnections[inIdx] = Connection(object, outIdx);
            
            // Update dependencies if the connection is now from a different object
            
            if (prevObject != object)
            {
                connectionRemoved(&queue, prevObject);
                object->addOutputDependency(&queue, mParent);
            }
           
            connectionUpdate(&queue);
        }
        
        return result;
    }
    
    void deleteConnection(unsigned long inIdx)
    {
        Queue queue;
        
        clearConnection(&queue, inIdx);
        connectionUpdate(&queue);
    }
    
    ConnectionResult addOrderingConnection(T *object, unsigned long outIdx)
    {
        if (!supportsOrderingConnections())
            return kConnectNoOrderingSupport;
            
        ConnectionResult result = connectionCheck(object);
        Queue queue;

        if (result == kConnectSuccess)
        {
            // If already connected there is nothing to do
        
            for (ConnectionIterator it = mOrderingConnections.begin(); it != mOrderingConnections.end(); it++)
                if (it->mObject == object && it->mIndex == outIdx)
                    return kConnectSuccess;

            // Add the ordering connection
            
            mOrderingConnections.push_back(Connection(object, outIdx));

            // Update dependencies
            
            object->addOutputDependency(&queue, mParent);

            connectionUpdate(&queue);
        }

        return result;
    }
    
    void deleteOrderingConnection(T *object, unsigned long outIdx)
    {
        Queue queue;

        for (ConnectionIterator it = mOrderingConnections.begin(); it != mOrderingConnections.end(); it++)
        {
            if (it->mObject == object && it->mIndex == outIdx)
            {
                deleteOrderingConnection(&queue, it);
                break;
            }
        }
        
        connectionUpdate(&queue);
    }
    
    void clearOrderingConnections()
    {
        Queue queue;

        deleteOrderingConnections(&queue);
        connectionUpdate(&queue);
    }
    
    void clearConnections()
    {
        Queue queue;

        // Clear input connections
        
        for (unsigned long i = 0; i < mConnections.size(); i++)
            clearConnection(&queue, i);
        
        // Delete ordering connections
        
        deleteOrderingConnections(&queue);
        
        // Delete output dependencies
        
        for (ObjectIterator it = mOutputDependencies.begin(); it != mOutputDependencies.end(); )
        {
            (*it)->disconnect(&queue, mParent);
            it = mOutputDependencies.erase(it);
        }
        
        connectionUpdate(&queue);
    }
    
    // Connection Queries

    bool isConnected(unsigned long inIdx) const
    {
        return getConnection(inIdx).mObject != NULL;
    }

    Connection getConnection(unsigned long idx) const                   { return mConnections[idx]; }
    
    bool supportsOrderingConnections() const                            { return mSupportsOrderingConnections; }
    unsigned long getNumOrderingConnections() const                     { return mOrderingConnections.size(); }
    Connection getOrderingConnection(unsigned long idx) const           { return mOrderingConnections[idx]; }
    
    // FIX - try to remove by refactoring DSP to minimise complexity...
    
    bool isOrderingConnection(T *object) const
    {
        for (ConstConnectionIterator it = mOrderingConnections.begin(); it != mOrderingConnections.end(); it++)
            if (it->mObject == object)
                return true;
        
        return false;
    }
    
    unsigned long getNumOutputDependencies() const                      { return mOutputDependencies.size(); }
    T *getOutputDependency(unsigned long idx) const                     { return mOutputDependencies[idx]; }
    
    // Automatic Dependency Connections
    
    virtual void autoOrderingConnections() = 0;
    virtual void clearAutoOrderingConnections() = 0;
    
protected:
    
    // IO Setup
    
    void setIO(unsigned long nIns, unsigned long nOuts, unsigned long nAudioChans = 0)
    {
        mNumIns = (getType() == kScheduler || nIns) ? nIns : 1;
        mNumOuts = nOuts;
        mNumAudioChans = nAudioChans;
        
        mConnections.resize(mNumIns);
    }
    
    // Ordering Setup
    
    void enableOrderingConnections()                         { mSupportsOrderingConnections = true; }

    // Memory Allocation
    
    template <class U> U *alloc(unsigned long N)
    {
        return reinterpret_cast<U *>(mAllocator->alloc(sizeof(U) * N));
    }

    template <class U> void dealloc(U *& ptr)
    {
        mAllocator->dealloc(ptr);
        ptr = NULL;
    }
    
    void clearAllocator() { mAllocator->clear(); }
    
    FrameLib_LocalAllocator::Storage *registerStorage(const char *name)     { return mAllocator->registerStorage(name); }
    
    void releaseStorage(FrameLib_LocalAllocator::Storage *&storage)
    {
        mAllocator->releaseStorage(storage->getName());
        storage = NULL;
    }
    
    // Info Helpers
    
    static const char *formatInfo(const char *verboseStr, const char *briefStr, bool verbose)
    {
        return verbose ? verboseStr : briefStr;
    }
    
    static std::string formatInfo(const char *str, unsigned long idx)
    {
        std::string info = str;
        std::string idxStr = numberedString("", idx + 1);
        
        for (size_t pos = info.find("#", 0); pos != std::string::npos;  pos = info.find("#", pos + 1))
            info.replace(pos, 1, idxStr);
        
        return info;
    }

    static std::string formatInfo(const char *verboseStr, const char *briefStr, unsigned long idx, bool verbose)
    {
        return formatInfo(formatInfo(verboseStr, briefStr, verbose), idx);
    }
    
    static std::string formatInfo(const char *str, const char *replaceStr)
    {
        std::string info = str;
        
        for (size_t pos = info.find("#", 0); pos != std::string::npos;  pos = info.find("#", pos + 1))
            info.replace(pos, 1, replaceStr);
        
        return info;
    }

    static std::string formatInfo(const char *verboseStr, const char *briefStr, const char *replaceStr, bool verbose)
    {
        return formatInfo(formatInfo(verboseStr, briefStr, verbose), replaceStr);
    }
    
    // String With Number Helper
    
    static  std::string numberedString(const char *str, unsigned long idx)
    {
        std::ostringstream outStr;
        
        outStr << str;
        outStr << idx;
        
        return outStr.str();
    }
    
private:
    
    // Connection Methods (private)
    
    virtual void connectionUpdate(Queue *queue) = 0;
    
    // Connection Check
    
    ConnectionResult connectionCheck(T *object)
    {
        if (object == this)
            return kConnectSelfConnection;
    
        if (object->mContext != mContext)
            return kConnectWrongContext;
    
        if (detectFeedback(object))
            return kConnectFeedbackDetected;
        
        return kConnectSuccess;
    }
    
    // Dependency Updating
    
    void addOutputDependency(Queue *queue, T *object)
    {
        for (ObjectIterator it = mOutputDependencies.begin(); it != mOutputDependencies.end(); it++)
            if (*it == object)
                return;
        
        mOutputDependencies.push_back(object);
        connectionUpdate(queue);
    }
    
    void deleteOutputDependency(Queue *queue, T *object)
    {
        for (ObjectIterator it = mOutputDependencies.begin(); it != mOutputDependencies.end(); it++)
        {
            if (*it == object)
            {
                mOutputDependencies.erase(it);
                connectionUpdate(queue);
                return;
            }
        }
    }
    
    // Check whether a removed object is still connected somewhere, and if not update it's output dependencies
    
    void connectionRemoved(Queue *queue, T * object)
    {
        // Check that there is an object connected and that it is not connected to another input / ordering connection also
        
         if (!object)
             return;
        
        for (ConnectionIterator it = mConnections.begin(); it != mConnections.end(); it++)
            if (it->mObject == object)
                return;
        
        for (ConnectionIterator it = mOrderingConnections.begin(); it != mOrderingConnections.end(); it++)
            if (it->mObject == object)
                return;
        
        // Update dependencies
        
        object->deleteOutputDependency(queue, mParent);
    }
    
    // Delete connection and set to defaults
    
    void clearConnection(Queue *queue, unsigned long inIdx)
    {
        T *prevObject = mConnections[inIdx].mObject;
        mConnections[inIdx] = Connection();
        connectionRemoved(queue, prevObject);
    }
    
    // Delete ordering connection
    
    ConnectionIterator deleteOrderingConnection(Queue *queue, ConnectionIterator it)
    {
        T *object = it->mObject;
        it = mOrderingConnections.erase(it);
        connectionRemoved(queue, object);
        
        return it;
    }
    
    // Delete all ordering connections

    void deleteOrderingConnections(Queue *queue)
    {
        for (ConnectionIterator it = mOrderingConnections.begin(); it != mOrderingConnections.end(); )
            it = deleteOrderingConnection(queue, it);
    }

    // Remove all connections from a single object
    
    void disconnect(Queue *queue, T *object)
    {
        for (ConnectionIterator it = mConnections.begin(); it != mConnections.end(); it++)
            if (it->mObject == object)
                *it = Connection();
        
        for (ConnectionIterator it = mOrderingConnections.begin(); it != mOrderingConnections.end(); )
        {
            if (it->mObject == object)
                it = mOrderingConnections.erase(it);
            else
                it++;
        }
        
        connectionUpdate(queue);
    }
    
    // Detect Potential Feedback in a Network
    
    bool detectFeedback(T *object)
    {
        object->mFeedback = false;
        Queue queue(mParent, &T::feedbackProbe);
        return object->mFeedback;
    }

    void feedbackProbe(Queue *queue)
    {
        mFeedback = true;
        for (typename std::vector <T *>::iterator it = mOutputDependencies.begin(); it != mOutputDependencies.end(); it++)
            queue->add(*it, &T::feedbackProbe);
    }
    
    // Data
    
    const ObjectType mType;
    FrameLib_Context mContext;
    FrameLib_Context::Allocator mAllocator;
    
    void *mOwner;
    T *mParent;
    
    // IO Counts
    
    unsigned long mNumIns;
    unsigned long mNumOuts;
    unsigned long mNumAudioChans;
    
    // Connections

    std::vector<Connection> mOrderingConnections;
    std::vector<Connection> mConnections;
    std::vector<T *> mOutputDependencies;
    
    bool mSupportsOrderingConnections;
    bool mFeedback;
};

// FrameLib_Block

// This abstract class provides a connectivity interface to FrameLib_DSP objects or blocks (groups of FrameLib_DSP objects).
// Standard objects inherit this in the FrameLib_DSP class.
// Objects that have asynchronous outputs can use this class to host multiple FrameLib_DSP objects and connect them correctly.

class FrameLib_Block : public FrameLib_Object<FrameLib_Block>
{
    
protected:
    
    typedef FrameLib_Object::UntypedConnection<FrameLib_Block> Connection;
    
public:
    
    // Constructor / Destructor
    
    FrameLib_Block(ObjectType type, FrameLib_Context context, void *owner)
    : FrameLib_Object<FrameLib_Block>(type, context, owner, this) {}
    virtual ~FrameLib_Block() {}

    // Channel Awareness
    
    virtual void setChannel(unsigned long chan) {}

    // Connection Queries
    
    virtual unsigned long getNumInputObjects(unsigned long blockIdx)                        { return 1; }
    virtual Connection getInputConnection(unsigned long blockIdx, unsigned long idx)        { return Connection(this, blockIdx); }

    virtual Connection getOutputConnection(unsigned long blockIdx)                          { return Connection(this, blockIdx); }

    virtual unsigned long getNumOrderingConnectionObjects()                                 { return 1; }
    virtual FrameLib_Block *getOrderingConnectionObject(unsigned long idx)                  { return this; }
};

#endif
