
#ifndef FRAMELIB_MULTICHANNEL_H
#define FRAMELIB_MULTICHANNEL_H

#include "FrameLib_Block.h"
#include "FrameLib_DSP.h"
#include <vector>

// FrameLib_MultiChannel

// This abstract class allows mulitchannel connnections and the means to update the network according to the number of channels

// FIX - improve audio handling temp memory code (could use realtime allocator?) - and memory alignment!

class FrameLib_MultiChannel
{
    // ************************************************************************************** //

    // Connection Queue - this allows the network to update itself without deep recursive stack calls
    
public:
    
    class ConnectionQueue
    {
    
    public:
        
        ConnectionQueue() : mTop(NULL), mTail(NULL) {}
        
        void add(FrameLib_MultiChannel *object)
        {
            object->mNext = NULL;
            
            if (!mTop)
            {
                // Queue is empty - add and start processing the queue

                mTop = mTail = object;

                while (mTop)
                {                    
                    object = mTop;
                    object->inputUpdate();
                    mTop = object->mNext;
                    object->mNext = NULL;
                }
                
                mTail = NULL;
            }
            else
            {
                // Add to the queue (which is already processing)

                mTail->mNext = object;
                mTail = object;
            }
        }
        
    private:

        FrameLib_MultiChannel *mTop;
        FrameLib_MultiChannel *mTail;
    };

    // ************************************************************************************** //

    // Connection Structures
    
protected:

    struct ConnectionInfo
    {
        ConnectionInfo(FrameLib_Block *object = NULL, unsigned long idx = 0) : mObject(object), mIndex(idx) {}
        
        FrameLib_Block *mObject;
        unsigned long mIndex;
    };

private:
    
    struct MultiChannelInput
    {
        FrameLib_MultiChannel *mObject;
        unsigned long mIndex;
    };
    
    struct MultiChannelOutput
    {
        std::vector <ConnectionInfo> mConnections;
    };
    
    // ************************************************************************************** //

    // Constructors
    
public:
    
    FrameLib_MultiChannel (ConnectionQueue *queue, unsigned long nIns, unsigned long nOuts) : mQueue(queue)
    {
        setIO(nIns, nOuts);
    }
    
    FrameLib_MultiChannel (ConnectionQueue *queue) : mQueue(queue)
    {
    }
    
    // Destructor (virtual) - this MUST call clearConnections() in the inheriting class
    
    virtual ~FrameLib_MultiChannel() = 0;

    // ************************************************************************************** //

    // IO Utilities
    
protected:
    
    // Call this in derived class constructors only if the IO size is not fixed
    
    void setIO(unsigned long nIns, unsigned long nOuts)
    {
        mInputs.resize(nIns);
        mOutputs.resize(nOuts);
    }
    
public:
    
    virtual void setSamplingRate(double samplingRate) = 0;
    
    unsigned long getNumIns()
    {
        return mInputs.size();
    }
    
    unsigned long getNumOuts()
    {
        return mOutputs.size();
    }
    
    virtual unsigned long getNumAudioIns()
    {
        // Override this if you are handling audio
    
        return 0;
    }
    
    virtual unsigned long getNumAudioOuts()
    {
        // Override this if you are handling audio
        
        return 0;
    }

    // ************************************************************************************** //

    // Audio processing
    
    virtual void blockProcess (double **ins, double **outs, unsigned long vecSize)
    {
        // Override to handle audio at the block level (objects with block-based audio must overload this)
    }
    
    virtual bool handlesAudio()
    {
        // This function allows you to tell the outside world (whatever that is) whether or not it should call the audio brocessing block
        
        return FALSE;
    }
    
    virtual void reset()
    {
        // Override this if your object contains FrameLib_Block objects and reset them here
    }
    
    // ************************************************************************************** //

    // Multichannel update code
    
private:
    
    // Override to deal with changes in the input connections
    
    virtual void inputUpdate() = 0;
    
protected:
    
    void outputUpdate()
    {
        // Add to the queue to update all output dependencies
        
        for (std::vector <FrameLib_MultiChannel *>::iterator it = mOutputDependencies.begin(); it != mOutputDependencies.end(); it++)
            mQueue->add(*it);
    }
    
    unsigned long getNumChans(unsigned long inIdx)
    {
        if (mInputs[inIdx].mObject)
            return mInputs[inIdx].mObject->mOutputs[mInputs[inIdx].mIndex].mConnections.size();
        
        return 0;
    }
    
    ConnectionInfo getChan(unsigned long inIdx, unsigned long chan)
    {
        return mInputs[inIdx].mObject->mOutputs[mInputs[inIdx].mIndex].mConnections[chan];
    }
    

    // ************************************************************************************** //

    // Connection methods (private)

private:

    // Dependency updating

    std::vector <FrameLib_MultiChannel *>::iterator removeOutputDependency(FrameLib_MultiChannel *object)
    {
        std::vector <FrameLib_MultiChannel *>::iterator it;
        
        for (it = mOutputDependencies.begin(); it != mOutputDependencies.end(); it++)
            if (*it == object)
                return mOutputDependencies.erase(it);

        return it;
    }
    
    void addOutputDependency(FrameLib_MultiChannel *object)
    {
        for (std::vector <FrameLib_MultiChannel *>::iterator it = mOutputDependencies.begin(); it != mOutputDependencies.end(); it++)
            if (*it == object)
                return;
        
        mOutputDependencies.push_back(object);
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
        
        mInputs[inIdx].mObject->removeOutputDependency(this);
    }

    // Removal of all connections from one object to this object
    
    std::vector <FrameLib_MultiChannel *>::iterator removeConnections(FrameLib_MultiChannel *object)
    {
        // Set any inputs connected to the object to default values
        
        for (unsigned long i = 0; i < mInputs.size(); i++)
        {
            if (mInputs[i].mObject == object)
            {
                mInputs[i].mObject = NULL;
                mInputs[i].mIndex = 0;
            }
        }
        
        // Update dependencies
        
        std::vector <FrameLib_MultiChannel *>::iterator updatedIterator = object->removeOutputDependency(this);
        mQueue->add(this);
        
        return updatedIterator;
    }

    // ************************************************************************************** //
    
    // Connection methods (public)
    
public:
    
    // N.B. - No sanity checks here to maximise speed and help debugging (better for it to crash if a mistake is made)
    
    void deleteConnection(unsigned long inIdx)
    {
        // Update Dependencies
        
        removeConnection(inIdx);
        
        // Set default values
        
        mInputs[inIdx].mObject = NULL;
        mInputs[inIdx].mIndex = 0;
        
        // Update
        
        mQueue->add(this);
    }
    
    void addConnection(FrameLib_MultiChannel *object, unsigned long outIdx, unsigned long inIdx)
    {
        // Update dependencies if the connected object has changed
        
        if (mInputs[inIdx].mObject != object)
        {
            removeConnection(inIdx);
            object->addOutputDependency(this);
        }
                          
        // Store data about connection
        
        mInputs[inIdx].mObject = object;
        mInputs[inIdx].mIndex = outIdx;
        
        // Update
        
        mQueue->add(this);
    }
    
    void clearConnections()
    {
        // Remove input connections
        
        for (unsigned long i = 0; i < mInputs.size(); i++)
        {
            removeConnection(i);
            mInputs[i].mObject = NULL;
            mInputs[i].mIndex = 0;
        }
        
        // Update
        
        mQueue->add(this);

        // Remove output connections
        
        for (std::vector <FrameLib_MultiChannel *>::iterator it = mOutputDependencies.begin(); it != mOutputDependencies.end(); )
            it = (*it)->removeConnections(this);
    }
    
    bool isConnected(unsigned long inIdx)
    {
        return mInputs[inIdx].mObject != NULL;
    }

    // ************************************************************************************** //

    // Member variables
    
private:
    
    // Queue
    
    ConnectionQueue *mQueue;
    FrameLib_MultiChannel *mNext;
    
    // Connection Info
    
    std::vector <FrameLib_MultiChannel *> mOutputDependencies;
    std::vector <MultiChannelInput> mInputs;

protected:
    
    std::vector <MultiChannelOutput> mOutputs;
    
};

// Virtual Destructor

FrameLib_MultiChannel::~FrameLib_MultiChannel()
{
}

// ************************************************************************************** //

// FrameLib_Pack - Pack single multichannel signals

class FrameLib_Pack : public FrameLib_MultiChannel
{
    enum AtrributeList {kInputs};

public:
    
    FrameLib_Pack(ConnectionQueue *queue, FrameLib_DSP::DSPQueue *dspQueue, FrameLib_Attributes::Serial *serialAttributes) : FrameLib_MultiChannel(queue)
    {
        mAttributes.addDouble(0, "inputs", 2, 0 );
        mAttributes.set(serialAttributes);
        setIO(mAttributes.getValue(kInputs), 1);
    }
    
    ~FrameLib_Pack()
    {
        // Clear connections before deletion
        
        clearConnections();
    }
    
    void setSamplingRate(double samplingRate)
    {
    }
    
private:
    
    // Update (pack)
    
    void inputUpdate()
    {
        mOutputs[0].mConnections.clear();
        
        for (unsigned long i = 0; i < getNumIns(); i++)
            for (unsigned long j = 0; j < getNumChans(i); j++)
                mOutputs[0].mConnections.push_back(getChan(i, j));
        
        outputUpdate();
    }
    
    FrameLib_Attributes mAttributes;
};

// ************************************************************************************** //

// FrameLib_Unpack - Unpack multichannel signals

class FrameLib_Unpack : public FrameLib_MultiChannel
{
    enum AtrributeList {kOutputs};
    
public:

    FrameLib_Unpack(ConnectionQueue *queue, FrameLib_DSP::DSPQueue *dspQueue, FrameLib_Attributes::Serial *serialAttributes) : FrameLib_MultiChannel(queue)
    {
        mAttributes.addDouble(kOutputs, "outputs", 2, 0);
        mAttributes.set(serialAttributes);
        setIO(1, mAttributes.getValue(kOutputs));
    }
   
    ~FrameLib_Unpack()
    {
        // Clear connections before deletion
        
        clearConnections();
    }
    
    void setSamplingRate(double samplingRate)
    {
    }
        
private:

    // Update (unpack)
    
    void inputUpdate()
    {
        for (unsigned long i = 0; i < getNumOuts(); i++)
            mOutputs[i].mConnections.clear();
        
        for (unsigned long i = 0; i < getNumChans(0) && i < getNumOuts(); i++)
            mOutputs[i].mConnections.push_back(getChan(0, i));
        
        outputUpdate();
    }
    
    FrameLib_Attributes mAttributes;
};

// ************************************************************************************** //

// FrameLib_Expand - MultiChannel expansion for FrameLib_Block objects

template <class T> class FrameLib_Expand : public FrameLib_MultiChannel
{

public:
    
    FrameLib_Expand(ConnectionQueue *connectQueue, FrameLib_DSP::DSPQueue *dspQueue, FrameLib_Attributes::Serial *serialisedAttributes)
    : FrameLib_MultiChannel(connectQueue), mDSPQueue(dspQueue), mAllocator(dspQueue->getAllocator())
    {
        // Make first block
        
        mBlocks.push_back(new T(dspQueue, serialisedAttributes));
        
        // Copy serialised attributes for later instantiations
        
        mSerialisedAttributes = new FrameLib_Attributes::Serial(serialisedAttributes->size());
        mSerialisedAttributes->write(serialisedAttributes);
        
        // Set up IO
        
        setIO(mBlocks[0]->getNumIns(), mBlocks[0]->getNumOuts());
        
        // Make initial output connections
        
        for (unsigned long i = 0; i < getNumOuts(); i++)
            mOutputs[i].mConnections.push_back(ConnectionInfo(mBlocks[0], i));
        
        setSamplingRate(0.0);
    }
    
    ~FrameLib_Expand()
    {
        // Clear connections before deletion
        
        clearConnections();
        
        // Delete blocks
        
        for (std::vector <FrameLib_Block *> :: iterator it = mBlocks.begin(); it != mBlocks.end(); it++)
            delete (*it);
        
        delete mSerialisedAttributes;
    }
    
    // Sampling Rate

    void setSamplingRate(double samplingRate)
    {
        mSamplingRate = samplingRate;
        
        for (std::vector <FrameLib_Block *> :: iterator it = mBlocks.begin(); it != mBlocks.end(); it++)
            (*it)->setSamplingRate(samplingRate);
    }
    
    // Reset
    
    void reset()
    {
        for (std::vector <FrameLib_Block *> :: iterator it = mBlocks.begin(); it != mBlocks.end(); it++)
            (*it)->reset();
    }
    
    // Handles Audio
    
    bool handlesAudio()
    {
        return mBlocks[0]->handlesAudio();
    }
    
    // IO
    
    virtual unsigned long getNumAudioIns()
    {
        return mBlocks[0]->getNumAudioIns();
    }
    
    virtual unsigned long getNumAudioOuts()
    {
        return mBlocks[0]->getNumAudioOuts();
    }
    
    // ************************************************************************************** //
    
    // Audio Processing
    
    virtual void blockProcess (double **ins, double **outs, unsigned long vecSize)
    {
        double *temps[getNumAudioOuts()];
        
        for (unsigned long i = 0; i < getNumAudioOuts(); i++)
        {
            if (i == 0)
                temps[0] = (double *) mAllocator->alloc(sizeof(double) * vecSize * getNumAudioOuts());
            else
                temps[i] = temps[0] + (i * vecSize);

            for (unsigned long j = 0; j < vecSize; j++)
                outs[i][j] = 0.0;
        }
        
        for (std::vector <FrameLib_Block *> :: iterator it = mBlocks.begin(); it != mBlocks.end(); it++)
        {
            // Process
            
            (*it)->blockUpdate(ins, temps, vecSize);

            // Sum to outputs
            
            for (unsigned long i = 0; i < getNumAudioOuts(); i++)
                for (unsigned long j = 0; j < vecSize; j++)
                    outs[i][j] += temps[i][j];
        }

        if (getNumAudioOuts())
            mAllocator->dealloc(temps[0]);
        
        mAllocator->clearLocal();
    }
    
    // ************************************************************************************** //

private:

    // Update (expand)
    
    void inputUpdate()
    {        
        // Find number of channels
        
        unsigned long nChannels = 0;
        unsigned long cChannels = mBlocks.size();
        
        for (unsigned long i = 0; i < getNumIns(); i++)
            if (getNumChans(i) > nChannels)
                nChannels = getNumChans(i);
        
        // Always maintain at least one object
        
        nChannels = nChannels ? nChannels : 1;
        
        // Resize if necessary
        
        if (nChannels != cChannels)
        {
            if (nChannels > cChannels)
            {
                mBlocks.resize(nChannels);
                
                for (unsigned long i = cChannels; i < nChannels; i++)
                {
                    mBlocks[i] = new T (mDSPQueue, mSerialisedAttributes);
                    mBlocks[i]->setSamplingRate(mSamplingRate);
                }
            }
            else
            {
                for (unsigned long i = nChannels; i < cChannels; i++)
                    delete mBlocks[i];
                
                mBlocks.resize(nChannels);
            }
            
            // Redo output connection lists
            
            for (unsigned long i = 0; i < getNumOuts(); i++)
                mOutputs[i].mConnections.clear();
            
            for (unsigned long i = 0; i < getNumOuts(); i++)
                for (unsigned long j = 0; j < nChannels; j++)
                    mOutputs[i].mConnections.push_back(ConnectionInfo(mBlocks[j], i));
            
            // Update output dependencies
            
            outputUpdate();
        }
        
        // Make input connections

        for (unsigned long i = 0; i < getNumIns(); i++)
        {
            if (getNumChans(i))
            {
                for (unsigned long j = 0; j < nChannels; j++)
                {
                    ConnectionInfo connection = getChan(i, j % getNumChans(i));
                    mBlocks[j]->addConnection(connection.mObject, connection.mIndex, i);
                }
            }
            else
            {
                for (unsigned long j = 0; j < nChannels; j++)
                    mBlocks[j]->deleteConnection(i);
            }
        }
    }

    FrameLib_DSP::DSPQueue *mDSPQueue;
    FrameLib_Local_Allocator *mAllocator;
    FrameLib_Attributes::Serial *mSerialisedAttributes;

    std::vector <FrameLib_Block *> mBlocks;
    
    double mSamplingRate;
};

#endif
