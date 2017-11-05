
#include "FrameLib_Sink.h"

// FIX - sink is only sample accurate (not subsample) - double the buffer and add a function to interpolate if necessary

// Constructor

FrameLib_Sink::FrameLib_Sink(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner) : FrameLib_AudioOutput(context, owner, &sParamInfo, 2, 0, 1)
{
    mParameters.addDouble(kLength, "length", 8000, 0);
    mParameters.setMin(0);
    mParameters.setInstantiation();
    mParameters.addEnum(kUnits, "units", 1);
    mParameters.addEnumItem(kMS, "ms");
    mParameters.addEnumItem(kSeconds, "seconds");
    mParameters.addEnumItem(kSamples, "samples");
    mParameters.setInstantiation();
    mParameters.addDouble(kDelay, "delay", 0, 2);
    mParameters.setMin(0);
    
    mParameters.set(serialisedParameters);
    
    setParameterInput(1);

    objectReset();
}

// Info

std::string FrameLib_Sink::objectInfo(bool verbose)
{
    return formatInfo("Outputs audio frames to the host environment by pasting them into an overlap-add buffer: "
                   "The length of the internal buffer determines the maximum frame length. Output suffers no latency. ",
                   "Outputs audio frames to the host environment by pasting them into an overlap add buffer.", verbose);
}

std::string FrameLib_Sink::inputInfo(unsigned long idx, bool verbose)
{
    if (idx)
        return parameterInputInfo(verbose);
    else
        return formatInfo("Frames to Output - overlapped to the output", "Frames to Output", verbose);
}

std::string FrameLib_Sink::FrameLib_Sink::audioInfo(unsigned long idx, bool verbose)
{
    return "Audio Output";
}

// Parameter Info

FrameLib_Sink::ParameterInfo FrameLib_Sink::sParamInfo;

FrameLib_Sink::ParameterInfo::ParameterInfo()
{
    add("Sets the internal buffer length in the units specified by the units parameter.");
    add("Sets the time units used to determine the buffer length.");
    add("Sets the delay before output.");
}

// Helpers

void FrameLib_Sink::copyAndZero(double *output, unsigned long offset, unsigned long size)
{
    if (size)
    {
        copyVector(output, &mBuffer[offset], size);
        zeroVector(&mBuffer[offset], size);
        
        mCounter = offset + size;
    }
}

void FrameLib_Sink::addToBuffer(double *input, unsigned long offset, unsigned long size)
{
    for (unsigned long i = 0; i < size; i++)
        mBuffer[i + offset] += input[i];
}

// Object Reset, Block Process and Process

void FrameLib_Sink::objectReset()
{
    Units units = (Units) mParameters.getInt(kUnits);
    double size = mParameters.getValue(kLength);
    
    switch (units)
    {
        case kSamples:  break;
        case kMS:       size *= mSamplingRate / 1000.0;     break;
        case kSeconds:  size *= mSamplingRate;              break;
    }
    
    size = round(size) + mMaxBlockSize;
    
    if (size != bufferSize())
        mBuffer.resize(size);
    
    zeroVector(&mBuffer[0], bufferSize());
    
    mCounter = 0;
}

void FrameLib_Sink::blockProcess(double **ins, double **outs, unsigned long blockSize)
{    
    // Safety
    
    if (blockSize > bufferSize())
        return;
    
    // Calculate first segment size and copy segments
    
    unsigned long size = ((mCounter + blockSize) > bufferSize()) ? bufferSize() - mCounter : blockSize;
    
    copyAndZero(outs[0], mCounter, size);
    copyAndZero(outs[0] + size, 0, blockSize - size);
}

void FrameLib_Sink::process()
{
    unsigned long sizeIn;
    
    FrameLib_TimeFormat frameTime = getFrameTime();
    FrameLib_TimeFormat delayTime = mParameters.getValue(kDelay);
    FrameLib_TimeFormat blockStartTime = getBlockStartTime();
    double *input = getInput(0, &sizeIn);
    
    // Calculate time offset
    
    unsigned long offset = round(delayTime + frameTime - blockStartTime);
    
    // Safety
    
    if (!sizeIn || frameTime < blockStartTime || (offset + sizeIn) > bufferSize())
        return;
    
    // Calculate actual offset into buffer
    
    offset += mCounter;
    offset = (offset < bufferSize()) ? offset : offset - bufferSize();
    
    // Calculate first segment size and copy segments
    
    unsigned long size = ((offset + sizeIn) > bufferSize()) ? bufferSize() - offset : sizeIn;
    
    addToBuffer(input, offset, size);
    addToBuffer(input + size, 0, sizeIn - size);
}
