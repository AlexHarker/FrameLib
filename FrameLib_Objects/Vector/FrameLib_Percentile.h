
#ifndef FRAMELIB_PRECENTILE_H
#define FRAMELIB_PRECENTILE_H

#include "FrameLib_DSP.h"
#include "FrameLib_Sort_Functions.h"

class FrameLib_Percentile : public FrameLib_Processor
{
    enum AttributeList {kPercentile};
    
public:
    
    FrameLib_Percentile (DSPQueue *queue, FrameLib_Attributes::Serial *serialisedAttributes) : FrameLib_Processor(queue, 1, 1)
    {
        mAttributes.addDouble(kPercentile, "percentile", 50.0, 0);
        mAttributes.setClip(0.0, 100.0);
        
        mAttributes.set(serialisedAttributes);
        
        mPercentile = mAttributes.getValue(kPercentile);
    }
    
protected:
    
    void process()
    {
        unsigned long sizeIn, sizeOut;
        double *input = getInput(0, &sizeIn);
        
        requestOutputSize(0, sizeIn ? 1 : 0);
        allocateOutputs();
        
        double *output = getOutput(0, &sizeOut);
        double *temp = (double *) mAllocator->alloc(sizeof(double) * (sizeIn + 1));
        
        if (sizeOut && temp)
        {
            sortAscending(temp, input, sizeIn);
            
            // Copy last value
            
            temp[sizeIn] = temp[sizeIn - 1];
            
            // Linearly interpolate output
            
            double position = (mPercentile * (sizeIn - 1) / 100.0);
            unsigned long idx = position;
            double fract = position - idx;
            
            output[0] = input[idx] + fract * (input[idx + 1] - input[idx]);
        }
    }
    
private:
    
    double mPercentile;
};

#endif