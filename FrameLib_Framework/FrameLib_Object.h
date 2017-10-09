
#ifndef FRAMELIB_BLOCK_H
#define FRAMELIB_BLOCK_H

#include "FrameLib_Types.h"
#include "FrameLib_Context.h"
#include "FrameLib_Parameters.h"
#include <algorithm>
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
        
        friend bool operator == (const UntypedConnection& a, const UntypedConnection& b) { return a.mObject == b.mObject && a.mIndex == b.mIndex; }
        friend bool operator != (const UntypedConnection& a, const UntypedConnection& b) { return !(a == b); }
        
        U *mObject;
        unsigned long mIndex;
    };
    
    // Typedefs for concision

    typedef typename FrameLib_Queueable<T>::Queue Queue;
    typedef UntypedConnection<T> Connection;

private:

    // Typedefs for concision
    
    typedef typename std::vector<T *>::iterator ObjectIterator;
    typedef UntypedConnection<const T> ConstConnection;
    typedef typename std::vector<Connection>::iterator ConnectionIterator;
    typedef typename std::vector<Connection>::const_iterator ConstConnectionIterator;

    // A connector is a thing with one input and many outputs
    
    struct Connector
    {
        Connector() : mInternal(false) {}
        
        void addOut(Connection connection, bool setInternal)
        {
            mInternal = setInternal ? true : mInternal;
            addUniqueItem(mOut, connection);
        }
        
        void deleteOut(Connection connection, bool setInternal)
        {
            deleteUniqueItem(mOut, connection);
            mInternal = setInternal && !mOut.size() ? false : mInternal;
        }
        
        void clearOuts(bool setInternal)
        {
            mOut.clear();
            mInternal = setInternal ? false : mInternal;
        }
        
        bool mInternal;
        Connection mIn;
        std::vector<Connection> mOut;
    };
    
    // Typedefs for concision
    
    typedef typename std::vector<Connector>::iterator  ConnectorIterator;
    typedef Connector& (FrameLib_Object::*ConnectorMethod)(unsigned long);
    typedef bool ListMethod(std::vector<Connection>&, Connection);
    typedef void (FrameLib_Object::*AlterMethod)(ConnectorMethod, Connection, unsigned long, bool);
    
public:

    // Constructor / Destructor
    
    FrameLib_Object(ObjectType type, FrameLib_Context context, void *owner)
    : mType(type), mContext(context), mAllocator(context), mOwner(owner), mNumAudioChans(0), mSupportsOrderingConnections(false), mFeedback(false) {}
    
    virtual ~FrameLib_Object()                  { clearConnections(false); }
   
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
    
    ConnectionResult addConnection(Connection connection, unsigned long inIdx)
    {
        ConnectionResult result = connectionCheck(connection, false);
        return (result == kConnectSuccess) ? changeConnection(connection, inIdx, true) : result;
    }
    
    void deleteConnection(unsigned long inIdx)
    {
        changeConnection(Connection(), inIdx, true);
    }
        
    ConnectionResult addOrderingConnection(Connection connection)
    {
        ConnectionResult result = connectionCheck(connection, true);

        if (result == kConnectSuccess)
            return changeOrderingConnection(connection, &addUniqueItem<Connection>, &FrameLib_Object::addToConnector);

        return result;
    }
    
    void deleteOrderingConnection(Connection connection)
    {
        changeOrderingConnection(connection, &deleteUniqueItem<Connection>, &FrameLib_Object::deleteFromConnector);
    }
    
    void clearOrderingConnections()
    {
        clearOrderingConnections(true);
    }
    
    void clearConnections()
    {
        clearConnections(true);
    }
    
    // Aliasing
    
    ConnectionResult setInputAlias(Connection alias, unsigned long inIdx)
    {
        ConnectionResult result = connectionCheck(alias, false);
        
        if (result == kConnectSuccess)
        {
            changeConnection(Connection(), inIdx, false);
            changeAlias(&FrameLib_Object::getInputConnector, alias, inIdx, true);
        }
        
        return result;
    }
    
    ConnectionResult setOrderingAlias(T *alias)
    {
        ConnectionResult result = connectionCheck(alias, true);

        if (result == kConnectSuccess)
        {
            clearOrderingConnections(false);
            changeOrderingAlias(alias, true);
        }
        
        return result;
    }

    ConnectionResult setOutputAlias(Connection alias, unsigned long outIdx)
    {
        ConnectionResult result = alias.mObject->connectionCheck(thisConnection(outIdx), false);

        if (result == kConnectSuccess)
        {
            clearOutput(outIdx);
            if (alias.mObject)
                alias.mObject->changeAlias(&FrameLib_Object::getOutputConnector, thisConnection(outIdx), alias.mIndex, true);
        }
        
        return result;
    }
    
    // Input Connection Queries

    bool isConnected(unsigned long inIdx) const                             { return getConnection(inIdx).mObject != NULL; }
    Connection getConnection(unsigned long inIdx) const                     { return getConnection(inIdx, false); }
    
    bool supportsOrderingConnections() const                                { return mSupportsOrderingConnections; }
    unsigned long getNumOrderingConnections() const                         { return traverseOrderingAliases()->mOrderingConnections.size(); }
    Connection getOrderingConnection(unsigned long idx) const               { return getOrderingConnection(idx, false); }
    
    // Automatic Dependency Connections
    
    virtual void autoOrderingConnections() = 0;
    virtual void clearAutoOrderingConnections() = 0;
    
    // Connection Update
    
    void callConnectionUpdate()                                            { Queue queue(static_cast<T *>(this), &T::FrameLib_Object::connectionUpdate); }
    
protected:
    
    // IO Connection Queries (protected)
    
    Connection getConnectionInternal(unsigned long inIdx) const             { return getConnection(inIdx, true); }
    Connection getOrderingConnectionInternal(unsigned long idx) const       { return getOrderingConnection(idx, true); }
    
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
    
    // Unique List Helpers
    
    template <class U> static bool addUniqueItem(std::vector<U>& list, U item)
    {
        if (std::find(list.begin(), list.end(), item) != list.end())
            return false;
        
        list.push_back(item);
        return true;
    }
    
    template <class U> static bool deleteUniqueItem(std::vector<U>& list, U item)
    {
        typename std::vector<U>::iterator it = std::find(list.begin(), list.end(), item);
        
        if (it == list.end())
            return false;

        list.erase(it);
        return true;
    }
    
private:
    
    // Connection Methods (private)
    
    Connector& getInputConnector(unsigned long idx)     { return mInputConnections[idx]; }
    Connector& getOutputConnector(unsigned long idx)    { return mOutputConnections[idx]; }
    Connector& getOrderingConnector(unsigned long idx)  { return mOrderingConnector; }

    static bool isOutput(ConnectorMethod method) { return method == &FrameLib_Object::getOutputConnector; }

    Connection thisConnection(unsigned long idx) const  { return Connection(static_cast<T *>(const_cast<FrameLib_Object *>(this)), idx); }
    
    // Add to / Delete from a Connectors Output List
    
    void addToConnector(ConnectorMethod method, Connection connection, unsigned long idx, bool setInternal = false)
    {
        if (connection.mObject)
            (connection.mObject->*method)(connection.mIndex).addOut(thisConnection(idx), setInternal);
    }
    
    void deleteFromConnector(ConnectorMethod method, Connection connection, unsigned long idx, bool setInternal = false)
    {
        if (connection.mObject)
            (connection.mObject->*method)(connection.mIndex).deleteOut(thisConnection(idx), setInternal);
    }
    
    // Default Connection Update
                                                                                         
    virtual void connectionUpdate(Queue *queue)
    {
        for (ConnectorIterator it = mInputConnections.begin(); it != mInputConnections.end(); it++)
            for (ConnectionIterator jt = it->mOut.begin(); jt != it->mOut.end(); jt++)
                queue->add(jt->mObject, &T::FrameLib_Object::connectionUpdate);
        
        for (ConnectorIterator it = mOutputConnections.begin(); it != mOutputConnections.end(); it++)
                queue->add(it->mIn.mObject, &T::FrameLib_Object::connectionUpdate);
    };
    
    // Input Connection Queries (with and without alias resolution)
    
    Connection getConnection(unsigned long inIdx, bool resolveAliases) const
    {
        Connection inputConnection = traverseAliases(&FrameLib_Object::getInputConnector, inIdx);
        Connection connection = inputConnection.mObject->mInputConnections[inputConnection.mIndex].mIn;
        if (resolveAliases && connection.mObject)
            connection = connection.mObject->traverseAliases(&FrameLib_Object::getOutputConnector, connection.mIndex);
        return connection;
    }
    
    Connection getOrderingConnection(unsigned long idx, bool resolveAliases) const
    {
        Connection connection = traverseOrderingAliases()->mOrderingConnections[idx];
        if (resolveAliases && connection.mObject)
            connection = connection.mObject->traverseAliases(&FrameLib_Object::getOutputConnector, connection.mIndex);
        return connection;
    }

    // Connection Check
    
    ConnectionResult connectionCheck(Connection connection, bool ordering)
    {
        if (ordering && !supportsOrderingConnections())
            return kConnectNoOrderingSupport;
    
        if (connection.mObject == this)
            return kConnectSelfConnection;
    
        if (connection.mObject->mContext != mContext)
            return kConnectWrongContext;
    
        if (detectFeedback(connection.mObject))
            return kConnectFeedbackDetected;
        
        return kConnectSuccess;
    }
    
    // Notifications
    
    void notifySelf(bool notify = true)
    {
        if (notify)
            callConnectionUpdate();
    }

    void notifyConnectionsChanged(Connection connection)
    {
        if (connection.mObject)
            connection.mObject->notifySelf();
    }
    
    void notifyAliasChanged(ConnectorMethod method, Connection connection)
    {
        if (connection.mObject)
            notifyConnectionsChanged(connection.mObject->traverseAliases(method, connection.mIndex));
    }

    // Change Input Connection

    ConnectionResult changeConnection(Connection connection, unsigned long inIdx, bool notify)
    {
        if (mInputConnections[inIdx].mIn == connection)
            return kConnectSuccess;
     
        if (mInputConnections[inIdx].mInternal || (connection.mObject && connection.mObject->mOutputConnections[connection.mIndex].mInternal))
            return kConnectAliased;
        
        // Update all values (note the swap)
            
        std::swap(mInputConnections[inIdx].mIn, connection);
        deleteFromConnector(&FrameLib_Object::getOutputConnector, connection, inIdx);
        addToConnector(&FrameLib_Object::getOutputConnector, mInputConnections[inIdx].mIn, inIdx);
        
        // Notify of updates
            
        notifyConnectionsChanged(connection);
        notifyConnectionsChanged(mInputConnections[inIdx].mIn);
        notifySelf();
        
        return kConnectSuccess;
    }
    
    // Change Ordering Connection
    
    ConnectionResult changeOrderingConnection(Connection connection, ListMethod listUpdate, AlterMethod alterConnector)
    {
        if (!supportsOrderingConnections())
            return kConnectNoOrderingSupport;
        
        if (mOrderingConnector.mInternal || connection.mObject->mOutputConnections[connection.mIndex].mInternal)
            return kConnectAliased;
        
        // Add / Delete and update all values
        
        if (!listUpdate(mOrderingConnections, connection))
            return kConnectSuccess;
        
        (this->*alterConnector)(&FrameLib_Object::getOutputConnector, connection, -1, false);
        
        // Notify
        
        notifyConnectionsChanged(connection);
        notifySelf();
        
        return kConnectSuccess;
    }
    
    // Clear Ordering Connections
    
    void clearOrderingConnections(bool notify)
    {
        if (!supportsOrderingConnections() || mOrderingConnector.mInternal)
            return;
        
        while (mOrderingConnections.size())
        {
            // Update all values
            
            Connection connection = mOrderingConnections.back();
            mOrderingConnections.pop_back();
            deleteFromConnector(&FrameLib_Object::getOutputConnector, connection, -1);
            
            // Notify
            
            notifyConnectionsChanged(connection);
        }
        
        // Notify
        
        notifySelf(notify);
    }
    
    // Clear output
    
    void clearOutput(unsigned long outIdx)
    {
        while (!mOutputConnections[outIdx].mInternal && mOutputConnections[outIdx].mOut.size())
        {
            // Update all values

            Connection connection = mOutputConnections[outIdx].mOut.back();
            mOutputConnections[outIdx].mOut.pop_back();
            connection.mObject->mInputConnections[connection.mIndex].mIn = Connection();
            
            // Notify

            notifyConnectionsChanged(connection);
        }
    }
    
    // Clear All Connections
    
    void clearConnections(bool notify)
    {
        // Clear input connections
        
        for (unsigned long i = 0; i < getNumIns(); i++)
        {
            changeConnection(Connection(), i, false);
            changeAlias(&FrameLib_Object::getInputConnector, Connection(), i, false);
            clearAliases(&FrameLib_Object::getInputConnector, i);
        }
        
        // Clear ordering connections
        
        clearOrderingConnections(false);
        changeOrderingAlias(NULL, false);
        clearAliases(&FrameLib_Object::getOrderingConnector, -1);
        
        // Clear outputs
        
        for (unsigned long i = 0; i < getNumOuts(); i++)
        {
            clearOutput(i);
            changeAlias(&FrameLib_Object::getOutputConnector, Connection(), i, false);
            clearAliases(&FrameLib_Object::getOutputConnector, i);
        }
        
        // Notify
        
        notifySelf(notify);
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
            addUniqueItem(dependencies, static_cast<T *>(const_cast<FrameLib_Object *>(this)));
    }
    
    // Aliasing Methods
    
    Connection traverseAliases(ConnectorMethod method, unsigned long idx) const
    {
        const Connector& connector = (const_cast<FrameLib_Object *>(this)->*method)(idx);

        if ((!isOutput(method) && connector.mInternal) || (isOutput(method) && connector.mIn.mObject))
            return connector.mIn.mObject->traverseAliases(method, connector.mIn.mIndex);
    
        return thisConnection(idx);
    }
    
    void changeAlias(ConnectorMethod method, Connection alias, unsigned long idx, bool notify)
    {
        Connector& connector = (this->*method)(idx);
        
        if (connector.mIn.mObject && !isOutput(method) && !connector.mInternal)
            return;
        
        // Update all values (note the swap)
        
        std::swap(connector.mIn, alias);
        connector.mInternal = isOutput(method) ? connector.mInternal : (bool) connector.mIn.mObject;
        deleteFromConnector(method, alias, idx, isOutput(method));
        addToConnector(method, connector.mIn, idx, isOutput(method));
        
        // Notify of updates
        
        notifyAliasChanged(method, alias);
        notifyAliasChanged(method, connector.mIn);
        notifySelf(notify);
    }
    
    void clearAliases(ConnectorMethod method, unsigned long idx)
    {
        std::vector<T *> dependencies;
        Connector& connector = (this->*method)(idx);

        if (isOutput(method) && !connector.mInternal)
            return;
        
        // Create a list of dependencies
        
        if (isOutput(method))
            addDependencies(dependencies, idx);
        else
            unwrapDependencies(dependencies, idx);
        
        // Clear

        connector.clearOuts(isOutput(method));
        
        // Notify
        
        for (ObjectIterator it = dependencies.begin(); it != dependencies.end(); it++)
            (*it)->notifySelf();
    }

    // Simply Ordering Calls
    
    const FrameLib_Object *traverseOrderingAliases() const      { return traverseAliases(&FrameLib_Object::getOrderingConnector, -1).mObject; }
    void changeOrderingAlias(T *alias, bool notify)             { changeAlias(&FrameLib_Object::getOrderingConnector, Connection(alias, -1), -1, notify); }
    
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
// Objects that have asynchronous outputs can use this class to host multiple FrameLib_DSP objects and alias the connections correctly.

class FrameLib_Block : public FrameLib_Object<FrameLib_Block>
{
    
public:
    
    // Constructor / Destructor
    
    FrameLib_Block(ObjectType type, FrameLib_Context context, void *owner) : FrameLib_Object<FrameLib_Block>(type, context, owner) {}
    virtual ~FrameLib_Block() {}

    // Channel Awareness
    
    virtual void setChannel(unsigned long chan) {}
};

#endif
