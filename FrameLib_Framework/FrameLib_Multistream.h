
#ifndef FRAMELIB_MULTISTREAM_H
#define FRAMELIB_MULTISTREAM_H

#include "FrameLib_Types.h"
#include "FrameLib_Context.h"
#include "FrameLib_Object.h"
#include "FrameLib_DSP.h"

#include <algorithm>
#include <vector>

/**
 
 @defgroup Multistream
 
 */

/**
 
 @class FrameLib_Multistream
 
 @ingroup Multistream
 
 @brief a abstract class proving multistream connnections and the means to the number of streams in a network.
 
 */

class FrameLib_Multistream
: public FrameLib_Object<FrameLib_Multistream>
, public FrameLib_Stack<FrameLib_Multistream, FrameLib_Multistream>::Node
{
protected:

    using InputStack = FrameLib_Stack<FrameLib_Multistream, FrameLib_Multistream>;
    using BlockConnection = FrameLib_Object<FrameLib_Block>::Connection;
    using MultistreamOutput = std::vector<BlockConnection>;
    using MultistreamConnection = FrameLib_Object::Connection;
    
public:
    
    virtual const FrameLib_Parameters::Serial *getSerialised() = 0;
    
    // Constructors

    FrameLib_Multistream(ObjectType type, FrameLib_Context context, FrameLib_Proxy *proxy, bool ownsStreams, unsigned long nStreams, unsigned long nIns, unsigned long nOuts)
    : FrameLib_Object(type, context, proxy)
    , mNumStreams(nStreams)
    , mInCount(0)
    , mOwnsStreams(ownsStreams)
    , mOutputChange(false)
    {
        setIO(nIns, nOuts);
    }
    
    FrameLib_Multistream(ObjectType type, FrameLib_Context context, FrameLib_Proxy *proxy, bool ownsStreams, unsigned long nStreams)
    : FrameLib_Object(type, context, proxy)
    , mNumStreams(nStreams)
    , mInCount(0)
    , mOwnsStreams(ownsStreams)
    , mOutputChange(false)
    {}
    
    // Destructor
    
    virtual ~FrameLib_Multistream() {}
    
    // Non-copyable
    
    FrameLib_Multistream(const FrameLib_Multistream&) = delete;
    FrameLib_Multistream& operator=(const FrameLib_Multistream&) = delete;

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
    
    unsigned long getInputNumStreams(unsigned long inIdx);
    BlockConnection getInputChan(unsigned long inIdx, unsigned long chan);
    
    unsigned long getOrderingConnectionNumStreams(unsigned long idx);
    BlockConnection getOrderingConnectionChan(unsigned long idx, unsigned long chan);

private:

    // Connection Methods (private)
    
    void connectionUpdate(Queue *queue) final;

    virtual bool inputUpdate() = 0;
    void outputUpdate(Queue *queue);

    void inputCheck(InputStack& stack);

protected:

    // Outputs
    
    std::vector<MultistreamOutput> mOutputs;
    
private:
    
    unsigned long mNumStreams;
    unsigned long mInCount;
    bool mOwnsStreams;
    bool mOutputChange;
};


/**
 
 @class FrameLib_Expand
 
 @ingroup Multistream

 @brief a template class for providing multistream support to any FrameLib_Block class.
 
 */

template <class T>
class FrameLib_Expand final : public FrameLib_Multistream
{
public:
    
    static constexpr ObjectType sType = T::sType;
    static constexpr bool sHandlesAudio = T::sHandlesAudio;
    
    const FrameLib_Parameters::Serial *getSerialised() override { return &mSerialisedParameters; }

    FrameLib_Expand(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy, unsigned long nStreams)
    : FrameLib_Multistream(T::sType, context, proxy, true, nStreams)
    , mSerialisedParameters(serialisedParameters ? serialisedParameters->size() : 0)
    {
        // Make first block
        
        mBlocks.add(new T(context, serialisedParameters, proxy));
        mBlocks[0]->setStream(this, 0);
        
        // Copy serialised parameters for later instantiations

        mSerialisedParameters.write(serialisedParameters);
        
        // Set up IO / fixed inputs / audio temps
        
        setIO(mBlocks[0]->getNumIns(), mBlocks[0]->getNumOuts(), getNumStreams() * mBlocks[0]->getNumAudioChans());
        mFixedInputs.resize(getNumIns());
        mAudioTemps.resize(mBlocks[0]->getNumAudioOuts());
        
        // Make initial output connections
        
        for (unsigned long i = 0; i < getNumOuts(); i++)
            mOutputs[i].push_back(BlockConnection(mBlocks[0].get(), i));
        
        // Check for ordering support
        
        if (mBlocks[0]->supportsOrderingConnections())
            enableOrderingConnections();
        
        reset(0.0, 4096);
    }
    
    // Fixed Inputs
    
    void setFixedInput(unsigned long idx, const double *input, unsigned long size) override
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
    
    uint64_t getBlockTime() const override { return mBlocks[0]->getBlockTime(); }

    void blockUpdate(const double * const *ins, double **outs, unsigned long blockSize, FrameLib_AudioQueue& queue) override
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

        for (size_t i = 0; i < mBlocks.size(); i++)
        {
            unsigned long inStreamOffset = internalNumIns * (static_cast<unsigned long>(i) % getNumStreams());
            unsigned long outStreamOffset = internalNumOuts * (static_cast<unsigned long>(i) % getNumStreams());
            
            mBlocks[i]->blockUpdate(ins + inStreamOffset, mAudioTemps.data(), blockSize, queue);
            
            for (unsigned long j = 0; j < internalNumOuts; j++)
                for (unsigned long k = 0; k < blockSize; k++)
                    outs[outStreamOffset + j][k] += mAudioTemps[j][k];
        }

        // Release temporary memory and clear allocator
        
        if (getNumAudioOuts())
           dealloc(mAudioTemps[0]);
    }
    
    void blockUpdate(const double * const *ins, double **outs, unsigned long blockSize) override
    {
        FrameLib_AudioQueue queue;
        blockUpdate(ins, outs, blockSize, queue);
    }
    
    // Reset
    
    void reset(double samplingRate, unsigned long maxBlockSize) override
    {
        mSamplingRate = samplingRate;
        mMaxBlockSize = maxBlockSize;
        
        for (auto it = mBlocks.begin(); it != mBlocks.end(); it++)
            (*it)->reset(samplingRate, maxBlockSize);
    }
    
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
        for (size_t i = 0; i < mBlocks.size(); i++)
        {
            unsigned long size = static_cast<unsigned long>(mFixedInputs[idx].size());
            mBlocks[i]->setFixedInput(idx, mFixedInputs[idx].data(), size);
        }
    }
    
    // Update (expand)
    
    bool inputUpdate() override
    {
        // Find number of channels (always keep at least one channel)
        
        unsigned long nChannels = 1;
        unsigned long cChannels = static_cast<unsigned long>(mBlocks.size());
        
        for (unsigned long i = 0; i < getNumIns(); i++)
            if (getInputNumStreams(i) > nChannels)
                nChannels = getInputNumStreams(i);
        
        nChannels = std::max(nChannels, getNumStreams());
        
        // Resize if necessary
        
        bool numChansChanged = nChannels != cChannels;
        
        if (numChansChanged)
        {
            // Change the number of hosted blocks
            
            if (nChannels > cChannels)
            {                
                for (unsigned long i = cChannels; i < nChannels; i++)
                {
                    mBlocks.add(new T(getContext(), &mSerialisedParameters, getProxy()));
                    mBlocks.back()->setStream(this, i);
                    mBlocks.back()->reset(mSamplingRate, mMaxBlockSize);
                }
            }
            else
                mBlocks.resize(nChannels);
            
            // Redo output connection lists
            
            for (unsigned long i = 0; i < getNumOuts(); i++)
                mOutputs[i].clear();
            
            for (unsigned long i = 0; i < getNumOuts(); i++)
                for (unsigned long j = 0; j < nChannels; j++)
                    mOutputs[i].push_back(BlockConnection(mBlocks[j].get(), i));
            
            // Update Fixed Inputs
            
            for (unsigned long i = 0; i < getNumIns(); i++)
                updateFixedInput(i);
        }
        
        // Make input connections

        for (unsigned long i = 0; i < getNumIns(); i++)
        {
            if (getInputNumStreams(i))
            {
                for (unsigned long j = 0; j < nChannels; j++)
                    mBlocks[j]->addConnection(getInputChan(i, j % getInputNumStreams(i)), i);
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
            if (getOrderingConnectionNumStreams(i))
                for (unsigned long j = 0; j < nChannels; j++)
                    mBlocks[j]->addOrderingConnection(getOrderingConnectionChan(i, j % getOrderingConnectionNumStreams(i)));
        }
        
        return numChansChanged;
    }

    // Member Variables
    
    FrameLib_Parameters::AutoSerial mSerialisedParameters;

    FrameLib_OwnedList<FrameLib_Block> mBlocks;
    std::vector<std::vector<double>> mFixedInputs;

    unsigned long mMaxBlockSize;
    double mSamplingRate;
    
    std::vector<double *> mAudioTemps;
};

#endif
