

#ifndef FRAMELIB_TRACE_H
#define FRAMELIB_TRACE_H

#include "FrameLib_DSP.h"
#include <cstring>


// FIX - MAX_VECTOR_SIZE hack
// FIX - trace is only sample accurate (not subsample) - double the buffer and add a function to interpolate if neceesary
// FIX - add multichannel later (including multichannel output from one cable - is it possible?)???
// FIX - trace writes whole vectors then traces, would it be better to specify which index to use?

#define MAX_VECTOR_SIZE 8192

class FrameLib_Trace : public FrameLib_AudioProcessor
{
    enum AttributeList {kLength, kUnits};
    enum Units {kMS, kSeconds, kSamples};
    
public:
    
    FrameLib_Trace(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner) : FrameLib_AudioProcessor(context, 1, 0, 0, 1)
    {
        mParameters.addDouble(kLength, "length", 8000, 0);
        mParameters.setMin(0.0);
        mParameters.addEnum(kUnits, "units", 1);
        mParameters.addEnumItem(kMS, "ms");
        mParameters.addEnumItem(kSeconds, "seconds");
        mParameters.addEnumItem(kSamples, "samples");
        
        mParameters.set(serialisedParameters);
        
        mBuffer = NULL;
        mFlags = NULL;
        mSize = 0;
        reset();
    }
    
    ~FrameLib_Trace()
    {
        delete[] mBuffer;
        delete[] mFlags;
    }
    
    void reset()
    {
        FrameLib_DSP::reset();

        long units = mParameters.getInt(kUnits);
        double size = mParameters.getValue(kLength);

        if (units != kSamples)
        {
            size *= mSamplingRate;
            
            if (units != kSeconds)
                size /= 1000.0;
        }
       
        size += MAX_VECTOR_SIZE;
        size = round(size);
        
        if (size != mSize)
        {
            mSize = size;
            delete[] mBuffer;
            delete[] mFlags;
            mBuffer = new double[mSize];
            mFlags = new bool[mSize];
        }
        
        memset(mBuffer, 0, mSize * sizeof(double));
        for (unsigned long i = 0; i < size; i++)
            mFlags[i] = false;

        mLastValue = 0.0;
        mCounter = 0;
    }
    
private:
    
    void copyAndZero(double *output, unsigned long offset, unsigned long size)
    {
        double trace = mLastValue;
        
        if (size)
        {
            for (unsigned long i = 0; i < size; i++)
            {
                output[i] = trace = mFlags[offset + i] ? mBuffer[offset + i] : trace;
                mFlags[offset + i] = false;
            }
            
            mLastValue = trace;
            mCounter = offset + size;
        }
    }
    
    void blockProcessPost(double **ins, double **outs, unsigned long vecSize)
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
    
    void WriteToBuffer(double *input, unsigned long offset, unsigned long size)
    {
        for (unsigned long i = 0; i < size; i++)
        {
            mBuffer[i + offset] = input[i];
            mFlags[i + offset] = true;
        }
    }
    
    void process()
    {
        unsigned long sizeIn;
        
        FrameLib_TimeFormat inputTime = getInputFrameTime(0);
        double *input = getInput(0, &sizeIn);
        
        // Calculate time offset
        
        unsigned long offset = round(inputTime - getBlockStartTime());
        
        // Safety
        
        if (!sizeIn || inputTime < getBlockStartTime() || (offset + sizeIn) > mSize)
            return;
        
        // Calculate actual offset into buffer
        
        offset += mCounter;
        offset = (offset < mSize) ? offset : offset - mSize;
        
        // Calculate first segment size and copy segments
        
        unsigned long size = ((offset + sizeIn) > mSize) ? mSize - offset : sizeIn;
        
        WriteToBuffer(input, offset, size);
        WriteToBuffer(input + size, 0, sizeIn - size);
    }

    
private:
    
    double *mBuffer;
    bool *mFlags;
    double mLastValue;
    unsigned long mSize;
    unsigned long mCounter;
};

#endif
