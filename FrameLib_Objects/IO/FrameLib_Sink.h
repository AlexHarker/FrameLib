

#ifndef FRAMELIB_SINK_H
#define FRAMELIB_SINK_H

#include "FrameLib_DSP.h"
#include <cstring>

// FIX - MAX_VECTOR_SIZE hack
// FIX - sink is only sample accurate (not subsample) - double the buffer and add a function to interpolate if neceesary
// FIX - add multichannel later (including multichannel output from one cable - is it possible?)

#define MAX_VECTOR_SIZE 8192

class FrameLib_Sink : public FrameLib_AudioOutput, private FrameLib_Info
{
    enum ParameterList { kLength, kUnits };
    enum Units { kMS, kSeconds, kSamples };
    
    struct ParameterInfo : public FrameLib_Parameters::Info
    {
        ParameterInfo()
        {
            add("Sets the internal buffer length in the units specified by the units parameter.");
            add("Sets the time units used to determine the buffer length.");
        }
    };
    
public:
    
    FrameLib_Sink(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner) : FrameLib_AudioOutput(context, 1, 0, 1)
    {
        mParameters.addDouble(kLength, "length", 8000, 0);
        mParameters.setMin(0);
        mParameters.setInstantiation();
        mParameters.addEnum(kUnits, "units", 1);
        mParameters.addEnumItem(kMS, "ms");
        mParameters.addEnumItem(kSeconds, "seconds");
        mParameters.addEnumItem(kSamples, "samples");
        mParameters.setInstantiation();

        mParameters.set(serialisedParameters);
        
        mParameters.setInfo(getParameterInfo());

        mBuffer = NULL;
        mSize = 0;
        objectReset();
    }
    
    ~FrameLib_Sink()
    {
        delete[] mBuffer;
    }
    
    void objectReset()
    {
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
            mBuffer = new double[mSize];
        }
        
        memset(mBuffer, 0, mSize * sizeof(double));
        
        mCounter = 0;
    }
    
    const char *objectInfo(bool verbose)
    {
        return getInfo("Outputs audio frames to the host environment by pasting them into an overlap-add buffer: "
                       "The length of the internal buffer determines the maximum frame length. Output suffers no latency. ",
                       "Outputs audio frames to the host environment by pasting them into an overlap add buffer.", verbose);
    }
    
    const char *inputInfo(unsigned long idx, bool verbose)  { return getInfo("Frames to Output", "Frames to Output - overlapped to the output", verbose); }
    const char *audioInfo(unsigned long idx, bool verbose)  { return "Audio Output"; }
    
private:
    
    void copyAndZero(double *output, unsigned long offset, unsigned long size)
    {
        if (size)
        {
            memcpy(output, mBuffer + offset, size * sizeof(double));
            memset(mBuffer + offset, 0, size * sizeof(double));
        
            mCounter = offset + size;
        }
    }

    void blockProcess(double **ins, double **outs, unsigned long vecSize)
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
    
    void AddToBuffer(double *input, unsigned long offset, unsigned long size)
    {
        for (unsigned long i = 0; i < size; i++)
            mBuffer[i + offset] += input[i];
    }
    
    void process()
    {
        unsigned long sizeIn;

        FrameLib_TimeFormat inputTime = getInputFrameTime(0);
        FrameLib_TimeFormat blockStartTime = getBlockStartTime();
        double *input = getInput(0, &sizeIn);
        
        // Calculate time offset
        
        unsigned long offset = round(inputTime - blockStartTime);
        
        // Safety
        
        if (!sizeIn || inputTime < blockStartTime || (offset + sizeIn) > mSize)
            return;
        
        // Calculate actual offset into buffer
        
        offset += mCounter;
        offset = (offset < mSize) ? offset : offset - mSize;

        // Calculate first segment size and copy segments
        
        unsigned long size = ((offset + sizeIn) > mSize) ? mSize - offset : sizeIn;
        
        AddToBuffer(input, offset, size);
        AddToBuffer(input + size, 0, sizeIn - size);
    }

    ParameterInfo *getParameterInfo()
    {
        static ParameterInfo info;
        return &info;
    }
    
    double *mBuffer;
    unsigned long mSize;
    unsigned long mCounter;
};

#endif
