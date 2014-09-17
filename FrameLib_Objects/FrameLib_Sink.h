

#ifndef FRAMELIB_SINK_H
#define FRAMELIB_SINK_H

#include "FrameLib_DSP.h"
#include <cstring>

// FIX - MAX_VECTOR_SIZE hack
// FIX - sink is only sample accurate (not subsample) - double the buffer and add a function to interpolate if neceesary

#define MAX_VECTOR_SIZE 8192

class FrameLib_Sink : public FrameLib_Output
{
    enum AttributeList {kLength, kUnit};
    enum Units {kMS, kSeconds, kSamples};
    
public:
    
    FrameLib_Sink(DSPQueue *queue, FrameLib_Attributes::Serial *serialisedAttributes) : FrameLib_Output(queue, 1, 0, 1, 1)
    {
        mAttributes.addDouble(kLength, "length", 8000, 0);
        mAttributes.setMin(0.0);
        mAttributes.addEnum(kUnit, "unit", 1);
        mAttributes.addEnumItem(kMS, "ms");
        mAttributes.addEnumItem(kSeconds, "seconds");
        mAttributes.addEnumItem(kSamples, "samples");
        
        mAttributes.set(serialisedAttributes);
        
        mBuffer = NULL;
        mSize = 0;
        reset();
    }
    
    void reset()
    {
        FrameLib_DSP::reset();

        long unit = mAttributes.getInt(kUnit);
        double size = mAttributes.getValue(kLength);

        if (unit != kSamples)
        {
            size *= mSamplingRate;
            
            if (unit != kSeconds)
                size /= 1000.0;
        }
       
        size += MAX_VECTOR_SIZE;
        
        if (size != mSize)
        {
            mSize = round(size);
            delete mBuffer;
            mBuffer = new double[mSize];
        }
        
        memset(mBuffer, 0, mSize * sizeof(double));
        
        mCounter = 0;
    }
    
private:
    
    void copyAndZero (double *output, unsigned long offset, unsigned long size)
    {
        if (size)
        {
            memcpy(output, mBuffer + offset, size * sizeof(double));
            memset(mBuffer + offset, 0, size * sizeof(double));
        
            mCounter = offset + size;
        }
    }
    
    void blockProcess (double **ins, double **outs, unsigned long vecSize)
    {
        double *output = outs[0];
        
        // Safety
        
        if (vecSize > mSize)
            return;
        
        // Calculate first segment size and copy segments
        
        unsigned long size = ((mCounter + vecSize) > mSize) ? mSize - mCounter : vecSize;
    
        copyAndZero(output, mCounter, size);
        copyAndZero(output + size, 0, vecSize - size);
    }
    
    void AddToBuffer (double *input, unsigned long offset, unsigned long size)
    {
        for (unsigned long i = 0; i < size; i++)
            mBuffer[i + offset] += input[i];
    }
    
    void process ()
    {
        unsigned long sizeIn;

        FrameLib_TimeFormat inputTime = getInputFrameTime(0);
        double *input = getInput(0, &sizeIn);
        
        // Calculate time offset
        
        unsigned long offset = round(inputTime - getBlockTime());
        
        // Safety
        
        if (!sizeIn || inputTime < getBlockTime() || (offset + sizeIn) > mSize)
            return;
        
        // Calculate actual offset into buffer
        
        offset += mCounter;
        offset = (offset < mSize) ? offset : offset - mSize;

        // Calculate first segment size and copy segments
        
        unsigned long size = ((offset + sizeIn) > mSize) ? mSize - offset : sizeIn;
        
        AddToBuffer(input, offset, size);
        AddToBuffer(input + size, 0, sizeIn - size);
    }

    
private:
    
    double *mBuffer;
    unsigned long mSize;
    unsigned long mCounter;
};

#endif
