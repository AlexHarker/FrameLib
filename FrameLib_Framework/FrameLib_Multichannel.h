
#ifndef FRAMELIB_MULTICHANNEL_H
#define FRAMELIB_MULTICHANNEL_H

#include "FrameLib_Types.h"
#include "FrameLib_Context.h"
#include "FrameLib_Object.h"
#include "FrameLib_DSP.h"

#include <algorithm>
#include <vector>

// FrameLib_MultiChannel

// This abstract class allows multichannel connnections and the means to update the network according to the number of channels

class FrameLib_MultiChannel : public FrameLib_Object<FrameLib_MultiChannel>
{
    
protected:

    typedef FrameLib_Object<FrameLib_Block>::Connection BlockConnection;
    typedef std::vector<BlockConnection> MultiChannelOutput;
    typedef FrameLib_Object::Connection MultiChannelConnection;
    
public:
    
    virtual const FrameLib_Parameters::Serial *getSerialised() = 0;
    
    // Constructors

    FrameLib_MultiChannel(ObjectType type, FrameLib_Context context, FrameLib_Proxy *proxy, unsigned long nStreams, unsigned long nIns, unsigned long nOuts)
    : FrameLib_Object(type, context, proxy), mNumStreams(nStreams)
    { setIO(nIns, nOuts); }
    
    FrameLib_MultiChannel(ObjectType type, FrameLib_Context context, FrameLib_Proxy *proxy, unsigned long nStreams)
    : FrameLib_Object(type, context, proxy), mNumStreams(nStreams) {}
    
    // Destructor
    
    virtual ~FrameLib_MultiChannel() {}
    
    // Default is not to handle audio
    
    static bool handlesAudio() { return false; }

    // Number of Streams
    
    unsigned long getNumStreams() { return mNumStreams; }
    
protected:
    
    // IO Utilities
    
    // Call this in derived class constructors if the IO size is not always the same
    
    void setIO(unsigned long nIns, unsigned long nOuts, unsigned long nAudioChans = 0)
    {
        FrameLib_Object::setIO(nIns, nOuts, nAudioChans);
        mOutputs.resize(getNumOuts());
    }
    
    // Query Connections for Individual Channels
    
    unsigned long getInputNumChans(unsigned long inIdx);
    BlockConnection getInputChan(unsigned long inIdx, unsigned long chan);
    
    unsigned long getOrderingConnectionNumChans(unsigned long idx);
    BlockConnection getOrderingConnectionChan(unsigned long idx, unsigned long chan);

private:

    // Deleted
    
    FrameLib_MultiChannel(const FrameLib_MultiChannel&) = delete;
    FrameLib_MultiChannel& operator=(const FrameLib_MultiChannel&) = delete;
    
    // Connection Methods (private)
    
    void connectionUpdate(Queue *queue) final
    {
        if (inputUpdate())
            outputUpdate(queue);
    }

    virtual bool inputUpdate() = 0;
    void outputUpdate(Queue *queue);
    
protected:

    // Outputs
    
    std::vector<MultiChannelOutput> mOutputs;
    
private:
    
    unsigned long mNumStreams;
};

// ************************************************************************************** //

// FrameLib_Pack - Pack Multichannel Signals

class FrameLib_Pack final : public FrameLib_MultiChannel
{
    enum AtrributeList { kInputs };

    struct ParameterInfo : public FrameLib_Parameters::Info { ParameterInfo() { add("Sets the number of inputs."); } };
    
public:
    
    const FrameLib_Parameters::Serial *getSerialised() override { return &mSerialisedParameters; }

    FrameLib_Pack(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy, unsigned long nStreams);
    
    // Set Fixed Inputs
    
    void setFixedInput(unsigned long idx, double *input, unsigned long size) override {};
    const double *getFixedInput(unsigned long idx, unsigned long *size) override { return getEmptyFixedInput(idx, size); }
    
    // Audio Processing
    
    void blockUpdate(const double * const *ins, double **outs, unsigned long blockSize) override {}
    void reset(double samplingRate, unsigned long maxBlockSize) override {}

    // Info
    
    std::string objectInfo(bool verbose) override;
    std::string inputInfo(unsigned long idx, bool verbose) override;
    std::string outputInfo(unsigned long idx, bool verbose) override;
    
    const FrameLib_Parameters *getParameters() const override { return &mParameters; }

    FrameType inputType(unsigned long idx) const override { return kFrameAny; }
    FrameType outputType(unsigned long idx) const override { return kFrameAny; }
    
    void autoOrderingConnections() override {}
    void clearAutoOrderingConnections() override {}
    
private:
    
    bool inputUpdate() override;

    FrameLib_Parameters::AutoSerial mSerialisedParameters;

    FrameLib_Parameters mParameters;
    
    static ParameterInfo sParamInfo;
};

// ************************************************************************************** //

// FrameLib_Unpack - Unpack Multichannel Signals

class FrameLib_Unpack final : public FrameLib_MultiChannel
{
    enum AtrributeList { kOutputs };
    
    struct ParameterInfo : public FrameLib_Parameters::Info { ParameterInfo() { add("Sets the number of outputs."); } };

public:

    const FrameLib_Parameters::Serial *getSerialised() override { return &mSerialisedParameters; }

    FrameLib_Unpack(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy, unsigned long nStreams);
    
    // Set Fixed Inputs
    
    void setFixedInput(unsigned long idx, double *input, unsigned long size) override {};
    const double *getFixedInput(unsigned long idx, unsigned long *size) override { return getEmptyFixedInput(idx, size); }
    
    // Audio Processing
    
    void blockUpdate(const double * const *ins, double **outs, unsigned long blockSize) override {}
    void reset(double samplingRate, unsigned long maxBlockSize) override {}

    // Info
    
    std::string objectInfo(bool verbose) override;
    std::string inputInfo(unsigned long idx, bool verbose) override;
    std::string outputInfo(unsigned long idx, bool verbose) override;

    const FrameLib_Parameters *getParameters() const override { return &mParameters; }

    FrameType inputType(unsigned long idx) const override { return kFrameAny; }
    FrameType outputType(unsigned long idx) const override { return kFrameAny; }
    
    void autoOrderingConnections() override {}
    void clearAutoOrderingConnections() override {}
    
private:
    
    virtual bool inputUpdate() override;
    
    FrameLib_Parameters::AutoSerial mSerialisedParameters;

    FrameLib_Parameters mParameters;
    
    static ParameterInfo sParamInfo;
};

// ************************************************************************************** //

// FrameLib_Expand - MultiChannel expansion for FrameLib_Block objects

template <class T> class FrameLib_Expand final : public FrameLib_MultiChannel
{

public:
    
    const FrameLib_Parameters::Serial *getSerialised() override { return &mSerialisedParameters; }

    FrameLib_Expand(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy, unsigned long nStreams)
    : FrameLib_MultiChannel(T::getType(), context, proxy, nStreams), mSerialisedParameters(serialisedParameters->size())
    {
        // Make first block
        
        mBlocks.push_back(new T(context, serialisedParameters, proxy));
        mBlocks[0]->setStream(this, 0);
        
        // Copy serialised parameters for later instantiations

        mSerialisedParameters.write(serialisedParameters);
        
        // Set up IO / fixed inputs / audio temps
        
        setIO(mBlocks[0]->getNumIns(), mBlocks[0]->getNumOuts(), getNumStreams() * mBlocks[0]->getNumAudioChans());
        mFixedInputs.resize(getNumIns());
        mAudioTemps.resize(mBlocks[0]->getNumAudioOuts());
        
        // Make initial output connections
        
        for (unsigned long i = 0; i < getNumOuts(); i++)
            mOutputs[i].push_back(BlockConnection(mBlocks[0], i));
        
        // Check for ordering support
        
        if (mBlocks[0]->supportsOrderingConnections())
            enableOrderingConnections();
        
        reset(0.0, 4096);
    }
    
    ~FrameLib_Expand()
    {
        // Delete blocks
        
        for (auto it = mBlocks.begin(); it != mBlocks.end(); it++)
            delete(*it);
    }
    
    // Fixed Inputs
    
    void setFixedInput(unsigned long idx, double *input, unsigned long size) override
    {
        if (idx < mFixedInputs.size())
        {
            mFixedInputs[idx].assign(input, input + size);
            updateFixedInput(idx);
        }
    }
    
    const double *getFixedInput(unsigned long idx, unsigned long *size) override
    {
        return mBlocks[0]->getFixedInput(idx, size);
    }

    // Audio Processing
        
    void blockUpdate(const double * const *ins, double **outs, unsigned long blockSize) override
    {
        unsigned long internalNumIns = mBlocks[0]->getNumAudioIns();
        unsigned long internalNumOuts = mBlocks[0]->getNumAudioOuts();
        
        // Allocate temporary memory
        
        if (internalNumOuts)
            mAudioTemps[0] = alloc<double>(blockSize * internalNumOuts);
        for (unsigned long i = 1; i < internalNumOuts; i++)
            mAudioTemps[i] = mAudioTemps[0] + (i * blockSize);
            
        // Zero outputs
        
        for (unsigned long i = 0; i < getNumAudioOuts(); i++)
            std::fill_n(outs[i], blockSize, 0.0);

        // Process and sum to outputs

        for (unsigned long i = 0; i < mBlocks.size(); i++)
        {
            unsigned long inStreamOffset = internalNumIns * (i % getNumStreams());
            unsigned long outStreamOffset = internalNumOuts * (i % getNumStreams());
            
            mBlocks[i]->blockUpdate(ins + inStreamOffset, mAudioTemps.data(), blockSize);
            
            for (unsigned long j = 0; j < internalNumOuts; j++)
                for (unsigned long k = 0; k < blockSize; k++)
                    outs[outStreamOffset + j][k] += mAudioTemps[j][k];
        }

        // Release temporary memory and clear allocator
        
        if (getNumAudioOuts())
           dealloc(mAudioTemps[0]);
                
        clearAllocator();
    }
   
    // Reset
    
    void reset(double samplingRate, unsigned long maxBlockSize) override
    {
        mSamplingRate = samplingRate;
        mMaxBlockSize = maxBlockSize;
        
        for (auto it = mBlocks.begin(); it != mBlocks.end(); it++)
            (*it)->reset(samplingRate, maxBlockSize);
    }
    
    // Handles Audio
    
    static bool handlesAudio() { return T::handlesAudio(); }
    
    // Info
    
    std::string objectInfo(bool verbose) override                       { return mBlocks[0]->objectInfo(verbose); }
    std::string inputInfo(unsigned long idx, bool verbose) override     { return mBlocks[0]->inputInfo(idx, verbose); }
    std::string outputInfo(unsigned long idx, bool verbose) override    { return mBlocks[0]->outputInfo(idx, verbose); }
    
    std::string audioInfo(unsigned long idx, bool verbose) override
    {
        return formatInfo((mBlocks[0]->audioInfo(idx % mBlocks[0]->getNumAudioChans(), verbose) + " [#]").c_str(), idx / mBlocks[0]->getNumAudioChans());
    }

    FrameType inputType(unsigned long idx) const override               { return mBlocks[0]->inputType(idx); }
    FrameType outputType(unsigned long idx) const override              { return mBlocks[0]->outputType(idx); }
    
    const FrameLib_Parameters *getParameters() const override           { return mBlocks[0]->getParameters(); }

    // Ordering Connections
    
    void autoOrderingConnections() override
    {
        for (auto it = mBlocks.begin(); it != mBlocks.end(); it++)
            (*it)->autoOrderingConnections();
    }

    void clearAutoOrderingConnections() override
    {
        for (auto it = mBlocks.begin(); it != mBlocks.end(); it++)
            (*it)->clearAutoOrderingConnections();
    }

private:

    // Update Fixed Inputs
    
    void updateFixedInput(unsigned long idx)
    {
        for (unsigned long i = 0; i < mBlocks.size(); i++)
            mBlocks[i]->setFixedInput(idx, mFixedInputs[idx].data(), mFixedInputs[idx].size());
    }
    
    // Update (expand)
    
    bool inputUpdate() override
    {
        // Find number of channels (always keep at least one channel)
        
        unsigned long nChannels = 1;
        unsigned long cChannels = mBlocks.size();
        
        for (unsigned long i = 0; i < getNumIns(); i++)
            if (getInputNumChans(i) > nChannels)
                nChannels = getInputNumChans(i);
        
        nChannels = std::max(nChannels, getNumStreams());
        
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
                    mBlocks[i] = new T(getContext(), &mSerialisedParameters, getProxy());
                    mBlocks[i]->setStream(this, i);
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
                mOutputs[i].clear();
            
            for (unsigned long i = 0; i < getNumOuts(); i++)
                for (unsigned long j = 0; j < nChannels; j++)
                    mOutputs[i].push_back(BlockConnection(mBlocks[j], i));
            
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
                    mBlocks[j]->addConnection(getInputChan(i, j % getInputNumChans(i)), i);
            }
            else
            {
                for (unsigned long j = 0; j < nChannels; j++)
                    mBlocks[j]->deleteConnection(i);
            }
        }
        
        // Clear ordering connections
        
        for (unsigned long j = 0; j < nChannels; j++)
            mBlocks[j]->clearOrderingConnections();
        
        // Make ordering connections
        
        for (unsigned long i = 0; i < getNumOrderingConnections(); i++)
        {
            if (getOrderingConnectionNumChans(i))
                for (unsigned long j = 0; j < nChannels; j++)
                    mBlocks[j]->addOrderingConnection(getOrderingConnectionChan(i, j % getOrderingConnectionNumChans(i)));
        }
        
        return numChansChanged;
    }

    // Member Variables
    
    FrameLib_Parameters::AutoSerial mSerialisedParameters;

    std::vector <FrameLib_Block *> mBlocks;
    std::vector <std::vector <double> > mFixedInputs;

    unsigned long mMaxBlockSize;
    double mSamplingRate;
    
    std::vector<double *> mAudioTemps;
};

#endif
