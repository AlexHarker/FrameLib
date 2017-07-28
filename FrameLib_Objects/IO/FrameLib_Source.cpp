
#include "FrameLib_Source.h"
#include <algorithm>

// FIX - MAX_VECTOR_SIZE hack
// FIX - source is only sample accurate (not subsample) - add a function to interpolate if neceesary
// FIX - allow parameters to change (and check naming and behaviour...)
// FIX - add delay for alignment purposes

#define MAX_VECTOR_SIZE 8192

// Constructor / Destructor

FrameLib_Source::FrameLib_Source(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner) : FrameLib_AudioInput(context, 1, 1, 1)
{
    mParameters.addDouble(kMaxLength, "length", 16384, 0);
    mParameters.setMin(0.0);
    mParameters.setInstantiation();
    mParameters.addInt(kLength, "size", 4096, 1);
    mParameters.setMin(0);
    mParameters.addEnum(kUnits, "units", 2);
    mParameters.addEnumItem(kSamples, "samples");
    mParameters.addEnumItem(kMS, "ms");
    mParameters.addEnumItem(kSeconds, "seconds");
    
    mParameters.set(serialisedParameters);
    
    mParameters.setInfo(&sParamInfo);
    
    mLength = convertTimeToSamples(mParameters.getValue(kLength));
    
    mBuffer = NULL;
    mSize = 0;
    objectReset();
}

FrameLib_Source::~FrameLib_Source()
{
    delete[] mBuffer;
}

// Info

const char *FrameLib_Source::objectInfo(bool verbose)
{
    return getInfo("Captures audio from the host environment and outputs the most recent values as frames: The size of captured frames is variable. "
                   "Latency is equivalent to the length of the captured frame. The length of the internal buffer determines the maximum frame length.",
                   "Captures audio from the host environment and outputs the most recent values as frames.", verbose);
}

const char *FrameLib_Source::inputInfo(unsigned long idx, bool verbose)
{
    return getInfo("Trigger Frame", "Trigger Frame - triggers capture to output", verbose);
}

const char *FrameLib_Source::outputInfo(unsigned long idx, bool verbose)
{
    return "Frame of Captured Values";
}

const char *FrameLib_Source::audioInfo(unsigned long idx, bool verbose)
{
    return "Audio Input";
}

// Parameter Info

FrameLib_Source::ParameterInfo FrameLib_Source::sParamInfo;

FrameLib_Source::ParameterInfo::ParameterInfo()
{
    add("Sets the internal buffer length in the units specified by the units parameter.");
    add("Sets the size of output frames.");
    add("Sets the time units used to determine the buffer length and output size.");
}

// Helpers

unsigned long FrameLib_Source::convertTimeToSamples(double time)
{
    long units = mParameters.getInt(kUnits);
    
    if (units != kSamples)
    {
        time *= mSamplingRate;
        
        if (units != kSeconds)
            time /= 1000.0;
    }
    
    return time;
}

void FrameLib_Source::copy(double *input, unsigned long offset, unsigned long size)
{
    if (size)
    {
        std::copy(input, input + size, mBuffer + offset);
        mCounter = offset + size;
    }
}

// Object Reset, Block Process and Process

void FrameLib_Source::objectReset()
{
    double size = convertTimeToSamples(mParameters.getValue(kMaxLength)) + MAX_VECTOR_SIZE;
    
    if (size != mSize)
    {
        mSize = round(size);
        delete[] mBuffer;
        mBuffer = new double[mSize];
    }
    
    std::fill_n(mBuffer, mSize, 0.0);
    
    mCounter = 0;
}

void FrameLib_Source::blockProcess(double **ins, double **outs, unsigned long vecSize)
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

void FrameLib_Source::process()
{
    unsigned long sizeOut = mLength;
    
    FrameLib_TimeFormat inputTime = getInputFrameTime(0);
    
    // Calculate output size
    
    requestOutputSize(0, sizeOut);
    allocateOutputs();
    double *output = getOutput(0, &sizeOut);
    
    // Calculate time offset
    
    long offset = round(getBlockEndTime() - inputTime);
    
    // Safety
    
    if (!sizeOut || (offset + sizeOut) > mSize)
        return;
    
    // Calculate actual offset into buffer
    
    offset += sizeOut;
    offset = (offset <= mCounter) ? mCounter - offset : mCounter + mSize - offset;
    
    // Calculate first segment size and copy segments
    
    unsigned long size = ((offset + sizeOut) > mSize) ? mSize - offset : sizeOut;
    
    std::copy(mBuffer + offset, mBuffer + (offset + size), output);
    std::copy(mBuffer, mBuffer + (sizeOut - size), output + size);
}
