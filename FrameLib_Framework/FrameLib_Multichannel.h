
#ifndef FRAMELIB_MULTICHANNEL_H
#define FRAMELIB_MULTICHANNEL_H

#include "FrameLib_Context.h"
#include "FrameLib_Block.h"
#include "FrameLib_DSP.h"
#include "FrameLib_ConnectionQueue.h"
#include <algorithm>
#include <vector>

// FrameLib_MultiChannel

// This abstract class allows mulitchannel connnections and the means to update the network according to the number of channels

class FrameLib_MultiChannel : private FrameLib_ConnectionQueue::Item
{
    // Connection and Other Structures
    
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
        MultiChannelInput() : mObject(NULL), mIndex(0) {}
        MultiChannelInput(FrameLib_MultiChannel *object, unsigned long index) : mObject(object), mIndex(index) {}
        
        FrameLib_MultiChannel *mObject;
        unsigned long mIndex;
    };
    
    struct MultiChannelOutput
    {
        std::vector <ConnectionInfo> mConnections;
    };
    
public:
    
    // Constructors

    FrameLib_MultiChannel(FrameLib_Context context, unsigned long nIns, unsigned long nOuts)
    : mContext(context), mQueue(context.getConnectionQueue())
    { setIO(nIns, nOuts); }
    
    FrameLib_MultiChannel(FrameLib_Context context) : mContext(context), mQueue(context.getConnectionQueue()) {}
    
    // Destructor
    
    virtual ~FrameLib_MultiChannel()
    {
        mContext.releaseConnectionQueue();
        mQueue = NULL;
        clearConnections();
    }
    
    // Basic Setup / IO Queries (override the audio methods if handling audio)

    virtual void setSamplingRate(double samplingRate) {};

    unsigned long getNumIns()                   { return mInputs.size(); }
    unsigned long getNumOuts()                  { return mOutputs.size(); }
    virtual unsigned long getNumAudioIns()      { return 0; }
    virtual unsigned long getNumAudioOuts()     { return 0; }

    // Set Fixed Inputs
    
    virtual void setFixedInput(unsigned long idx, double *input, unsigned long size) {};

    // Audio Processing
    
    virtual void blockUpdate(double **ins, double **outs, unsigned long vecSize) {}
    virtual void reset() {}

    static bool handlesAudio() { return false; }

    // Connection Methods
    
    // N.B. - No sanity checks here to maximise speed and help debugging (better for it to crash if a mistake is made)
    
    void deleteConnection(unsigned long inIdx);
    void addConnection(FrameLib_MultiChannel *object, unsigned long outIdx, unsigned long inIdx);
    void clearConnections();
    bool isConnected(unsigned long inIdx);
    
protected:
    
    // IO Utilities
    
    // Call this in derived class constructors if the IO size is not static
    
    void setIO(unsigned long nIns, unsigned long nOuts)
    {
        mInputs.resize(nIns);
        mOutputs.resize(nOuts);
    }
    
    unsigned long getInputNumChans(unsigned long inIdx);
    ConnectionInfo getInputChan(unsigned long inIdx, unsigned long chan) { return mInputs[inIdx].mObject->mOutputs[mInputs[inIdx].mIndex].mConnections[chan]; }

private:

    // Dependency updating

    void addOutputDependency(FrameLib_MultiChannel *object);
    std::vector <FrameLib_MultiChannel *>::iterator removeOutputDependency(FrameLib_MultiChannel *object);

    // Connection methods (private)
    
    void updateConnections() { if (mQueue) mQueue->add(this); }
    
    void clearConnection(unsigned long inIdx);
    void removeConnection(unsigned long inIdx);
    std::vector <FrameLib_MultiChannel *>::iterator removeConnections(FrameLib_MultiChannel *object);
    
    virtual void outputUpdate();

protected:

    // Member variables

    // Context
    
    FrameLib_Context mContext;
    
    // Outputs
    
    std::vector <MultiChannelOutput> mOutputs;

private:
    
    // Queue
    
    FrameLib_ConnectionQueue *mQueue;
    
    // Connection Info
    
    std::vector <FrameLib_MultiChannel *> mOutputDependencies;
    std::vector <MultiChannelInput> mInputs;
};


// ************************************************************************************** //

// FrameLib_Pack - Pack single multichannel signals

class FrameLib_Pack : public FrameLib_MultiChannel
{
    enum AtrributeList {kInputs};

public:
    
    FrameLib_Pack(FrameLib_Context context, FrameLib_Attributes::Serial *serialAttributes, void *owner);
    
private:
    
    virtual bool inputUpdate();
    
    FrameLib_Attributes mAttributes;
};

// ************************************************************************************** //

// FrameLib_Unpack - Unpack multichannel signals

class FrameLib_Unpack : public FrameLib_MultiChannel
{
    enum AtrributeList {kOutputs};
    
public:

    FrameLib_Unpack(FrameLib_Context context, FrameLib_Attributes::Serial *serialAttributes, void *owner);
    
private:
    
    virtual bool inputUpdate();
        
    FrameLib_Attributes mAttributes;
};

// ************************************************************************************** //

// FrameLib_Expand - MultiChannel expansion for FrameLib_Block objects

template <class T> class FrameLib_Expand : public FrameLib_MultiChannel
{

public:
    
    FrameLib_Expand(FrameLib_Context context, FrameLib_Attributes::Serial *serialisedAttributes, void *owner)
    : FrameLib_MultiChannel(context), mAllocator(context.getAllocator()), mSerialisedAttributes(serialisedAttributes->size()), mOwner(owner)
    {
        // Make first block
        
        mBlocks.push_back(new T(context, serialisedAttributes, owner));
        
        // Copy serialised attributes for later instantiations

        mSerialisedAttributes.write(serialisedAttributes);
        
        // Set up IO / Fixed Inputs / Audio Temps
        
        setIO(mBlocks[0]->getNumIns(), mBlocks[0]->getNumOuts());
        mFixedInputs.resize(getNumIns());
        mAudioTemps.resize(getNumAudioOuts());
        
        // Make initial output connections
        
        for (unsigned long i = 0; i < getNumOuts(); i++)
            mOutputs[i].mConnections.push_back(ConnectionInfo(mBlocks[0], i));
        
        setSamplingRate(0.0);
    }
    
    ~FrameLib_Expand()
    {
        // Clear connections before deleting internal objects
        
        clearConnections();
        
        // Delete blocks
        
        for (std::vector <FrameLib_Block *> :: iterator it = mBlocks.begin(); it != mBlocks.end(); it++)
            delete (*it);
        
        mContext.releaseAllocator();
    }
    
    // Sampling Rate

    virtual void setSamplingRate(double samplingRate)
    {
        mSamplingRate = samplingRate;
        
        for (std::vector <FrameLib_Block *> :: iterator it = mBlocks.begin(); it != mBlocks.end(); it++)
            (*it)->setSamplingRate(samplingRate);
    }
    
    // Audio IO Queries
    
    virtual unsigned long getNumAudioIns()  { return mBlocks[0]->getNumAudioIns(); }
    virtual unsigned long getNumAudioOuts() { return mBlocks[0]->getNumAudioOuts(); }
    
    // Fixed Inputs
    
    virtual void setFixedInput(unsigned long idx, double *input, unsigned long size)
    {
        if (idx < mFixedInputs.size())
        {
            mFixedInputs[idx].assign(input, input + size);
            updateFixedInput(idx);
        }
    }
    
    // Audio Processing
    
    virtual void blockUpdate(double **ins, double **outs, unsigned long vecSize)
    {
        // Allocate Temps
        
        if (getNumAudioOuts())
            mAudioTemps[0] = (double *) mAllocator->alloc(sizeof(double) * vecSize * getNumAudioOuts());
        for (unsigned long i = 1; i < getNumAudioOuts(); i++)
            mAudioTemps[i] = mAudioTemps[0] + (i * vecSize);
            
        // Zero Outputs
        
        for (unsigned long i = 0; i < getNumAudioOuts(); i++)
            std::fill(outs[i], outs[i] + vecSize, 0.0);

        // Process and Sum to Outputs

        for (std::vector <FrameLib_Block *> :: iterator it = mBlocks.begin(); it != mBlocks.end(); it++)
        {
            (*it)->blockUpdate(ins, &mAudioTemps[0], vecSize);
            
            for (unsigned long i = 0; i < getNumAudioOuts(); i++)
                for (unsigned long j = 0; j < vecSize; j++)
                    outs[i][j] += mAudioTemps[i][j];
        }

        // Release Temps and Clear Allocator
        
        if (getNumAudioOuts())
            mAllocator->dealloc(mAudioTemps[0]);
                
        mAllocator->clear();
    }
   
    // Reset
    
    void reset()
    {
        for (std::vector <FrameLib_Block *> :: iterator it = mBlocks.begin(); it != mBlocks.end(); it++)
            (*it)->reset();
    }
    
    // Handles Audio
    
    static bool handlesAudio() { return T::handlesAudio(); }
    
private:

    // Update Fixed Inputs
    
    void updateFixedInput(unsigned long idx)
    {
        for (unsigned long i = 0; i < mBlocks.size(); i++)
            mBlocks[i]->setFixedInput(idx, &mFixedInputs[idx][0], mFixedInputs[idx].size());
    }
    
    // Update (expand)
    
    virtual bool inputUpdate()
    {
        // Find number of channels
        
        unsigned long nChannels = 0;
        unsigned long cChannels = mBlocks.size();
        
        for (unsigned long i = 0; i < getNumIns(); i++)
            if (getInputNumChans(i) > nChannels)
                nChannels = getInputNumChans(i);
        
        // Always maintain at least one object
        
        nChannels = nChannels ? nChannels : 1;
        
        // Resize if necessary
        
        bool numChansChanged = nChannels != cChannels;
        
        if (numChansChanged)
        {
            if (nChannels > cChannels)
            {
                mBlocks.resize(nChannels);
                
                for (unsigned long i = cChannels; i < nChannels; i++)
                {
                    mBlocks[i] = new T(mContext, &mSerialisedAttributes, mOwner);
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
            
            // Update Fixed Inputs
            
            for (unsigned long i = 0; i < getNumIns(); i++)
                updateFixedInput(i);
        }
        
        // Make input connections

        for (unsigned long i = 0; i < getNumIns(); i++)
        {
            if (getInputNumChans(i))
            {
                for (unsigned long j = 0; j < nChannels; j++)
                {
                    ConnectionInfo connection = getInputChan(i, j % getInputNumChans(i));
                    mBlocks[j]->addConnection(connection.mObject, connection.mIndex, i);
                }
            }
            else
            {
                for (unsigned long j = 0; j < nChannels; j++)
                    mBlocks[j]->deleteConnection(i);
            }
        }
        
        return numChansChanged;
    }

    FrameLib_LocalAllocator *mAllocator;
    FrameLib_Attributes::Serial mSerialisedAttributes;

    std::vector <FrameLib_Block *> mBlocks;
    std::vector <std::vector <double> > mFixedInputs;
    
    void *mOwner;
    
    double mSamplingRate;
    
    std::vector<double *> mAudioTemps;
};

#endif
