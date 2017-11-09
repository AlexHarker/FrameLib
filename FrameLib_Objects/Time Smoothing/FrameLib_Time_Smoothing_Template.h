
#ifndef FRAMELIB_TIMESMOOTHING_TEMPLATE_H
#define FRAMELIB_TIMESMOOTHING_TEMPLATE_H

#include "FrameLib_DSP.h"

template <class T> class FrameLib_TimeSmoothing : public FrameLib_Processor
{
    const int sNumFrames = 0;
    
public:
    
    // Constructor
    
    FrameLib_TimeSmoothing(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner) : FrameLib_Processor(context, owner, NULL, 1, 1), mLastLength(0), mLastNumFrames(0), mCounter(0), mFrames(NULL)
    {
        mParameters.addInt(sNumFrames, "num_frames", 10, 0);
        
        serialisedParameters->read(&mParameters);
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
    
private:
    
    // Smooth
    
    virtual void smooth(double *output, const double *newFrame, const double *oldestFrame, unsigned long size) = 0;
    virtual void resetSize(unsigned long size) = 0;

    // Process
    
    void process()
    {
        unsigned long sizeIn, sizeOut;
        unsigned long numFrames = mParameters.getInt(sNumFrames);
        
        const double *input = getInput(0, &sizeIn);
        
        if (mLastLength != sizeIn || mLastNumFrames != numFrames)
        {
            unsigned long totalSize = sizeIn * numFrames;
            
            dealloc(mFrames);
            mFrames = alloc<double>(totalSize);
            
            zeroVector(mFrames, totalSize);
            
            mCounter = 0;
            mLastLength = sizeIn;
            mLastNumFrames = numFrames;
            
            resetSize(sizeIn);
        }
        
        requestOutputSize(0, sizeIn);
        allocateOutputs();
        double *output = getOutput(0, &sizeOut);
        
        if (!mFrames)
            return;
            
        // Object must deal with NULL outputs
        
        double *oldestFrame = mFrames + (mCounter * sizeIn);
        smooth(output, input, oldestFrame, sizeIn);
        copyVector(oldestFrame, input, sizeIn);
        mCounter = (++mCounter >= numFrames) ? 0 : mCounter;
    }
    
    // Operator Description (specialise to change description)

    const char *getOpString() { return "<vector operation>"; }
    
    unsigned long mLastLength;
    unsigned long mLastNumFrames;
    unsigned long mCounter;

    unsigned long mFrameSize;
    double *mFrames;
};

#endif
