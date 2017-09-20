
#ifndef FRAMELIB_CONNECTABLE_H
#define FRAMELIB_CONNECTABLE_H

#include "FrameLib_Types.h"
#include "FrameLib_Object.h"

template <class T>
class FrameLib_Connectable : public FrameLib_Block
{
    
public:
    
    struct Input
    {
        Input() : mObject(NULL), mIndex(0) {}
        Input(T *object, unsigned long index) : mObject(object), mIndex(index) {}
        
        T *mObject;
        unsigned long mIndex;
    };
    
    FrameLib_Connectable(ObjectType type, FrameLib_Context context) : FrameLib_Block(type, context) {}
    
    ConnectionResult addConnection(T *object, unsigned long outIdx, unsigned long inIdx)
    {
        if (object == this)
            return kConnectSelfConnection;
        
        if (object->getContext() != getContext())
            return kConnectWrongContext;
        
        if (detectFeedback(object))
            return kConnectFeedbackDetected;
        
        // Update dependencies if the connection is now from a different object
        
        if (mInputs[inIdx].mObject != object)
        {
            removeConnection(inIdx);
            addInputDependency(object);
            object->addOutputDependency(this);
        }
        
        // Store data about connection and reset the dependency count
        
        mInputs[inIdx].setInput(object, outIdx);
        connectionUpdate();
        
        return kConnectSuccess;
    }

    void deleteConnection(unsigned long inIdx)
    {
        clearConnection(inIdx);
        connectionUpdate();
    }

    void clearConnections()
    {
        // Remove input connections
        
        for (unsigned long i = 0; i < mInputs.size(); i++)
            clearConnection(i);
        
        // Remove output connections
        
        for (typename std::vector< T *>::iterator it = mOutputDependencies.begin(); it != mOutputDependencies.end(); )
            it = (*it)->disconnect(this);
        
        connectionUpdate();
    }

    bool isConnected(unsigned long inIdx)
    {
        return mInputs[inIdx].mObject != NULL;
    }

    virtual void connectionUpdate() = 0;

    // Connection Methods (private)

    // Dependency Updating

    void addInputDependency(FrameLib_DSP *object)
    {
        for (typename std::vector <T *>::iterator it = mInputDependencies.begin(); it != mInputDependencies.end(); it++)
            if (*it == object)
                return;
        
        mInputDependencies.push_back(object);
    }
    
    void addOutputDependency(FrameLib_DSP *object)
    {
        for (typename std::vector<T *>::iterator it = mOutputDependencies.begin(); it != mOutputDependencies.end(); it++)
            if (*it == object)
                return;
        
        mOutputDependencies.push_back(object);
    }

    std::vector <FrameLib_DSP *>::iterator removeInputDependency(FrameLib_DSP *object)
    {
        typename std::vector <T *>::iterator it;
        
        for (it = mInputDependencies.begin(); it != mInputDependencies.end(); it++)
            if (*it == object)
                return mInputDependencies.erase(it);
        
        return it;
    }

    typename std::vector <T *>::iterator removeOutputDependency(T *object)
    {
        typename std::vector <T *>::iterator it;
        
        for (it = mOutputDependencies.begin(); it != mOutputDependencies.end(); it++)
            if (*it == object)
                return mOutputDependencies.erase(it);
        
        return it;
    }

    // Remove connection and set to defaults

    void clearConnection(unsigned long inIdx)
    {
        removeConnection(inIdx);
        mInputs[inIdx].setInput();
    }

    // Removal of one connection to this object (before replacement / deletion)

    void removeConnection(unsigned long inIdx)
    {
        // Check that there is an object connected and that it is not connected to another input also
        
        if (!mInputs[inIdx].mObject)
            return;
        
        for (unsigned long i = 0; i < mInputs.size(); i++)
            if (mInputs[i].mObject == mInputs[inIdx].mObject && i != inIdx)
                return;
        
        // Update dependencies
        
        removeInputDependency(mInputs[inIdx].mObject);
        mInputs[inIdx].mObject->removeOutputDependency(this);
    }

    // Removal of all connections from one object to this object

    typename std::vector<T *>::iterator disconnect(T *object)
    {
        // Set any inputs connected to the object to default values
        
        for (unsigned long i = 0; i < mInputs.size(); i++)
            if (mInputs[i].mObject == object)
                mInputs[i].setInput();
        
        // Update dependencies
        
        removeInputDependency(object);
        return object->removeOutputDependency(this);
    }

    // Detect Potential Feedback in a Network

    bool detectFeedback(T *object)
    {
        object->setFeedback(false);
        feedbackProbe();
        return object->getFeedback();
    }

    void feedbackProbe()
    {
        setFeedback(true);
        for (typename std::vector <T *>::iterator it = mOutputDependencies.begin(); it != mOutputDependencies.end(); it++)
            (*it)->feedbackProbe();
    }
    
protected:

    std::vector<Input> mInputs;
    
    std::vector<T *> mInputDependencies;
    std::vector<T *> mOutputDependencies;
};

#endif
