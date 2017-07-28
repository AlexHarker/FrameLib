
#include "FrameLib_Trace.h"
#include <algorithm>

// FIX - MAX_VECTOR_SIZE hack
// FIX - trace is only sample accurate (not subsample) - double the buffer and add a function to interpolate if neceesary
// FIX - add multichannel later (including multichannel output from one cable - is it possible?)???
// FIX - trace writes whole vectors then traces, would it be better to specify which index to use?

#define MAX_VECTOR_SIZE 8192

// Constructor / Destructor

FrameLib_Trace::FrameLib_Trace(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner) : FrameLib_AudioOutput(context, 1, 0, 1)
{
    mParameters.addDouble(kLength, "length", 8000, 0);
    mParameters.setMin(0.0);
    mParameters.setInstantiation();
    mParameters.addEnum(kUnits, "units", 1);
    mParameters.addEnumItem(kMS, "ms");
    mParameters.addEnumItem(kSeconds, "seconds");
    mParameters.addEnumItem(kSamples, "samples");
    mParameters.setInstantiation();
    
    mParameters.set(serialisedParameters);
    
    mParameters.setInfo(&sParamInfo);
    
    mBuffer = NULL;
    mFlags = NULL;
    mSize = 0;
    objectReset();
}

FrameLib_Trace::~FrameLib_Trace()
{
    delete[] mBuffer;
    delete[] mFlags;
}

// Info

const char *FrameLib_Trace::objectInfo(bool verbose)
{
    return getInfo("Outputs audio frames to the host environment without overlapping, continuing the final value till a new frame arrives: "
                   "This is intended for tracking control type values. The length of the internal buffer determines the maximum frame length. "
                   "Output suffers no latency.",
                   "Outputs audio frames to the host environment without overlapping, continuing the final value till a new frame arrives.", verbose);
}

const char *FrameLib_Trace::inputInfo(unsigned long idx, bool verbose)
{
    return getInfo("Frames to Output", "Frames to Output - overlapped to the output", verbose);
}

const char *FrameLib_Trace::audioInfo(unsigned long idx, bool verbose)
{
    return "Audio Output";
}

// Parameter Info

FrameLib_Trace::ParameterInfo FrameLib_Trace::sParamInfo;

FrameLib_Trace::ParameterInfo::ParameterInfo()
{
    add("Sets the internal buffer length in the units specified by the units parameter.");
    add("Sets the time units used to determine the buffer length.");
}

// Helpers

void FrameLib_Trace::copyAndZero(double *output, unsigned long offset, unsigned long size)
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

void FrameLib_Trace::writeToBuffer(double *input, unsigned long offset, unsigned long size)
{
    std::copy(input, input + size, mBuffer + offset);
    std::fill_n(mFlags + offset, size, true);
}

// Object Reset, Block Process and Process

void FrameLib_Trace::objectReset()
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
        delete[] mFlags;
        mBuffer = new double[mSize];
        mFlags = new bool[mSize];
    }
    
    std::fill_n(mBuffer, mSize, 0.0);
    std::fill_n(mFlags, mSize, false);
    
    mLastValue = 0.0;
    mCounter = 0;
}

void FrameLib_Trace::blockProcess(double **ins, double **outs, unsigned long vecSize)
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

void FrameLib_Trace::process()
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
    
    writeToBuffer(input, offset, size);
    writeToBuffer(input + size, 0, sizeIn - size);
}
