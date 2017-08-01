
#include "FrameLib_Source.h"

// FIX - MAX_VECTOR_SIZE hack
// FIX - source is only sample accurate (not subsample) - add a function to interpolate if neceesary
// FIX - allow parameters to change (and check naming and behaviour...)
// FIX - add delay for alignment purposes

#define MAX_VECTOR_SIZE 8192

// Constructor

FrameLib_Source::FrameLib_Source(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner) : FrameLib_AudioInput(context, 2, 1, 1)
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
    
    setParameterInput(1);
    
    objectReset();
}

// Info

std::string FrameLib_Source::objectInfo(bool verbose)
{
    return getInfo("Captures audio from the host environment and outputs the most recent values as frames: The size of captured frames is variable. "
                   "Latency is equivalent to the length of the captured frame. The length of the internal buffer determines the maximum frame length.",
                   "Captures audio from the host environment and outputs the most recent values as frames.", verbose);
}

std::string FrameLib_Source::inputInfo(unsigned long idx, bool verbose)
{
    if (idx)
        return getInfo("Parameter Update - tagged input updates paramaeters", "Parameter Update", verbose);
    else
        return getInfo("Trigger Frame - triggers capture to output", "Trigger Frame", verbose);
}

std::string FrameLib_Source::outputInfo(unsigned long idx, bool verbose)
{
    return "Frame of Captured Values";
}

std::string FrameLib_Source::audioInfo(unsigned long idx, bool verbose)
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
    Units units = (Units) mParameters.getInt(kUnits);
    
    switch (units)
    {
        case kSamples:  break;
        case kMS:       time *= mSamplingRate / 1000.0;     break;
        case kSeconds:  time *= mSamplingRate;              break;
    }
    
    return round(time);
}

void FrameLib_Source::copy(double *input, unsigned long offset, unsigned long size)
{
    if (size)
    {
        copyVector(&mBuffer[offset], input, size);
        mCounter = offset + size;
    }
}

// Object Reset, Block Process and Process

void FrameLib_Source::objectReset()
{
    unsigned long size = convertTimeToSamples(mParameters.getValue(kMaxLength)) + MAX_VECTOR_SIZE;
    
    if (size != mBuffer.size())
        mBuffer.resize(size);
    
    zeroVector(&mBuffer[0], bufferSize());
    
    mCounter = 0;
}

void FrameLib_Source::blockProcess(double **ins, double **outs, unsigned long vecSize)
{    
    // Safety
    
    if (vecSize > bufferSize())
        return;
    
    // Calculate first segment size and copy segments
    
    unsigned long size = ((mCounter + vecSize) > bufferSize()) ? bufferSize() - mCounter : vecSize;
    
    copy(ins[0], mCounter, size);
    copy(ins[0] + size, 0, vecSize - size);
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
    
    if (!sizeOut || (offset + sizeOut) > bufferSize())
        return;
    
    // Calculate actual offset into buffer
    
    offset += sizeOut;
    offset = (offset <= mCounter) ? mCounter - offset : mCounter + bufferSize() - offset;
    
    // Calculate first segment size and copy segments
    
    unsigned long size = ((offset + sizeOut) > bufferSize()) ? bufferSize() - offset : sizeOut;
    
    copyVector(output, &mBuffer[offset], size);
    copyVector(output + size, &mBuffer[0], (sizeOut - size));
}
