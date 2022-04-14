
#ifndef FRAMELIB_TIMEBUFFER_TEMPLATE_H
#define FRAMELIB_TIMEBUFFER_TEMPLATE_H

#include "FrameLib_RingBuffer.h"
#include "FrameLib_PaddedVector.h"
#include "FrameLib_DSP.h"

#include <type_traits>

template <class T, unsigned long nParams = 0>
class FrameLib_TimeBuffer : public FrameLib_Processor, private FrameLib_RingBuffer
{
protected:
 
    using Padded = const PaddedVector&;

    enum ParameterList
    {
        kMaxFrames = 0,
        kNumFrames = 1,
        kPadding = 2 + nParams,
        kStartMode = 3 + nParams
    };
    
    enum StartModes { kPad, kShorten };
    
    // Parameter Info

    struct ParameterInfo : public FrameLib_Parameters::Info
    {
        ParameterInfo()
        {
            add("Sets the maximum number of frames for calculation. "
                "Note that the internal buffer is reset when this changes.");
            add("Sets the current number of frames for calculation as an integer.");
            
            addCustomInfo<nParams != 0>();
            
            add("Sets the padding value.");
            add("Sets the behaviour when there are insufficient frames stored (as after a reset): "
                "pad - pad the calculation with default values to account for the missing frames. "
                "shorten - calculate on all frames received since the reset. "
                "Note that frames at the reset input set the frame used for padding. "
                "If the frame is too short or empty it is padded with the pad parameter.");
        }
        
        template <bool B, typename std::enable_if<B, int>::type = 0>
        void addCustomInfo()
        {
            for (unsigned long i = 0; i < nParams; i++)
                add(T::paramInfo(i));
        }
        
        template <bool B, typename std::enable_if<!B, int >::type = 0>
        void addCustomInfo() {}
    };
    
    void completeDefaultParameters(const FrameLib_Parameters::Serial *serialisedParameters)
    {
        mParameters.addDouble(kPadding, "pad", 0.0, kPadding);
        
        mParameters.addEnum(kStartMode, "start");
        mParameters.addEnumItem(kPad, "pad");
        mParameters.addEnumItem(kShorten, "shorten");
        
        mParameters.set(serialisedParameters);
    }
    
public:
    
    // Constructor
    
    FrameLib_TimeBuffer(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy)
    : FrameLib_Processor(context, proxy, &sParamInfo, 3, 2)
    , FrameLib_RingBuffer(this)
    , mLastNumFrames(0)
    {
        mParameters.addInt(kMaxFrames, "max_frames", 10, 0);
        mParameters.setMin(1);
    
        mParameters.addInt(kNumFrames, "num_frames", 10, 1);
        mParameters.setMin(1);
        
        if (!nParams)
            completeDefaultParameters(serialisedParameters);
        
        setParameterInput(2);
    }
    
    // Info
    
    virtual const char *getOpString() { return ""; }
    virtual const char *getExtraString() { return ""; }

    std::string objectInfo(bool verbose) override
    {
        if (verbose)
        {
            std::string str = formatInfo("Calculates the # per sample over the most recent frames: ", getOpString());
            
            str += getExtraString();
            
            str += "The output is the same length as the input. "
                    "Input frames are expected to be of a uniform length. "
                   "If the input length changes then the internal storage is reset. "
                   "The object can also be reset by sending a frame to the reset input. "
                   "The start parameter controls behaviour when insufficient frames are stored. "
                   "The underlength output indicates that insufficient frames are stored.";
            
            return str;
        }
        else
        {
            return formatInfo("Calculates the # per sample over the most recent frames.", getOpString());
        }
    }
    
    std::string inputInfo(unsigned long idx, bool verbose) override
    {
        switch (idx)
        {
            case 0:     return "Input";
            case 1:     return formatInfo("Reset Input - resets and sets padding frame without output", "Reset Input", verbose);
            default:    return parameterInputInfo(verbose);
        }
    }
    
    std::string outputInfo(unsigned long idx, bool verbose) override
    {
        if (idx)
            return formatInfo("Underlength - indicates insufficient frames are stored", "Underlength", verbose);
        else
            return "Output";
    }
    
protected:
    
    unsigned long getMaxFrames() const  { return mParameters.getInt(kMaxFrames); }

    unsigned long getNumFrames(bool forceValid = false) const
    {
        bool valid = forceValid || mParameters.getEnum<StartModes>(kStartMode) == kShorten;
        
        if (valid)
            return std::min(getRequestedNumFrames(), getValidFrames() + 1);
        else
            return getRequestedNumFrames();
    }
    
private:
    
    unsigned long getRequestedNumFrames() const
    {
        unsigned long maxFrames = getMaxFrames();
        unsigned long numFrames = mParameters.getInt(kNumFrames);
        
        return std::min(maxFrames, numFrames);
    }
    
    // Smooth
    
    virtual void resetSize(unsigned long maxFrames, unsigned long size) = 0;
    
    virtual void result(double *output, unsigned long size, Padded pad, unsigned long padSize) = 0;
    
    virtual void add(const double *newFrame, unsigned long size) = 0;
    virtual void remove(const double *oldFrame, unsigned long size) = 0;

    virtual void exchange(const double *newFrame, const double *oldFrame, unsigned long size)
    {
        remove(oldFrame, size);
        add(newFrame, size);
    }
    
    // Object reset
    
    void objectReset() override
    {
        resize(0, 0);
        resetSize(0, 0);
        mLastNumFrames = 0;
        mLastResetTime = FrameLib_TimeFormat(0);
    }
    
    // Process
    
    void process() override
    {
        unsigned long sizeIn, sizeReset, sizeOut, sizeEdge;

        StartModes startMode = mParameters.getEnum<StartModes>(kStartMode);
        double pad = mParameters.getValue(kPadding);
        
        unsigned long numFrames;
        unsigned long maxFrames = getMaxFrames();
        unsigned long requestedFrames = getRequestedNumFrames();

        const double *input = getInput(0, &sizeIn);
        const double *resetInput = getInput(1, &sizeReset);

        const bool forceReset = mLastResetTime != getInputFrameTime(1);
        const bool frameSizeMismatch = sizeIn != getFrameLength();
        const bool maxFramesMismatch = maxFrames != FrameLib_RingBuffer::getNumFrames();
        
        if (forceReset || frameSizeMismatch || maxFramesMismatch)
        {
            resize(maxFrames, sizeIn);
            resetSize(maxFrames, sizeIn);
            mLastNumFrames = 0;
            mLastResetTime = getInputFrameTime(1);
        }

        // N.B. retrieve actual number of frames after reset
        
        numFrames = getNumFrames(true);
        
        // Allocate outputs
        
        requestOutputSize(0, getFrameLength());
        requestOutputSize(1, 1);
        allocateOutputs();
        double *outputMain = getOutput(0, &sizeOut);
        double *outputEdge = getOutput(1, &sizeEdge);
        
        // Update with the correct, frames, store and output
        
        if (sizeIn)
        {
            unsigned long padSize = startMode == kPad ? requestedFrames - numFrames : 0;

            if (numFrames > mLastNumFrames)
            {
                for (unsigned long i = numFrames; i > mLastNumFrames + 1; i--)
                    add(getFrame(i - 1), sizeIn);
                
                add(input, sizeIn);
            }
            else
            {
                for (unsigned long i = mLastNumFrames; i > numFrames; i--)
                    remove(getFrame(i), sizeIn);
                
                exchange(input, getFrame(numFrames), sizeIn);
            }
            
            result(outputMain, sizeOut, PaddedVector(resetInput, sizeReset, pad), padSize);
            write(input, sizeIn);
            mLastNumFrames = numFrames;
        }
        
        if (sizeEdge)
            outputEdge[0] = sizeIn && requestedFrames != numFrames ? 1 : 0;
    }
    
    static ParameterInfo sParamInfo;

    FrameLib_TimeFormat mLastResetTime;
    unsigned long mLastNumFrames;
};

template <class T, unsigned long N>
typename FrameLib_TimeBuffer<T, N>::ParameterInfo FrameLib_TimeBuffer<T, N>::sParamInfo;

#endif
