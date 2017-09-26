
#ifndef FRAMELIB_BLOCK_H
#define FRAMELIB_BLOCK_H

#include "FrameLib_Types.h"
#include "FrameLib_Context.h"
#include "FrameLib_Parameters.h"
#include <string>
#include <sstream>
#include <iostream>

// FrameLib_Object

// This abstract template class outlines the basic functionality that objects (blocks / DSP / multichannel must provide)

template <class T>
class FrameLib_Object 
{
    struct InputConnection
    {
        InputConnection() : mObject(NULL), mIndex(0) {}
        InputConnection(T *object, unsigned long index) : mObject(object), mIndex(index) {}
        
        T *mObject;
        unsigned long mIndex;
    };
    
    typedef typename std::vector< T *>::iterator ObjectIterator;
    typedef typename std::vector< InputConnection>::iterator ConnectionIterator;
    
public:
    
    // Constructor / Destructor
    
    FrameLib_Object(ObjectType type, FrameLib_Context context, void *owner, T *parent)
    : mType(type), mContext(context), mOwner(owner), mParent(parent), mNumIns(0), mNumOuts(0), mNumAudioChans(0), mSupportsDependencyConnections(false), mFeedback(false) {}
    virtual ~FrameLib_Object() {}
   
    // Object Type
    
    ObjectType getType() const                  { return mType; }
    
    // Context
    
    FrameLib_Context getContext() const         { return mContext; }

    // Owner
    
    void *getOwner() const                      { return mOwner; }
    
    // Queries
    
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
   
    virtual FrameType inputType(unsigned long idx) = 0;
    virtual FrameType outputType(unsigned long idx) = 0;
    
    // N.B. Parameter objects can be queried directly for info
    
    virtual const FrameLib_Parameters *getParameters()                          { return NULL;  }
    
    // Connection 
    
    ConnectionResult addConnection(T *object, unsigned long outIdx, unsigned long inIdx)
    {
        ConnectionResult result = connectionCheck(object);
        
        if (result == kConnectSuccess)
        {
            // Store data about connection and reset the dependency count
            
            T *prevObject = mInputConnections[inIdx].mObject;
            mInputConnections[inIdx] = InputConnection(object, outIdx);
            
            // Update dependencies if the connection is now from a different object
            
            if (prevObject != object)
            {
                removeInputDependency(prevObject);
                object->addOutputDependency(mParent);
            }
           
            connectionUpdate();
        }
        
        return result;
    }
    
    void deleteConnection(unsigned long inIdx)
    {
        clearConnection(inIdx);
        connectionUpdate();
    }
    
    ConnectionResult addDependencyConnection(T *object, unsigned long outIdx)
    {
        if (!supportsDependencyConnections())
            return kConnectNoDependencySupport;
            
        ConnectionResult result = connectionCheck(object);
        
        if (result == kConnectSuccess)
        {
            // If already connected there is nothing to do
        
            for (ConnectionIterator it = mDependencyConnections.begin(); it != mDependencyConnections.end(); it++)
                if (it->mObject == object && it->mIndex == outIdx)
                    return kConnectSuccess;

            // Update dependencies
            
            object->addOutputDependency(mParent);

            // Add the dependency connection
            
            mDependencyConnections.push_back(InputConnection(object, outIdx));
            
            connectionUpdate();
        }

        return result;
    }
    
    void deleteDependencyConnection(T *object, unsigned long outIdx)
    {
        for (ConnectionIterator it = mDependencyConnections.begin(); it != mDependencyConnections.end(); it++)
            if (it->mObject == object && it->mIndex == outIdx)
                it = deleteDependencyConnection(it);
        
        connectionUpdate();
    }
    
    void clearDependencyConnections()
    {
        deleteDependencyConnections();
        connectionUpdate();
    }
    
    void clearConnections()
    {
        // Remove input connections
        
        for (unsigned long i = 0; i < mInputConnections.size(); i++)
            clearConnection(i);
        
        // Remove dependency connections
        
        deleteDependencyConnections();
        
        // Remove output connections
        
        for (ObjectIterator it = mOutputDependencies.begin(); it != mOutputDependencies.end(); )
        {
            (*it)->disconnect(mParent);
            it = mOutputDependencies.erase(it);
        }
        
        connectionUpdate();
    }
    
    bool isConnected(unsigned long inIdx)
    {
        return mInputConnections[inIdx].mObject != NULL;
    }

    // Connection Access
    
    bool supportsDependencyConnections() const                          { return mSupportsDependencyConnections; }
    unsigned long getNumOutputDependencies() const                      { return mOutputDependencies.size(); }
    T *getOutputDependency(unsigned long idx) const                     { return mOutputDependencies[idx]; }
    unsigned long getNumDependencyConnections() const                   { return mDependencyConnections.size(); }
    T *getDependencyConnection(unsigned long idx) const                 { return mDependencyConnections[idx].mObject; }
    unsigned long getDependencyConnectionIdx(unsigned long idx) const   { return mDependencyConnections[idx].mIndex; }
    T *getInputConnection(unsigned long idx) const                      { return mInputConnections[idx].mObject; }
    unsigned long getInputConnectionIdx(unsigned long idx) const        { return mInputConnections[idx].mIndex; }
    
protected:
    
    // IO Setup
    
    void setIO(unsigned long nIns, unsigned long nOuts, unsigned long nAudioChans = 0)
    {
        mNumIns = (getType() == kScheduler || nIns) ? nIns : 1;
        mNumOuts = nOuts;
        mNumAudioChans = nAudioChans;
        
        mInputConnections.resize(mNumIns);
    }
    
    // Dependency Setup
    
    void enableDependencyConnections()                         { mSupportsDependencyConnections = true; }

    // Info Helpers
    
    const char *formatInfo(const char *verboseStr, const char *briefStr, bool verbose)
    {
        return verbose ? verboseStr : briefStr;
    }
    
    std::string formatInfo(const char *verboseStr, const char *briefStr, unsigned long idx, bool verbose)
    {
        std::string info = formatInfo(verboseStr, briefStr, verbose);
        std::ostringstream idxStr;
        
        idxStr << (idx + 1);
        
        for (size_t pos = info.find("#", 0); pos != std::string::npos;  pos = info.find("#", pos + 1))
            info.replace(pos, 1, idxStr.str());
        
        return info;
    }
    
    std::string formatInfo(const char *verboseStr, const char *briefStr, const char *replaceStr, bool verbose)
    {
        std::string info = formatInfo(verboseStr, briefStr, verbose);
        
        for (size_t pos = info.find("#", 0); pos != std::string::npos;  pos = info.find("#", pos + 1))
            info.replace(pos, 1, replaceStr);
        
        return info;
    }
    
private:
    
    // Connection Methods (private)
    
    virtual void connectionUpdate() = 0;
    
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
    
    void addOutputDependency(T *object)
    {
        for (ObjectIterator it = mOutputDependencies.begin(); it != mOutputDependencies.end(); it++)
            if (*it == object)
                return;
        
        mOutputDependencies.push_back(object);
        connectionUpdate();
    }
    
    void removeOutputDependency(T *object)
    {
        for (ObjectIterator it = mOutputDependencies.begin(); it != mOutputDependencies.end(); it++)
        {
            if (*it == object)
            {
                mOutputDependencies.erase(it);
                connectionUpdate();
                return;
            }
        }
    }
    
    // Remove  one connection to this object (before replacement / deletion)
    
    void removeInputDependency(T * object)
    {
        // Check that there is an object connected and that it is not connected to another input /dependency connection also
        
         if (!object)
             return;
        
        for (ConnectionIterator it = mInputConnections.begin(); it != mInputConnections.end(); it++)
            if (it->mObject == object)
                return;
        
        for (ConnectionIterator it = mDependencyConnections.begin(); it != mDependencyConnections.end(); it++)
            if (it->mObject == object)
                return;
        
        // Update dependencies
        
        object->removeOutputDependency(mParent);
    }
    
    // Remove connection and set to defaults
    
    void clearConnection(unsigned long inIdx)
    {
        T *prevObject = mInputConnections[inIdx].mObject;
        mInputConnections[inIdx] = InputConnection();
        removeInputDependency(prevObject);
    }
    
    // Remove dependency connection
    
    ConnectionIterator deleteDependencyConnection(ConnectionIterator it)
    {
        T *object = it->mObject;
        it = mDependencyConnections.erase(it);
        removeInputDependency(object);
        
        return it;
    }
    
    void deleteDependencyConnections()
    {
        for (ConnectionIterator it = mDependencyConnections.begin(); it != mDependencyConnections.end(); it++)
            it = deleteDependencyConnection(it);
    }

    // Remove all connections from a single object
    
    void disconnect(T *object)
    {
        for (ConnectionIterator it = mInputConnections.begin(); it != mInputConnections.end(); it++)
            if (it->mObject == object)
                *it = InputConnection();
        
        for (ConnectionIterator it = mDependencyConnections.begin(); it != mDependencyConnections.end(); it++)
            if (it->mObject == object)
                it = mDependencyConnections.erase(it);
        
        connectionUpdate();
    }
    
    // Detect Potential Feedback in a Network
    
    bool detectFeedback(T *object)
    {
        object->mFeedback = false;
        feedbackProbe();
        return object->mFeedback;
    }
    
    void feedbackProbe()
    {
        mFeedback = true;
        for (typename std::vector <T *>::iterator it = mOutputDependencies.begin(); it != mOutputDependencies.end(); it++)
            (*it)->feedbackProbe();
    }

    // Data
    
    const ObjectType mType;
    FrameLib_Context mContext;

    void *mOwner;
    T *mParent;
    
    // IO Counts
    
    unsigned long mNumIns;
    unsigned long mNumOuts;
    unsigned long mNumAudioChans;
    
    // Connections

    std::vector<InputConnection> mDependencyConnections;
    std::vector<InputConnection> mInputConnections;
    std::vector<T *> mOutputDependencies;
    
    bool mSupportsDependencyConnections;
    bool mFeedback;

};

// FrameLib_Block

// This abstract class provides a connectivity interface to FrameLib_DSP objects or blocks (groups of FrameLib_DSP objects).
// Standard objects inherit this in the FrameLib_DSP class.
// Objects that have asynchronous outputs can use this class to host multiple FrameLib_DSP objects and connect them correctly.

class FrameLib_Block : public FrameLib_Object<FrameLib_Block>
{
    
public:
    
    // Constructor / Destructor
    
    FrameLib_Block(ObjectType type, FrameLib_Context context, void *owner)
    : FrameLib_Object<FrameLib_Block>(type, context, owner, this) {}
    virtual ~FrameLib_Block() {}

    // Connection Queries
    
    virtual class FrameLib_DSP *getInputObject(unsigned long blockIdx) = 0;
    virtual class FrameLib_DSP *getOutputObject(unsigned long blockIdx) = 0;
    virtual unsigned long getInputObjectIdx(unsigned long blockIdx) = 0;
};

#endif
