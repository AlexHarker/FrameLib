
#ifndef FRAMELIB_TIMESMOOTHING_TEMPLATE_H
#define FRAMELIB_TIMESMOOTHING_TEMPLATE_H

#include "FrameLib_FrameSet.h"
#include "FrameLib_DSP.h"

template <class T> class FrameLib_TimeSmoothing : public FrameLib_Processor, private FrameLib_RingBuffer
{
    const int sMaxFrames = 0;
    const int sNumFrames = 1;
    
public:
    
    // Constructor
    
    FrameLib_TimeSmoothing(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner) : FrameLib_Processor(context, owner, NULL, 2, 1), FrameLib_RingBuffer(context), mLastNumFrames(0)
    {
        mParameters.addInt(sMaxFrames, "max_frames", 10, 0);
        mParameters.setMin(1);
        serialisedParameters->read(&mParameters);
    
        mParameters.addInt(sNumFrames, "num_frames", 10, 1);
        mParameters.setMin(1);
        serialisedParameters->read(&mParameters);

        setParameterInput(1);
    }
    
    // Info
    
    std::string objectInfo(bool verbose)
    {
        return formatInfo("Calculates the # of the input frame: The result is a single value.",
                       "Calculates the # of the input frame.", getOpString(), verbose);
    }
    
    std::string inputInfo(unsigned long idx, bool verbose)      { return "Input"; }
    std::string outputInfo(unsigned long idx, bool verbose)     { return "Result"; }

protected:
    
    void smoothReset()
    {
        FrameLib_RingBuffer::resize(0, 0);
        mLastNumFrames = 0;
    }
    
    unsigned long getMaxFrames() const  { return mParameters.getInt(sMaxFrames); }

    unsigned long getNumFrames() const
    {
        unsigned long maxFrames = getMaxFrames();
        unsigned long numFrames = mParameters.getInt(sNumFrames);
        
        return maxFrames < numFrames ? maxFrames : numFrames;
    }

private:
    
    // Smooth
    
    virtual void add(const double *newFrame, unsigned long size) = 0;
    virtual void remove(const double *oldFrame, unsigned long size) = 0;

    virtual void exchange(const double *newFrame, const double *oldFrame, unsigned long size)
    {
        remove(oldFrame, size);
        add(newFrame, size);
    }
    
    virtual void result(double *output, unsigned long size) = 0;
    
    virtual void resetSize(unsigned long maxFrames, unsigned long size) = 0;
    
    // Process
    
    void process()
    {
        unsigned long sizeIn, sizeOut;
        unsigned long maxFrames = getMaxFrames();
        unsigned long numFrames = getNumFrames();
        
        const double *input = getInput(0, &sizeIn);

        if (getFrameLength() != sizeIn || FrameLib_RingBuffer::getNumFrames() != maxFrames)
        {
            resize(maxFrames, sizeIn);
            resetSize(maxFrames, sizeIn);
        }

        requestOutputSize(0, getFrameLength());
        allocateOutputs();
        double *output = getOutput(0, &sizeOut);
        
        if (numFrames > mLastNumFrames)
        {
            for (unsigned long i = numFrames - 1; i > mLastNumFrames; i--)
                add(getFrame(i), sizeIn);
            
            add(input, sizeIn);
        }
        else
        {
            for (unsigned long i = mLastNumFrames; i > numFrames; i--)
                remove(getFrame(i), sizeIn);
            
            exchange(input, getFrame(numFrames), sizeIn);
        }
        
        result(output, sizeOut);
        write(input, sizeIn);
        mLastNumFrames = numFrames;
    }
    
    // Operator Description (specialise to change description)

    const char *getOpString() { return "<vector operation>"; }
    
    unsigned long mLastNumFrames;
};

#endif
