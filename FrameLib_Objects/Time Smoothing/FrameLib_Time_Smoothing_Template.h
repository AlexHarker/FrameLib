
#ifndef FRAMELIB_TIMESMOOTHING_TEMPLATE_H
#define FRAMELIB_TIMESMOOTHING_TEMPLATE_H

#include "FrameLib_DSP.h"

template <class T> class FrameLib_TimeSmoothing : public FrameLib_Processor
{
    const int sMaxFrames = 0;
    const int sNumFrames = 1;
    
public:
    
    // Constructor
    
    FrameLib_TimeSmoothing(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner) : FrameLib_Processor(context, owner, NULL, 2, 1), mLastLength(0), mLastNumFrames(0), mLastMaxFrames(0), mCounter(0), mFrames(NULL)
    {
        mParameters.addInt(sMaxFrames, "max_frames", 10, 0);
        mParameters.setMin(1);
        serialisedParameters->read(&mParameters);
    
        mParameters.addInt(sNumFrames, "num_frames", 10, 1);
        mParameters.setClip(1, mParameters.getInt(sMaxFrames));
        serialisedParameters->read(&mParameters);

        setParameterInput(1);
    }
    
    ~FrameLib_TimeSmoothing()
    {
        dealloc(mFrames);
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
        mLastLength = 0;
        mLastNumFrames = 0;
    }
    
    unsigned long getNumFrames() const { return mLastNumFrames; }
    unsigned long getMaxFrames() const { return mLastMaxFrames; }
    
    void incrementCounter(unsigned long& counter)
    {
        counter = (++counter >= getMaxFrames()) ? 0 : counter;
    }
    
    unsigned long calcFrameCount(unsigned long nFrames)
    {
        unsigned long maxCount = getMaxFrames();
        unsigned long count = mCounter + maxCount - nFrames;
        
        return count >= maxCount ? count - maxCount : count;
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
    
    virtual void resetSize(unsigned long size) = 0;

    // Process
    
    void process()
    {
        unsigned long sizeIn, sizeOut;
        unsigned long maxFrames = mParameters.getInt(sMaxFrames);
        unsigned long numFrames = mParameters.getInt(sNumFrames);
        
        const double *input = getInput(0, &sizeIn);
        const double *frame;

        if (mLastLength != sizeIn || mLastMaxFrames != maxFrames)
        {
            unsigned long totalSize = sizeIn * maxFrames;
            
            dealloc(mFrames);
            mFrames = alloc<double>(totalSize);
            
            zeroVector(mFrames, totalSize);
            
            mCounter = 0;
            mLastLength = sizeIn;
            mLastMaxFrames = maxFrames;
            resetSize(sizeIn);
        }

        mLastNumFrames = numFrames;

        requestOutputSize(0, sizeIn);
        allocateOutputs();
        double *output = getOutput(0, &sizeOut);
        
        if (!mFrames)
            return;
        
        if (numFrames > mLastNumFrames)
        {
            unsigned long frameCount = calcFrameCount(numFrames - 1);
            
            for (unsigned long i = 0; i < (numFrames - mLastNumFrames) - 1; i++)
            {
                frame = mFrames + (frameCount * sizeIn);
                add(frame, sizeIn);
                incrementCounter(frameCount);
            }
            
            add(input, sizeIn);
        }
        else
        {
            unsigned long frameCount = calcFrameCount(numFrames);

            for (unsigned long i = 1; i < (mLastNumFrames - numFrames); i++)
            {
                frame = mFrames + (frameCount * sizeIn);
                remove(frame, sizeIn);
                incrementCounter(frameCount);
            }
            
            exchange(input, mFrames + (frameCount * sizeIn), sizeIn);
        }
        
        result(output, sizeOut);
        copyVector(mFrames + (mCounter * sizeIn), input, sizeIn);
        incrementCounter(mCounter);
    }
    
    // Operator Description (specialise to change description)

    const char *getOpString() { return "<vector operation>"; }
    
    unsigned long mLastLength;
    unsigned long mLastNumFrames;
    unsigned long mLastMaxFrames;
    unsigned long mCounter;

    unsigned long mFrameSize;
    double *mFrames;
};

#endif
