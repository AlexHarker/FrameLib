
#ifndef FRAMELIB_MULTICHANNEL_H
#define FRAMELIB_MULTICHANNEL_H

#include "FrameLib_Context.h"
#include "FrameLib_Object.h"
#include "FrameLib_DSP.h"
#include "FrameLib_ConnectionQueue.h"
#include "FrameLib_Info.h"
#include <algorithm>
#include <vector>

// FrameLib_MultiChannel

// This abstract class allows mulitchannel connnections and the means to update the network according to the number of channels

class FrameLib_MultiChannel : public FrameLib_Object<FrameLib_MultiChannel>, private FrameLib_ConnectionQueue::Item
{
    
protected:

    // Connection Info Structure

    struct ConnectionInfo
    {
        ConnectionInfo(FrameLib_Block *object = NULL, unsigned long idx = 0) : mObject(object), mIndex(idx) {}
        
        FrameLib_Block *mObject;
        unsigned long mIndex;
    };

private:
    
    // IO Structures

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

    FrameLib_MultiChannel(ObjectType type, FrameLib_Context context, unsigned long nIns, unsigned long nOuts)
    : FrameLib_Object(type), mQueue(context)
    { setIO(nIns, nOuts); }
    
    FrameLib_MultiChannel(ObjectType type, FrameLib_Context context) : FrameLib_Object(type), mQueue(context) {}
    
    // Destructor
    
    virtual ~FrameLib_MultiChannel()
    {
        mQueue.release();
        clearConnections();
    }
    
    // Set Fixed Inputs
    
    virtual void setFixedInput(unsigned long idx, double *input, unsigned long size) {};

    // Audio Processing
    
    virtual void blockUpdate(double **ins, double **outs, unsigned long vecSize) {}
    virtual void reset(double samplingRate) {}

    static bool handlesAudio() { return false; }

    // Connection Methods
    
    // N.B. - No sanity checks here to maximise speed and help debugging (better for it to crash if a mistake is made)
    
    virtual void deleteConnection(unsigned long inIdx);
    virtual void addConnection(FrameLib_MultiChannel *object, unsigned long outIdx, unsigned long inIdx);
    virtual void clearConnections();
    virtual bool isConnected(unsigned long inIdx);
    
protected:
    
    // IO Utilities
    
    // Call this in derived class constructors if the IO size is not static
    
    void setIO(unsigned long nIns, unsigned long nOuts, unsigned long nAudioChans = 0)
    {
        FrameLib_Object::setIO(nIns, nOuts, nAudioChans);
        mInputs.resize(getNumIns());
        mOutputs.resize(getNumOuts());
    }
    
    // Query Input Channels
    
    unsigned long getInputNumChans(unsigned long inIdx);
    ConnectionInfo getInputChan(unsigned long inIdx, unsigned long chan) { return mInputs[inIdx].mObject->mOutputs[mInputs[inIdx].mIndex].mConnections[chan]; }

private:

    // Deleted
    
    FrameLib_MultiChannel(const FrameLib_MultiChannel&);
    FrameLib_MultiChannel& operator=(const FrameLib_MultiChannel&);
    
    // Dependency Updating

    void addOutputDependency(FrameLib_MultiChannel *object);
    std::vector <FrameLib_MultiChannel *>::iterator removeOutputDependency(FrameLib_MultiChannel *object);

    // Connection Methods (private)
    
    void updateConnections() { if (mQueue) mQueue->add(this); }
    
    void clearConnection(unsigned long inIdx);
    void removeConnection(unsigned long inIdx);
    std::vector <FrameLib_MultiChannel *>::iterator disconnect(FrameLib_MultiChannel *object);
    
    virtual void outputUpdate();

protected:

    // Member Variables

    // Outputs
    
    std::vector <MultiChannelOutput> mOutputs;

private:
    
    // Queue
    
    FrameLib_Context::ConnectionQueue mQueue;
    
    // Connection Info
    
    std::vector <FrameLib_MultiChannel *> mOutputDependencies;
    std::vector <MultiChannelInput> mInputs;
};

// ************************************************************************************** //

// FrameLib_Pack - Pack Multichannel Signals

class FrameLib_Pack : public FrameLib_MultiChannel, private FrameLib_Info
{
    enum AtrributeList { kInputs };

    struct ParameterInfo : public FrameLib_Parameters::Info { ParameterInfo() { add("Sets the number of inputs."); } };
    
public:
    
    FrameLib_Pack(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner);
    
    // Info
    
    virtual const char *objectInfo(bool verbose);
    virtual const char *inputInfo(unsigned long idx, bool verbose);
    virtual const char *outputInfo(unsigned long idx, bool verbose);
    
    virtual const FrameLib_Parameters *getParameters() { return &mParameters; }

    virtual FrameType inputType(unsigned long idx) { return kFrameAny; }
    virtual FrameType outputType(unsigned long idx) { return kFrameAny; }
    
private:
    
    virtual bool inputUpdate();

    FrameLib_Parameters mParameters;
    
    static ParameterInfo sParamInfo;
};

// ************************************************************************************** //

// FrameLib_Unpack - Unpack Multichannel Signals

class FrameLib_Unpack : public FrameLib_MultiChannel, private FrameLib_Info
{
    enum AtrributeList { kOutputs };
    
    struct ParameterInfo : public FrameLib_Parameters::Info { ParameterInfo() { add("Sets the number of outputs."); } };

public:

    FrameLib_Unpack(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner);
    
    // Info
    
    virtual const char *objectInfo(bool verbose);
    virtual const char *inputInfo(unsigned long idx, bool verbose);
    virtual const char *outputInfo(unsigned long idx, bool verbose);

    virtual const FrameLib_Parameters *getParameters() { return &mParameters; }

    virtual FrameType inputType(unsigned long idx) { return kFrameAny; }
    virtual FrameType outputType(unsigned long idx) { return kFrameAny; }
    
private:
    
    virtual bool inputUpdate();
        
    FrameLib_Parameters mParameters;
    
    static ParameterInfo sParamInfo;
};

// ************************************************************************************** //

// FrameLib_Expand - MultiChannel expansion for FrameLib_Block objects

template <class T> class FrameLib_Expand : public FrameLib_MultiChannel
{

public:
    
    FrameLib_Expand(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner)
    : FrameLib_MultiChannel(T::getType(), context), mContext(context), mAllocator(context), mSerialisedParameters(serialisedParameters->size()), mOwner(owner)
    {
        // Make first block
        
        mBlocks.push_back(new T(context, serialisedParameters, owner));
        
        // Copy serialised parameters for later instantiations

        mSerialisedParameters.write(serialisedParameters);
        
        // Set up IO / fixed inputs / audio temps
        
        setIO(mBlocks[0]->getNumIns(), mBlocks[0]->getNumOuts(), mBlocks[0]->getNumAudioChans());
        mFixedInputs.resize(getNumIns());
        mAudioTemps.resize(getNumAudioOuts());
        
        // Make initial output connections
        
        for (unsigned long i = 0; i < getNumOuts(); i++)
            mOutputs[i].mConnections.push_back(ConnectionInfo(mBlocks[0], i));
        
        reset(0.0);
    }
    
    ~FrameLib_Expand()
    {
        // Clear connections before deleting internal objects
        
        clearConnections();
        
        // Delete blocks
        
        for (std::vector <FrameLib_Block *> :: iterator it = mBlocks.begin(); it != mBlocks.end(); it++)
            delete(*it);
    }
    
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
        // Allocate temporary memory
        
        if (getNumAudioOuts())
            mAudioTemps[0] = (double *) mAllocator->alloc(sizeof(double) * vecSize * getNumAudioOuts());
        for (unsigned long i = 1; i < getNumAudioOuts(); i++)
            mAudioTemps[i] = mAudioTemps[0] + (i * vecSize);
            
        // Zero outputs
        
        for (unsigned long i = 0; i < getNumAudioOuts(); i++)
            std::fill(outs[i], outs[i] + vecSize, 0.0);

        // Process and sum to outputs

        for (std::vector <FrameLib_Block *> :: iterator it = mBlocks.begin(); it != mBlocks.end(); it++)
        {
            (*it)->blockUpdate(ins, &mAudioTemps[0], vecSize);
            
            for (unsigned long i = 0; i < getNumAudioOuts(); i++)
                for (unsigned long j = 0; j < vecSize; j++)
                    outs[i][j] += mAudioTemps[i][j];
        }

        // Release temporary memory and clear allocator
        
        if (getNumAudioOuts())
            mAllocator->dealloc(mAudioTemps[0]);
                
        mAllocator->clear();
    }
   
    // Reset
    
    virtual void reset(double samplingRate)
    {
        mSamplingRate = samplingRate;

        for (std::vector <FrameLib_Block *> :: iterator it = mBlocks.begin(); it != mBlocks.end(); it++)
            (*it)->reset(samplingRate);
    }
    
    // Handles Audio
    
    static bool handlesAudio() { return T::handlesAudio(); }
    
    virtual const char *objectInfo(bool verbose)                    { return mBlocks[0]->objectInfo(verbose); }
    virtual const char *inputInfo(unsigned long idx, bool verbose)  { return mBlocks[0]->inputInfo(idx, verbose); }
    virtual const char *outputInfo(unsigned long idx, bool verbose) { return mBlocks[0]->outputInfo(idx, verbose); }
    virtual const char *audioInfo(unsigned long idx, bool verbose)  { return mBlocks[0]->audioInfo(idx, verbose); }

    virtual FrameType inputType(unsigned long idx)                  { return mBlocks[0]->inputType(idx); }
    virtual FrameType outputType(unsigned long idx)                 { return mBlocks[0]->outputType(idx); }
    
    virtual const FrameLib_Parameters *getParameters()              { return mBlocks[0]->getParameters(); }

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
        // Find number of channels (always keep at least one channel)
        
        unsigned long nChannels = 1;
        unsigned long cChannels = mBlocks.size();
        
        for (unsigned long i = 0; i < getNumIns(); i++)
            if (getInputNumChans(i) > nChannels)
                nChannels = getInputNumChans(i);
        
        // Resize if necessary
        
        bool numChansChanged = nChannels != cChannels;
        
        if (numChansChanged)
        {
            if (nChannels > cChannels)
            {
                mBlocks.resize(nChannels);
                
                for (unsigned long i = cChannels; i < nChannels; i++)
                {
                    mBlocks[i] = new T(mContext, &mSerialisedParameters, mOwner);
                    mBlocks[i]->reset(mSamplingRate);
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

    // Member Variables
    
    FrameLib_Context mContext;
    FrameLib_Context::Allocator mAllocator;
    FrameLib_Parameters::AutoSerial mSerialisedParameters;

    std::vector <FrameLib_Block *> mBlocks;
    std::vector <std::vector <double> > mFixedInputs;
    
    void *mOwner;
    
    double mSamplingRate;
    
    std::vector<double *> mAudioTemps;
};

#endif
