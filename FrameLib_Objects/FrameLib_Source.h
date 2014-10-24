

#ifndef FRAMELIB_SOURCE_H
#define FRAMELIB_SOURCE_H

#include "FrameLib_DSP.h"
#include <cstring>

// FIX - MAX_VECTOR_SIZE hack
// FIX - source is only sample accurate (not subsample) - add a function to interpolate if neceesary
// FIX - allow attributes to change (and check naming and behaviour...
// FIX - check DSP ordering

#define MAX_VECTOR_SIZE 8192

class FrameLib_Source : public FrameLib_Processor
{
    enum AttributeList {kMaxLength, kLength, kUnits};
    enum Units {kSamples, kMS, kSeconds};
    
private:
    
    unsigned long convertTimeToSamples(double time)
    {
        long units = mAttributes.getInt(kUnits);
        
        if (units != kSamples)
        {
            time *= mSamplingRate;
            
            if (units != kSeconds)
                time /= 1000.0;
        }
        
        return time;
    }
    
public:
    
    FrameLib_Source(DSPQueue *queue, FrameLib_Attributes::Serial *serialisedAttributes) : FrameLib_Processor(queue, 2, 1, 1, 0)
    {
        mAttributes.addDouble(kMaxLength, "length", 16384, 0);
        mAttributes.setMin(0.0);
        mAttributes.addDouble(kLength, "size", 4096, 1);
        mAttributes.setMin(0.0);
        mAttributes.addEnum(kUnits, "unit", 2);
        mAttributes.addEnumItem(kSamples, "samples");
        mAttributes.addEnumItem(kMS, "ms");
        mAttributes.addEnumItem(kSeconds, "seconds");
        
        mAttributes.set(serialisedAttributes);

        mLength = convertTimeToSamples(mAttributes.getValue(kLength));
        
        mBuffer = NULL;
        mSize = 0;
        reset();
    }
    
    void reset()
    {
        FrameLib_DSP::reset();

        double size = convertTimeToSamples(mAttributes.getValue(kMaxLength)) + MAX_VECTOR_SIZE;
        
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
    
    void copy (double *input, unsigned long offset, unsigned long size)
    {
        if (size)
        {
            memcpy(mBuffer + offset, input, size * sizeof(double));
            mCounter = offset + size;
        }
    }
    
    void blockProcess (double **ins, double **outs, unsigned long vecSize)
    {
        double *input = ins[0];
        
        // Safety
        
        if (vecSize > mSize)
            return;
        
        // Calculate first segment size and copy segments
        
        unsigned long size = ((mCounter + vecSize) > mSize) ? mSize - mCounter : vecSize;
    
        copy(input, mCounter, size);
        copy(input + size, 0, vecSize - size);
    }
    
    void process ()
    {
        unsigned long sizeOut = mLength;

        FrameLib_TimeFormat inputTime = getInputFrameTime(0);
        
        // Calculate output size
        
        requestOutputSize(0, sizeOut);
        allocateOutputs();
        double *output = getOutput(0, &sizeOut);
        
        // Calculate time offset
        
        unsigned long offset = round(getBlockTime() - inputTime);
        
        // Safety
        
        if (!sizeOut || (offset + sizeOut) > mSize)
            return;
        
        // Calculate actual offset into buffer
        
        offset += sizeOut;
        offset = (offset <= mCounter) ? mCounter - offset : mCounter + mSize - offset;

        // Calculate first segment size and copy segments
        
        unsigned long size = ((offset + sizeOut) > mSize) ? mSize - offset : sizeOut;
        
        memcpy(output, (mBuffer + offset), size * sizeof(double));
        memcpy((output + size), mBuffer, (sizeOut - size) * sizeof(double));
    }

    
private:
    
    double *mBuffer;
    unsigned long mSize;
    unsigned long mLength;
    unsigned long mCounter;
};

#endif
