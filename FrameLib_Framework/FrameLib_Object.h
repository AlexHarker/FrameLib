
#ifndef FRAMELIB_BLOCK_H
#define FRAMELIB_BLOCK_H

#include "FrameLib_Types.h"
#include "FrameLib_Context.h"
#include "FrameLib_Parameters.h"
#include "FrameLib_ProcessingQueue.h"
#include "FrameLib_Queues.h"

#include <algorithm>
#include <string>
#include <sstream>
#include <iostream>

/**
 
 @defgroup DSP Processing Objects
  
 */


/**
 
 @class FrameLib_Connection
 
 @ingroup DSP
 
 @brief an abstract template class holds the connected object and IO indices for object connections of arbitrary type.
 
 @sa FrameLib_Block, FrameLib_DSP FrameLib_Multistream
 
 */

template <class T, typename U>
struct FrameLib_Connection
{
    FrameLib_Connection() : mObject(nullptr), mIndex(0) {}
    FrameLib_Connection(T *object, unsigned long index) : mObject(object), mIndex(index) {}
    
    friend bool operator == (const FrameLib_Connection& a, const FrameLib_Connection& b)
    {
        return a.mObject == b.mObject && a.mIndex == b.mIndex;
    }
    
    friend bool operator != (const FrameLib_Connection& a, const FrameLib_Connection& b)
    {
        return !(a == b);
    }
    
    T *mObject;
    U mIndex;
};

/**
 
 @class FrameLib_Object
 
 @ingroup DSP
 
 @brief an abstract template class providing an interface for FrameLib objects and implementing connectivity.

 
 @sa FrameLib_Block, FrameLib_DSP FrameLib_Multistream
 
 */

template <class T>
class FrameLib_Object : public FrameLib_MethodQueue<T>::Node
{
    
public:
    
    using Queue = FrameLib_MethodQueue<T>;
    using Connection = FrameLib_Connection<T, unsigned long>;

    // An allocator that you can pass to other objects/code whilst this object exists
    
    class Allocator
    {
    public:
        
        Allocator(FrameLib_Object& object) : mObject(object) {}
        
        template <class U>
        U *allocate(size_t N)       { return mObject.alloc<U>(N); }
        
        template <class U>
        void deallocate(U *& ptr)   { mObject.dealloc(ptr); }
        
    private:
        
        FrameLib_Object &mObject;
    };
    
private:

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
    
    // Connector method typedef and kOrdering definition
    
    typedef Connector& (FrameLib_Object::*ConnectorMethod)(unsigned long);

    const unsigned long kOrdering = -1;

public:

    // Constructor / Destructor
    
    FrameLib_Object(ObjectType type, FrameLib_Context context, FrameLib_Proxy *proxy)
    : mType(type), mContext(context), mAllocator(context), mLocalAllocator(nullptr), mProxy(proxy), mNumAudioChans(0), mSupportsOrderingConnections(false), mFeedback(false) {}
    
    virtual ~FrameLib_Object()                  { clearConnections(false); }
   
    // Object Type
    
    ObjectType getType() const                  { return mType; }
    
    // Context and Error Reporter
    
    FrameLib_Context getContext() const         { return mContext; }
    FrameLib_ErrorReporter& getReporter() const { return *(mContext.getGlobal()); }

    // Owner
    
    FrameLib_Proxy *getProxy() const            { return mProxy; }
    
    template <class U>
    U *castProxy(FrameLib_Proxy *proxy)
    {
        return proxy ? dynamic_cast<U *>(proxy) : nullptr;
    }
    
    template <class U>
    U *cloneProxy(FrameLib_Proxy *proxy)
    {
        U *typedProxy = castProxy<U>(proxy);
        return typedProxy ? dynamic_cast<U *>(typedProxy)->clone() : nullptr;
    }
    
    // IO Queries
    
    unsigned long getNumIns() const             { return static_cast<unsigned long>(mInputConnections.size()); }
    unsigned long getNumOuts() const            { return static_cast<unsigned long>(mOutputConnections.size()); }
    unsigned long getNumAudioIns() const        { return getType() != kOutput ? mNumAudioChans : 0; }
    unsigned long getNumAudioOuts() const       { return getType() == kOutput ? mNumAudioChans : 0; }
    unsigned long getNumAudioChans() const      { return mNumAudioChans; }
    
    // Set / Get Fixed Inputs
    
    virtual void setFixedInput(unsigned long idx, const double *input, unsigned long size) = 0;
    virtual const double *getFixedInput(unsigned long idx, unsigned long *size) = 0;

    // Audio Processing

    // Override to handle audio at the block level (reset called with the audio engine resets)
    
    virtual uint64_t getBlockTime() const = 0;
    virtual void blockUpdate(const double * const *ins, double **outs, unsigned long blockSize, FrameLib_AudioQueue& queue) = 0;
    virtual void blockUpdate(const double * const *ins, double **outs, unsigned long blockSize) = 0;
    virtual void reset(double samplingRate, unsigned long maxBlockSize) = 0;
    
    // Return to host to request to be passed audio
    
    bool handlesAudio() const  { return mType == kScheduler || getNumAudioChans(); }
    
    // Info

    virtual std::string objectInfo(bool verbose = false)                        { return "No object info available";  }
    virtual std::string inputInfo(unsigned long idx, bool verbose = false)      { return "No input info available";  }
    virtual std::string outputInfo(unsigned long idx, bool verbose = false)     { return "No output info available";  }
    virtual std::string audioInfo(unsigned long idx, bool verbose = false)      { return "No audio channel info available";  }
   
    virtual FrameType inputType(unsigned long idx) const = 0;
    virtual FrameType outputType(unsigned long idx) const = 0;
    
    // N.B. Parameter objects can be queried directly for info
    
    virtual const FrameLib_Parameters *getParameters() const                    { return nullptr; }
    
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
            return addOrderingConnection(connection, true);

        return result;
    }
    
    void deleteOrderingConnection(Connection connection)
    {
        deleteOrderingConnection(connection, true);
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
        ConnectionResult result = connectionCheck(Connection(alias, 0), true);

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

    bool isConnected(unsigned long inIdx) const                         { return getConnection(inIdx).mObject != nullptr; }
    Connection getConnection(unsigned long inIdx) const                 { return getConnection(inIdx, false); }
    
    bool supportsOrderingConnections() const                            { return mSupportsOrderingConnections; }
    Connection getOrderingConnection(unsigned long idx) const           { return getOrderingConnection(idx, false); }
    
    unsigned long getNumOrderingConnections() const
    {
        return static_cast<unsigned long>(traverseOrderingAliases()->mOrderingConnections.size());
    }
    
    // Automatic Dependency Connections
    
    virtual void autoOrderingConnections() = 0;
    virtual void clearAutoOrderingConnections() = 0;
    
    // Connection Update
    
    void callConnectionUpdate()
    {
        Queue queue(static_cast<T *>(this), &T::FrameLib_Object::connectionUpdate);
    }
    
    template <class U>
    void addOutputDependencies(std::vector<U *> &dependencies)
    {
        for (unsigned long i = 0; i < getNumOuts(); i++)
            addOutputDependencies(dependencies, i);
    }
    
    template <class U>
    void addOutputDependencies(std::vector<U *> &dependencies, unsigned long outIdx)
    {
        addOutputDependencies<std::vector<U *>>(dependencies, outIdx);
    }
    
protected:
    
    // IO Connection Queries (protected)
    
    Connection getConnectionInternal(unsigned long inIdx) const         { return getConnection(inIdx, true); }
    Connection getOrderingConnectionInternal(unsigned long idx) const   { return getOrderingConnection(idx, true); }
    
    void addOutputDependencies(Queue *queue)
    {
        for (unsigned long i = 0; i < getNumOuts(); i++)
            addOutputDependencies(queue, i);
    }
    
    void addOutputDependencies(Queue *queue, unsigned long outIdx)
    {
        addOutputDependencies<Queue *>(queue, outIdx);
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
    
    template <class U>
    U *alloc(size_t N)
    {
        FrameLib_LocalAllocator *allocator = mLocalAllocator;

        if (allocator)
            return reinterpret_cast<U *>(allocator->alloc(sizeof(U) * N));
        else
            return reinterpret_cast<U *>(mAllocator->alloc(sizeof(U) * N));
    }

    template <class U>
    void dealloc(U *& ptr)
    {
        FrameLib_LocalAllocator *allocator = mLocalAllocator;

        if (allocator)
            allocator->dealloc(ptr);
        else
            mAllocator->dealloc(ptr);
        
        ptr = nullptr;
    }
    
    void setLocalAllocator(FrameLib_LocalAllocator *allocator)      { mLocalAllocator = allocator; }
    void removeLocalAllocator()                                     { mLocalAllocator = nullptr; }
    void pruneAllocator()                                           { mAllocator->prune(); }
    
    FrameLib_ContextAllocator::Storage *registerStorage(const char *name)
    {
        return mAllocator->registerStorage(name);
    }
    
    void releaseStorage(FrameLib_ContextAllocator::Storage *&storage)
    {
        mAllocator->releaseStorage(storage->getName());
        storage = nullptr;
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
    
    static std::string parameterInputInfo(bool verbose)
    {
        return formatInfo("Parameter Update - tagged input updates parameters", "Parameter Update", verbose);
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
    
    template <class U>
    static bool addUniqueItem(std::vector<U>& list, U item)
    {
        if (std::find(list.begin(), list.end(), item) != list.end())
            return false;
        
        list.push_back(item);
        return true;
    }
    
    template <class U>
    static bool deleteUniqueItem(std::vector<U>& list, U item)
    {
        auto it = std::find(list.begin(), list.end(), item);
        
        if (it == list.end())
            return false;

        list.erase(it);
        return true;
    }
    
    // Input getter helper for empty inputs
    
    const double *getEmptyFixedInput(unsigned long idx, unsigned long *size)
    {
        *size = 0;
        return nullptr;
    }

private:
    
    // Connection Methods (private)
    
    Connector& getInputConnector(unsigned long idx)     { return mInputConnections[idx]; }
    Connector& getOutputConnector(unsigned long idx)    { return mOutputConnections[idx]; }
    Connector& getOrderingConnector(unsigned long idx)  { return mOrderingConnector; }

    static bool isOutput(ConnectorMethod method) { return method == &FrameLib_Object::getOutputConnector; }

    Connection thisConnection(unsigned long idx) const  { return Connection(static_cast<T *>(const_cast<FrameLib_Object *>(this)), idx); }
    
    // Add to / Delete from a Connector's Output List
    
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
    
    // Queue the Dependencies of a Vector of Connectors
    
    void queueConnectorVectorDependencies(Queue *queue, const std::vector<Connector>& connectors) const
    {
        for (auto it = connectors.begin(); it != connectors.end(); it++)
            for (auto jt = it->mOut.begin(); jt != it->mOut.end(); jt++)
                queue->add(jt->mObject);
    }
    
    // Default Connection Update
                                                                                         
    virtual void connectionUpdate(Queue *queue)
    {
        queueConnectorVectorDependencies(queue, mInputConnections);
        
        for (auto it = mOutputConnections.begin(); it != mOutputConnections.end(); it++)
            queue->add(it->mIn.mObject);
    };
    
    // Input Connection Queries (with and without alias resolution)
    
    Connection getConnection(unsigned long inIdx, bool resolveAliases) const
    {
        Connection connection = traverseAliases(&FrameLib_Object::getInputConnector, inIdx);
        connection = connection.mObject->mInputConnections[connection.mIndex].mIn;
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
    
    void notifySelf(bool notify, Queue *queue = nullptr)
    {
        if (notify)
        {
            if (queue)
                queue->add(dynamic_cast<T *>(const_cast<FrameLib_Object *>(this)));
            else
                callConnectionUpdate();
        }
    }

    void notifyConnectionsChanged(Connection connection, Queue *queue = nullptr)
    {
        if (connection.mObject)
        {
            if (queue)
                queue->add(connection.mObject);
            else
                connection.mObject->callConnectionUpdate();
        }
    }
    
    void notifyAliasChanged(ConnectorMethod method, Connection connection, Queue *queue)
    {
        if (!connection.mObject)
            return;
        
        if (method == &FrameLib_Object::getOrderingConnector)
        {
            FrameLib_Object *object = connection.mObject->traverseOrderingAliases();
            std::vector<Connection> &connections = object->mOrderingConnections;
            
            for (auto it = connections.begin(); it != connections.end(); it++)
                notifyConnectionsChanged(*it, queue);
        }
        else
        {
            connection = connection.mObject->traverseAliases(method, connection.mIndex);
            notifyConnectionsChanged((connection.mObject->*method)(connection.mIndex).mIn, queue);
        }
    }

    // Change Input Connection

    ConnectionResult changeConnection(Connection connection, unsigned long inIdx, bool notify, Queue *queue = nullptr)
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
            
        notifyConnectionsChanged(connection, queue);
        notifyConnectionsChanged(mInputConnections[inIdx].mIn, queue);
        notifySelf(notify, queue);
        
        return kConnectSuccess;
    }
    
    // Change Ordering Connection
    
    typedef bool ListMethod(std::vector<Connection>&, Connection);
    typedef void (FrameLib_Object::*AlterMethod)(ConnectorMethod, Connection, unsigned long, bool);
    
    ConnectionResult changeOrderingConnection(Connection connection, ListMethod listUpdate, AlterMethod alterConnector, bool notify, Queue *queue)
    {
        if (!supportsOrderingConnections())
            return kConnectNoOrderingSupport;
        
        if (mOrderingConnector.mInternal || connection.mObject->mOutputConnections[connection.mIndex].mInternal)
            return kConnectAliased;
        
        // Add / Delete and update all values
        
        if (!listUpdate(mOrderingConnections, connection))
            return kConnectSuccess;
        
        (this->*alterConnector)(&FrameLib_Object::getOutputConnector, connection, kOrdering, false);
        
        // Notify (use queue to minimise calls, and ensure all changes are already complete)
        
        notifyConnectionsChanged(connection, queue);
        notifySelf(notify, queue);
        
        return kConnectSuccess;
    }
    
    ConnectionResult addOrderingConnection(Connection connection, bool notify, Queue *queue = nullptr)
    {
        return changeOrderingConnection(connection, &addUniqueItem<Connection>, &FrameLib_Object::addToConnector, notify, queue);
    }
    
    void deleteOrderingConnection(Connection connection, bool notify, Queue *queue = nullptr)
    {
        changeOrderingConnection(connection, &deleteUniqueItem<Connection>, &FrameLib_Object::deleteFromConnector, notify, queue);
    }
    
    // Clear Ordering Connections
    
    void clearOrderingConnections(bool notify, Queue *queue = nullptr)
    {
        if (!supportsOrderingConnections() || mOrderingConnector.mInternal)
            return;
        
        while (mOrderingConnections.size())
        {
            // Update all values
            
            Connection connection = mOrderingConnections.back();
            mOrderingConnections.pop_back();
            deleteFromConnector(&FrameLib_Object::getOutputConnector, connection, kOrdering);
            
            // Notify
            
            notifyConnectionsChanged(connection, queue);
        }
        
        // Notify
        
        notifySelf(notify, queue);
    }
    
    // Clear output
    
    void clearOutput(unsigned long outIdx, Queue *queue = nullptr)
    {
        while (!mOutputConnections[outIdx].mInternal && mOutputConnections[outIdx].mOut.size())
        {
            // Update all values

            Connection connection = mOutputConnections[outIdx].mOut.back();
            mOutputConnections[outIdx].mOut.pop_back();
            
            if (connection.mIndex == kOrdering)
                connection.mObject->deleteOrderingConnection(thisConnection(outIdx), false, queue);
            else
                connection.mObject->mInputConnections[connection.mIndex].mIn = Connection();
            
            // Notify

            notifyConnectionsChanged(connection, queue);
        }
    }
    
    // Clear All Connections
    
    void clearConnections(bool notify)
    {
        // Clear input connections
        
        Queue queue;
        
        for (unsigned long i = 0; i < getNumIns(); i++)
        {
            changeConnection(Connection(), i, false, &queue);
            changeAlias(&FrameLib_Object::getInputConnector, Connection(), i, false, &queue);
            clearAliases(&FrameLib_Object::getInputConnector, i, &queue);
        }
        
        // Clear ordering connections
        
        clearOrderingConnections(false, &queue);
        changeOrderingAlias(nullptr, false, &queue);
        clearAliases(&FrameLib_Object::getOrderingConnector, kOrdering, &queue);
        
        // Clear outputs
        
        for (unsigned long i = 0; i < getNumOuts(); i++)
        {
            clearOutput(i, &queue);
            changeAlias(&FrameLib_Object::getOutputConnector, Connection(), i, false, &queue);
            clearAliases(&FrameLib_Object::getOutputConnector, i, &queue);
        }
        
        // Notify
        
        notifySelf(notify, &queue);
        
        queue.start(&T::FrameLib_Object::connectionUpdate);
    }

    // Add Output Dependencies
        
    void addDependency(Queue *queue) const
    {
        queue->add(dynamic_cast<T *>(const_cast<FrameLib_Object *>(this)));
    }
    
    template <class U>
    void addDependency(std::vector<U *>& dependencies) const
    {
        U *object = dynamic_cast<U *>(const_cast<FrameLib_Object *>(this));

        if (object)
            addUniqueItem(dependencies, object);
    }
    
    template <class U>
    void traverseDependencies(U& dependencies, const Connector& connector, void (FrameLib_Object::*method)(U&, unsigned long) const) const
    {
        for (auto it = connector.mOut.begin(); it != connector.mOut.end(); it++)
            (it->mObject->*method)(dependencies, it->mIndex);
    }
    
    template <class U>
    void addOutputDependencies(U &dependencies, unsigned long outIdx) const
    {
        if (mOutputConnections[outIdx].mInternal)
            traverseDependencies(dependencies, mOutputConnections[outIdx], &FrameLib_Object::addOutputDependencies<U>);
        else
            traverseDependencies(dependencies, mOutputConnections[outIdx], &FrameLib_Object::unwrapInputAliases<U>);
    }
    
    template <class U>
    void unwrapInputAliases(U& dependencies, unsigned long inIdx) const
    {
        if (inIdx == kOrdering && mOrderingConnector.mOut.size())
            traverseDependencies(dependencies, mOrderingConnector, &FrameLib_Object::unwrapInputAliases<U>);
        else if (inIdx != kOrdering && mInputConnections[inIdx].mOut.size())
            traverseDependencies(dependencies, mInputConnections[inIdx], &FrameLib_Object::unwrapInputAliases<U>);
        else
            addDependency(dependencies);
    }
    
    // Aliasing Methods
    
    Connection traverseAliases(ConnectorMethod method, unsigned long idx) const
    {
        const Connector& connector = (const_cast<FrameLib_Object *>(this)->*method)(idx);

        if ((!isOutput(method) && connector.mInternal) || (isOutput(method) && connector.mIn.mObject))
            return connector.mIn.mObject->traverseAliases(method, connector.mIn.mIndex);
    
        return thisConnection(idx);
    }
    
    void changeAlias(ConnectorMethod method, Connection alias, unsigned long idx, bool notify, Queue *queue = nullptr)
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
        
        notifyAliasChanged(method, alias, queue);
        notifyAliasChanged(method, connector.mIn, queue);
        notifySelf(notify, queue);
    }
    
    void clearAliases(ConnectorMethod method, unsigned long idx, Queue *queue)
    {
        // N.B. Queue pointer allows the pointer to be passed by reference (as required for the vector versions)
    
        Connector& connector = (this->*method)(idx);

        if (isOutput(method) && !connector.mInternal)
            return;
        
        // Create a list of dependencies
        
        if (isOutput(method))
            addOutputDependencies(queue, idx);
        else
            unwrapInputAliases(queue, idx);
        
        // Remove from aliased objects and clear

        for (auto it = connector.mOut.begin(); it != connector.mOut.end(); it++)
            (it->mObject->*method)(it->mIndex).mIn = Connection();
        connector.clearOuts(isOutput(method));
    }

    // Simpler Ordering Calls
    
    FrameLib_Object *traverseOrderingAliases() const      { return traverseAliases(&FrameLib_Object::getOrderingConnector, kOrdering).mObject; }
    
    void changeOrderingAlias(T *alias, bool notify, Queue *queue = nullptr)
    {
        changeAlias(&FrameLib_Object::getOrderingConnector, Connection(alias, kOrdering), kOrdering, notify, queue);
    }
    
    // Detect Potential Feedback in a Network
    
    bool detectFeedback(T *object)
    {
        object->mFeedback = false;
        Queue queue(static_cast<T*>(this), &T::feedbackProbe);
        return object->mFeedback;
    }
    
    void feedbackProbe(Queue *queue)
    {
        mFeedback = true;
        queueConnectorVectorDependencies(queue, mOutputConnections);
    }
    
    // Data
    
    const ObjectType mType;
    FrameLib_Context mContext;
    FrameLib_Context::Allocator mAllocator;
    FrameLib_LocalAllocator* mLocalAllocator;

    FrameLib_Proxy *mProxy;
    
    // Audio IO Counts
    
    unsigned long mNumAudioChans;
    
    // Connections

    std::vector<Connector> mInputConnections;
    std::vector<Connector> mOutputConnections;
    std::vector<Connection> mOrderingConnections;
    Connector mOrderingConnector;
    
    bool mSupportsOrderingConnections;
    bool mFeedback;
};


/**
 
 @class FrameLib_Block
 
 @ingroup DSP

 @brief an abstract class that represents either a single FrameLib_DSP object, or a group of connected FrameLib_DSP objects.
 
    This abstract class provides a connectivity interface to FrameLib_DSP objects or blocks (groups of FrameLib_DSP objects). Most objects inherit this in the FrameLib_DSP class. Objects that have asynchronous outputs can use this class to host multiple FrameLib_DSP objects and alias the connections correctly.
 
 */

class FrameLib_Block : public FrameLib_Object<FrameLib_Block>
{
    
public:
    
    // Constructor / Destructor
    
    FrameLib_Block(ObjectType type, FrameLib_Context context, FrameLib_Proxy *proxy) : FrameLib_Object<FrameLib_Block>(type, context, proxy) {}
    virtual ~FrameLib_Block() {}

    // Stream Awareness
    
    virtual void setStream(void *streamOwner, unsigned long stream) {}
};

#endif
