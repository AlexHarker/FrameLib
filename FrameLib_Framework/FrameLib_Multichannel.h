
#ifndef FRAMELIB_MULTICHANNEL_H
#define FRAMELIB_MULTICHANNEL_H

#include "FrameLib_Context.h"
#include "FrameLib_Object.h"
#include "FrameLib_DSP.h"
#include <algorithm>
#include <vector>

// FrameLib_MultiChannel

// This abstract class allows mulitchannel connnections and the means to update the network according to the number of channels

class FrameLib_MultiChannel : public FrameLib_Object<FrameLib_MultiChannel>
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

    FrameLib_MultiChannel(ObjectType type, FrameLib_Context context, void *owner, unsigned long nIns, unsigned long nOuts)
    : FrameLib_Object(type, context, owner, this), mDeleted(false)
    { setIO(nIns, nOuts); }
    
    FrameLib_MultiChannel(ObjectType type, FrameLib_Context context, void *owner) : FrameLib_Object(type, context, owner, this), mDeleted(false) {}
    
    // Destructor
    
    virtual ~FrameLib_MultiChannel()
    {
        mDeleted = true;
        clearConnections();
    }
    
    // Set Fixed Inputs
    
    virtual void setFixedInput(unsigned long idx, double *input, unsigned long size) {};

    // Audio Processing
    
    virtual void blockUpdate(double **ins, double **outs, unsigned long blockSize) {}
    virtual void reset(double samplingRate, unsigned long maxBlockSize) {}

    static bool handlesAudio() { return false; }

protected:
    
    // IO Utilities
    
    // Call this in derived class constructors if the IO size is not always the same
    
    void setIO(unsigned long nIns, unsigned long nOuts, unsigned long nAudioChans = 0)
    {
        FrameLib_Object::setIO(nIns, nOuts, nAudioChans);
        mOutputs.resize(getNumOuts());
    }
    
    // Query Input Channels
    
    unsigned long getInputNumChans(unsigned long inIdx);
    ConnectionInfo getInputChan(unsigned long inIdx, unsigned long chan);
    unsigned long getDependencyConnectionNumChans(unsigned long idx);
    ConnectionInfo getDependencyConnectionChan(unsigned long idx, unsigned long chan);

private:

    // Deleted
    
    FrameLib_MultiChannel(const FrameLib_MultiChannel&);
    FrameLib_MultiChannel& operator=(const FrameLib_MultiChannel&);
    
    // Connection Methods (private)
    
    void connectionUpdate(Queue *queue)
    {
        if (!mDeleted && inputUpdate())
            outputUpdate(queue);
    }

    virtual bool inputUpdate() = 0;
    void outputUpdate(Queue *queue);
    
protected:

    // Outputs
    
    std::vector <MultiChannelOutput> mOutputs;
    
private:
    
    bool mDeleted;
};

// ************************************************************************************** //

// FrameLib_Pack - Pack Multichannel Signals

class FrameLib_Pack : public FrameLib_MultiChannel
{
    enum AtrributeList { kInputs };

    struct ParameterInfo : public FrameLib_Parameters::Info { ParameterInfo() { add("Sets the number of inputs."); } };
    
public:
    
    FrameLib_Pack(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner);
    
    // Info
    
    virtual std::string objectInfo(bool verbose);
    virtual std::string inputInfo(unsigned long idx, bool verbose);
    virtual std::string outputInfo(unsigned long idx, bool verbose);
    
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

class FrameLib_Unpack : public FrameLib_MultiChannel
{
    enum AtrributeList { kOutputs };
    
    struct ParameterInfo : public FrameLib_Parameters::Info { ParameterInfo() { add("Sets the number of outputs."); } };

public:

    FrameLib_Unpack(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner);
    
    // Info
    
    virtual std::string objectInfo(bool verbose);
    virtual std::string inputInfo(unsigned long idx, bool verbose);
    virtual std::string outputInfo(unsigned long idx, bool verbose);

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
    : FrameLib_MultiChannel(T::getType(), context, owner), mAllocator(context), mSerialisedParameters(serialisedParameters->size())
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
        
        // Check for dependency support
        
        if (mBlocks[0]->supportsDependencyConnections())
            enableDependencyConnections();
        
        reset(0.0, 4096);
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
    
    // FIX - allow the expand object to write/read different blocks from different audio IO (auto multichannel)
    
    virtual void blockUpdate(double **ins, double **outs, unsigned long blockSize)
    {
        // Allocate temporary memory
        
        if (getNumAudioOuts())
            mAudioTemps[0] = (double *) mAllocator->alloc(sizeof(double) * blockSize * getNumAudioOuts());
        for (unsigned long i = 1; i < getNumAudioOuts(); i++)
            mAudioTemps[i] = mAudioTemps[0] + (i * blockSize);
            
        // Zero outputs
        
        for (unsigned long i = 0; i < getNumAudioOuts(); i++)
            std::fill_n(outs[i], blockSize, 0.0);

        // Process and sum to outputs

        for (std::vector <FrameLib_Block *> :: iterator it = mBlocks.begin(); it != mBlocks.end(); it++)
        {
            (*it)->blockUpdate(ins, &mAudioTemps[0], blockSize);
            
            for (unsigned long i = 0; i < getNumAudioOuts(); i++)
                for (unsigned long j = 0; j < blockSize; j++)
                    outs[i][j] += mAudioTemps[i][j];
        }

        // Release temporary memory and clear allocator
        
        if (getNumAudioOuts())
            mAllocator->dealloc(mAudioTemps[0]);
                
        mAllocator->clear();
    }
   
    // Reset
    
    virtual void reset(double samplingRate, unsigned long maxBlockSize)
    {
        mSamplingRate = samplingRate;
        mMaxBlockSize = maxBlockSize;
        
        for (std::vector <FrameLib_Block *> :: iterator it = mBlocks.begin(); it != mBlocks.end(); it++)
            (*it)->reset(samplingRate, maxBlockSize);
    }
    
    // Handles Audio
    
    static bool handlesAudio() { return T::handlesAudio(); }
    
    virtual std::string objectInfo(bool verbose)                    { return mBlocks[0]->objectInfo(verbose); }
    virtual std::string inputInfo(unsigned long idx, bool verbose)  { return mBlocks[0]->inputInfo(idx, verbose); }
    virtual std::string outputInfo(unsigned long idx, bool verbose) { return mBlocks[0]->outputInfo(idx, verbose); }
    virtual std::string audioInfo(unsigned long idx, bool verbose)  { return mBlocks[0]->audioInfo(idx, verbose); }

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
            // Change the number of hosted blocks
            
            if (nChannels > cChannels)
            {
                mBlocks.resize(nChannels);
                
                for (unsigned long i = cChannels; i < nChannels; i++)
                {
                    mBlocks[i] = new T(getContext(), &mSerialisedParameters, getOwner());
                    mBlocks[i]->reset(mSamplingRate, mMaxBlockSize);
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
        
        // Clear dependency connections
        
        for (unsigned long j = 0; j < nChannels; j++)
            mBlocks[j]->clearDependencyConnections();
        
        // Make dependency connections
        
        for (unsigned long i = 0; i < getNumDependencyConnections(); i++)
        {
            if (getDependencyConnectionNumChans(i))
            {
                for (unsigned long j = 0; j < nChannels; j++)
                {
                    ConnectionInfo connection = getDependencyConnectionChan(i, j % getDependencyConnectionNumChans(i));
                    mBlocks[j]->addDependencyConnection(connection.mObject, connection.mIndex);
                }
            }
        }
        
        return numChansChanged;
    }

    // Member Variables
    
    FrameLib_Context::Allocator mAllocator;
    FrameLib_Parameters::AutoSerial mSerialisedParameters;

    std::vector <FrameLib_Block *> mBlocks;
    std::vector <std::vector <double> > mFixedInputs;
    
    unsigned long mMaxBlockSize;
    double mSamplingRate;
    
    std::vector<double *> mAudioTemps;
};

#endif
