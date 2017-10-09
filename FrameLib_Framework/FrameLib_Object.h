
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
        
        Queue() : mMethod(NULL), mFirst(NULL), mTop(NULL), mTail(NULL) {}
        Queue(T *object, Method method) : mMethod(NULL), mFirst(NULL), mTop(NULL), mTail(NULL) { add(object, method); }
        
        void add(T *object, Method method)
        {
            // Do not add if NULL or re-add if already in queue
            
            if (!object || object->FrameLib_Queueable<T>::mNext != NULL)
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
        
        bool equal(const UntypedConnection& connection) { return connection.mObject == mObject && connection.mIndex == mIndex; }
        
        U *mObject;
        unsigned long mIndex;
    };
    
private:
    
    // Typedefs for concision
    
    typedef UntypedConnection<T> ObjectTypeConnection;
    typedef UntypedConnection<FrameLib_Object> Connection;
    typedef UntypedConnection<const FrameLib_Object> ConstConnection;
    typedef typename std::vector<Connection>::iterator ConnectionIterator;
    typedef typename std::vector<Connection>::const_iterator ConstConnectionIterator;
    
    // A connector is a thing with one input and many outputs
    
    struct Connector
    {
        Connector() : mInternal(false) {}
        
        void addOut(Connection connection, bool setInternal)
        {
            if (setInternal)
                mInternal = true;
            
            for (ConnectionIterator it = mOut.begin(); it != mOut.end(); it++)
                if (it->equal(connection))
                    return;
            
            mOut.push_back(connection);
        }
        
        void deleteOut(Connection connection, bool setInternal)
        {
            for (ConnectionIterator it = mOut.begin(); it != mOut.end(); it++)
            {
                if (it->equal(connection))
                {
                    mOut.erase(it);
                    break;
                }
            }
            
            if (setInternal && !mOut.size())
                mInternal = false;
        }
        
        void clearOuts(bool setInternal)
        {
            mOut.clear();
            if (setInternal)
                mInternal = false;
        }

        bool mInternal;
        Connection mIn;
        std::vector<Connection> mOut;
    };
    
    typedef typename std::vector<Connector>::iterator  ConnectorIterator;

public:

    typedef typename FrameLib_Queueable<T>::Queue Queue;

    // Constructor / Destructor
    
    FrameLib_Object(ObjectType type, FrameLib_Context context, void *owner)
    : mType(type), mContext(context), mAllocator(context), mOwner(owner), mNumAudioChans(0), mSupportsOrderingConnections(false), mFeedback(false) {}
    
    virtual ~FrameLib_Object()
    {
        deleteConnections(false);
    }
   
    // Object Type
    
    ObjectType getType() const                  { return mType; }
    
    // Context
    
    FrameLib_Context getContext() const         { return mContext; }

    // Owner
    
    void *getOwner() const                      { return mOwner; }
    
    // IO Queries
    
    unsigned long getNumIns() const             { return mInputConnections.size(); }
    unsigned long getNumOuts() const            { return mOutputConnections.size(); }
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
        if (mInputConnections[inIdx].mInternal || object->mOutputConnections[outIdx].mInternal)
            return kConnectAliased;
        
        Connection connection = Connection(object, outIdx);
        
        ConnectionResult result = connectionCheck(object);

        if (result == kConnectSuccess)
            changeConnection(inIdx, connection, true);
        
        return result;
    }
    
    void deleteConnection(unsigned long inIdx)
    {
        if (mInputConnections[inIdx].mInternal)
            return;
        
        changeConnection(inIdx, Connection(), true);
    }
        
    ConnectionResult addOrderingConnection(T *object, unsigned long outIdx)
    {
        if (!supportsOrderingConnections())
            return kConnectNoOrderingSupport;
        if (mOrderingConnector.mInternal || object->mOutputConnections[outIdx].mInternal)
            return kConnectAliased;

        Connection connection = Connection(object, outIdx);
        ConnectionResult result = connectionCheck(object);

        if (result == kConnectSuccess)
        {
            // If already connected there is nothing to do
        
            for (ConnectionIterator it = mOrderingConnections.begin(); it != mOrderingConnections.end(); it++)
                if (it->equal(connection))
                    return kConnectSuccess;

            // Add the ordering connections
            
            connection.mObject->mOutputConnections[connection.mIndex].addOut(Connection(this, -1), false);
            mOrderingConnections.push_back(connection);
            
            // Notify
            
            connection.mObject->callConnectionUpdate();
            callConnectionUpdate();
        }

        return result;
    }
    
    void deleteOrderingConnection(T *object, unsigned long outIdx)
    {
        if (!supportsOrderingConnections() || mOrderingConnector.mInternal)
            return;

        for (ConnectionIterator it = mOrderingConnections.begin(); it != mOrderingConnections.end(); it++)
        {
            if (it->equal(Connection(object, outIdx)))
            {
                deleteOrderingConnection(it, true);
                return;
            }
        }
    }
    
    void clearOrderingConnections()
    {
        if (!supportsOrderingConnections() || mOrderingConnector.mInternal)
            return;
        
        deleteOrderingConnections(true);
    }
    
    void clearConnections()
    {
        deleteConnections(true);
    }
    
    // Aliasing
    
    void setInputAlias(ObjectTypeConnection alias, unsigned long inIdx)
    {
        if (!mInputConnections[inIdx].mInternal && alias.mObject)
            changeConnection(inIdx, Connection(), false);
        
        changeInputAlias(Connection(alias.mObject, alias.mIndex), inIdx, true);
    }
    
    void setOutputAlias(ObjectTypeConnection alias, unsigned long outIdx)
    {
        if (!mOutputConnections[outIdx].mInternal && alias.mObject)
            clearOutput(outIdx);
        if (alias.mObject)
            alias.mObject->changeOutputAlias(Connection(this, outIdx), alias.mIndex, true);
        
    }
    
    void setOrderingAlias(T *alias)
    {
        if (!supportsOrderingConnections())
            return;
        
        if (!mOrderingConnector.mInternal && alias)
            deleteOrderingConnections(false);
        changeOrderingAlias(alias, true);
    }
    
    // Input Connection Queries

    bool isConnected(unsigned long inIdx) const                             { return getConnection(inIdx).mObject != NULL; }
    ObjectTypeConnection getConnection(unsigned long inIdx) const           { return getConnection(inIdx, false); }
    
    bool supportsOrderingConnections() const                                { return mSupportsOrderingConnections; }
    unsigned long getNumOrderingConnections() const                         { return traverseOrderingAliasesOutwards()->mOrderingConnections.size(); }
    ObjectTypeConnection getOrderingConnection(unsigned long idx) const     { return getOrderingConnection(idx, false); }
    
    // Automatic Dependency Connections
    
    virtual void autoOrderingConnections() = 0;
    virtual void clearAutoOrderingConnections() = 0;
    
protected:
    
    // IO Connection Queries (protected)
    
    ObjectTypeConnection getConnectionInternal(unsigned long inIdx) const           { return getConnection(inIdx, true); }
    ObjectTypeConnection getOrderingConnectionInternal(unsigned long idx) const     { return getOrderingConnection(idx, true); }
    
    std::vector<T *> getOutputDependencies() const
    {
        std::vector<T *> dependencies;
        
        for (unsigned long i = 0; i < getNumOuts(); i++)
            addDependencies(dependencies, i);
        
        return dependencies;
    }
   
    // IO Setup
    
    void setIO(unsigned long nIns, unsigned long nOuts, unsigned long nAudioChans = 0)
    {
        mNumAudioChans = nAudioChans;
        
        mInputConnections.resize((getType() == kScheduler || nIns) ? nIns : 1);
        mOutputConnections.resize(nOuts);
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
    
    virtual void connectionUpdate(Queue *queue)
    {
        for (ConnectorIterator it = mInputConnections.begin(); it != mInputConnections.end(); it++)
            for (ConnectionIterator jt = it->mOut.begin(); jt != it->mOut.end(); jt++)
                queue->add(static_cast<T *>(jt->mObject), &T::FrameLib_Object::connectionUpdate);
        
        for (ConnectorIterator it = mOutputConnections.begin(); it != mOutputConnections.end(); it++)
                queue->add(static_cast<T *>(it->mIn.mObject), &T::FrameLib_Object::connectionUpdate);
    };
    
    void callConnectionUpdate()
    {
        Queue queue(static_cast<T *>(this), &T::FrameLib_Object::connectionUpdate);
    }
    
    // Input Connection Queries (with and without alias resolution
    
    ObjectTypeConnection getConnection(unsigned long inIdx, bool resolveAliases) const
    {
        Connection inputConnection = traverseInputAliasesOutwards(inIdx);
        Connection connection = inputConnection.mObject->mInputConnections[inputConnection.mIndex].mIn;
        if (resolveAliases && connection.mObject)
            connection = connection.mObject->traverseOutputAliasesInwards(connection.mIndex);
        return ObjectTypeConnection(static_cast<T*>(connection.mObject), connection.mIndex);
    }
    
    ObjectTypeConnection getOrderingConnection(unsigned long idx, bool resolveAliases) const
    {
        const FrameLib_Object *object = traverseOrderingAliasesOutwards();
        Connection connection = Connection(object->mOrderingConnections[idx].mObject, object->mOrderingConnections[idx].mIndex);
        if (resolveAliases && connection.mObject)
            connection = connection.mObject->traverseOutputAliasesInwards(connection.mIndex);
        return ObjectTypeConnection(static_cast<T*>(connection.mObject), connection.mIndex);
    }

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
    
    // Change input connection
    
    void changeConnection(unsigned long inIdx, Connection connection, bool notify)
    {
        if (!mInputConnections[inIdx].mIn.equal(connection))
        {
            // Update all values
            
            Connection prevConnection = mInputConnections[inIdx].mIn;
            
            mInputConnections[inIdx].mIn = connection;
            if (prevConnection.mObject)
                prevConnection.mObject->mOutputConnections[prevConnection.mIndex].deleteOut(Connection(this, inIdx), false);
            if (connection.mObject)
                connection.mObject->mOutputConnections[connection.mIndex].addOut(Connection(this, inIdx), false);
            
            // Notify of updates
            
            if (prevConnection.mObject)
                prevConnection.mObject->callConnectionUpdate();
            if (connection.mObject)
                connection.mObject->callConnectionUpdate();
            if (notify)
                callConnectionUpdate();
        }
    }
    
    // Delete ordering connection
    
    ConnectionIterator deleteOrderingConnection(ConnectionIterator it, bool notify)
    {
        Connection connection = *it;
        
        connection.mObject->mOutputConnections[connection.mIndex].deleteOut(Connection(this, -1), false);
        it = mOrderingConnections.erase(it);
        
        // Notify
        
        connection.mObject->callConnectionUpdate();
        
        if (notify)
            callConnectionUpdate();
        
        return it;
    }
    
    // Delete all ordering connections
    
    void deleteOrderingConnections(bool notify)
    {
        // Delete
        
        for (ConnectionIterator it = mOrderingConnections.begin(); it != mOrderingConnections.end(); )
            it = deleteOrderingConnection(it, false);
        
        // Notify
        
        if (notify)
            callConnectionUpdate();
    }
    
    // Clear output
    
    void clearOutput(unsigned long outIdx)
    {
        for (ConnectionIterator it = mOutputConnections[outIdx].mOut.begin(); it != mOutputConnections[outIdx].mOut.end(); )
        {
            // Update all values

            Connection prevConnection = *it;
            
            it = mOutputConnections[outIdx].mOut.erase(it);
            prevConnection.mObject->mInputConnections[prevConnection.mIndex].mIn = Connection();
            
            // Notify

            prevConnection.mObject->callConnectionUpdate();
        }
    }
    
    // Delete all connections
    
    void deleteConnections(bool notify)
    {
        // Clear input connections
        
        for (unsigned long i = 0; i < getNumIns(); i++)
        {
            clearInputAliases(i);
            if (mInputConnections[i].mInternal)
                changeInputAlias(Connection(), i, false);
            else
                changeConnection(i, Connection(), false);
        }
        
        // Clear ordering connections
        
        clearOrderingAliases();
        if (mOrderingConnector.mInternal)
            changeOrderingAlias(NULL, false);
        else
            deleteOrderingConnections(false);
        
        // Clear outputs
        
        for (unsigned long i = 0; i < getNumOuts(); i++)
        {
            if (mOutputConnections[i].mIn.mObject)
                changeOutputAlias(Connection(), i, false);
            if (mOutputConnections[i].mInternal)
                clearOutputAliases(i);
            else
                clearOutput(i);
        }
        
        // Notify
        
        if (notify)
            callConnectionUpdate();
    }

    // Add dependencies
    
    void addDependencies(std::vector<T *> &dependencies, unsigned long outIdx) const
    {
        if (mOutputConnections[outIdx].mInternal)
        {
            for (ConstConnectionIterator it = mOutputConnections[outIdx].mOut.begin(); it != mOutputConnections[outIdx].mOut.end(); it++)
                it->mObject->addDependencies(dependencies, it->mIndex);
        }
        else
        {
            for (ConstConnectionIterator it = mOutputConnections[outIdx].mOut.begin(); it != mOutputConnections[outIdx].mOut.end(); it++)
                it->mObject->unwrapDependencies(dependencies, it->mIndex);
        }
    }
    
    void unwrapDependencies(std::vector<T *> &dependencies, unsigned long inIdx) const
    {
        if (inIdx == -1 && mOrderingConnector.mOut.size())
        {
            for (ConstConnectionIterator it = mOrderingConnector.mOut.begin(); it != mOrderingConnector.mOut.end(); it++)
                it->mObject->unwrapDependencies(dependencies, it->mIndex);
        }
        else if (mInputConnections[inIdx].mOut.size())
        {
            for (ConstConnectionIterator it = mInputConnections[inIdx].mOut.begin(); it != mInputConnections[inIdx].mOut.end(); it++)
                it->mObject->unwrapDependencies(dependencies, it->mIndex);
        }
        else
            addDependency(dependencies, static_cast<T *>(const_cast<FrameLib_Object *>(this)));
    }
    
    static void addDependency(std::vector<T *> &dependencies, T *object)
    {
        typename std::vector<T *>::iterator jt;
        
        for (jt = dependencies.begin(); jt != dependencies.end(); jt++)
            if (*jt == object)
                break;
        
        if (jt == dependencies.end())
            dependencies.push_back(dynamic_cast<T*>(object));
    }
    
    // Aliasing
    
    Connection traverseInputAliasesOutwards(unsigned long inIdx) const
    {
        if (mInputConnections[inIdx].mInternal)
            return mInputConnections[inIdx].mIn.mObject->traverseInputAliasesOutwards(mInputConnections[inIdx].mIn.mIndex);
        
        return Connection(const_cast<FrameLib_Object *>(this), inIdx);
    }
    
    const FrameLib_Object *traverseOrderingAliasesOutwards() const
    {
        if (mOrderingConnector.mInternal)
            return mOrderingConnector.mIn.mObject->traverseOrderingAliasesOutwards();
        
        return this;
    }
    
    Connection traverseOutputAliasesInwards(unsigned long outIdx) const
    {
        if (mOutputConnections[outIdx].mIn.mObject)
            return mOutputConnections[outIdx].mIn.mObject->traverseOutputAliasesInwards(mOutputConnections[outIdx].mIn.mIndex);
        
        return Connection(const_cast<FrameLib_Object *>(this), outIdx);
    }
    
    typedef Connector& (FrameLib_Object::*ConnectorMethod)(unsigned long);
    
    Connector& getInputConnector(unsigned long idx)       { return mInputConnections[idx]; }
    Connector& getOutputConnector(unsigned long idx)      { return mOutputConnections[idx]; }
    Connector& getOrderingConnector(unsigned long idx)    { return mOrderingConnector; }
    
    void changeAlias(ConnectorMethod method, Connection alias, unsigned long idx, bool output, bool notify)
    {
        Connector& connector = (this->*method)(idx);
        Connection prevConnection = connector.mIn;
        
        // Update all values
        
        if (prevConnection.mObject)
            (prevConnection.mObject->*method)(prevConnection.mIndex).deleteOut(Connection(this, idx), output);
        if (alias.mObject)
            (alias.mObject->*method)(alias.mIndex).addOut(Connection(this, idx), output);
        
        connector.mIn = alias;
        if (!output)
            connector.mInternal = alias.mObject;
        
        // Notify of updates
        
        if ((notify || output) && prevConnection.mObject)
            prevConnection.mObject->callConnectionUpdate();
        if (alias.mObject)
            alias.mObject->callConnectionUpdate();
        if (notify)
            callConnectionUpdate();
    }

    void clearAliases(ConnectorMethod method, unsigned long idx, bool output)
    {
        Connector& connector = (this->*method)(idx);
        
        if (output && !connector.mInternal)
            return;
        
        // Remove alias without notification (avoid re-entrancy (expect that the host is in the destructor

        for (ConnectionIterator it = connector.mOut.begin(); it != connector.mOut.end(); )
        {
            Connection prevConnection = *it;
            (it->mObject->*method)(it->mIndex).mIn = Connection();
            it = connector.mOut.erase(it);
            if (!output)
                prevConnection.mObject->callConnectionUpdate();
        }
    }

    void changeInputAlias(Connection alias, unsigned long inIdx, bool notify)    { changeAlias(&FrameLib_Object::getInputConnector, alias, inIdx, false, notify); }
    void changeOutputAlias(Connection alias, unsigned long outIdx, bool notify)  { changeAlias(&FrameLib_Object::getOutputConnector, alias, outIdx, true, notify); }
    void changeOrderingAlias(T *alias, bool notify)                              { changeAlias(&FrameLib_Object::getOrderingConnector, Connection(alias, -1), -1, false, notify); }
    
    void clearInputAliases(unsigned long inIdx)     { clearAliases(&FrameLib_Object::getInputConnector, inIdx, false); }
    void clearOutputAliases(unsigned long outIdx)   { clearAliases(&FrameLib_Object::getOutputConnector, outIdx, true); }
    void clearOrderingAliases()                     { clearAliases(&FrameLib_Object::getOrderingConnector, -1, false); }
    
    // Detect Potential Feedback in a Network
    
    bool detectFeedback(T *object)
    {
        object->mFeedback = false;
        Queue queue(dynamic_cast<T*>(this), &T::feedbackProbe);
        return object->mFeedback;
    }

    void feedbackProbe(Queue *queue)
    {
        mFeedback = true;
        
        for (ConnectorIterator it = mOutputConnections.begin(); it != mOutputConnections.end(); it++)
            for (ConnectionIterator jt = it->mOut.begin(); jt != it->mOut.end(); jt++)
                queue->add(static_cast<T *>(jt->mObject), &T::feedbackProbe);
    }
    
    // Data
    
    const ObjectType mType;
    FrameLib_Context mContext;
    FrameLib_Context::Allocator mAllocator;
    
    void *mOwner;
    
    // Audio IO Counts
    
    unsigned long mNumAudioChans;
    
    // Connections

    std::vector<Connector> mInputConnections;
    std::vector<Connector> mOutputConnections;
    
    Connector mOrderingConnector;
    std::vector<Connection> mOrderingConnections;
    
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
    : FrameLib_Object<FrameLib_Block>(type, context, owner) {}
    virtual ~FrameLib_Block() {}

    // Channel Awareness
    
    virtual void setChannel(unsigned long chan) {}
};

#endif
