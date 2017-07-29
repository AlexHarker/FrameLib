
#include "FrameLib_Sink.h"
#include <algorithm>

// FIX - MAX_VECTOR_SIZE hack
// FIX - sink is only sample accurate (not subsample) - double the buffer and add a function to interpolate if neceesary
// FIX - add multichannel later (including multichannel output from one cable - is it possible?)

#define MAX_VECTOR_SIZE 8192

// Constructor

FrameLib_Sink::FrameLib_Sink(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner) : FrameLib_AudioOutput(context, 1, 0, 1)
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
    
    mParameters.setInfo(&sParamInfo);

    objectReset();
}

// Info

const char *FrameLib_Sink::objectInfo(bool verbose)
{
    return getInfo("Outputs audio frames to the host environment by pasting them into an overlap-add buffer: "
                   "The length of the internal buffer determines the maximum frame length. Output suffers no latency. ",
                   "Outputs audio frames to the host environment by pasting them into an overlap add buffer.", verbose);
}

const char *FrameLib_Sink::inputInfo(unsigned long idx, bool verbose)
{
    return getInfo("Frames to Output", "Frames to Output - overlapped to the output", verbose);
}

const char *FrameLib_Sink::FrameLib_Sink::audioInfo(unsigned long idx, bool verbose)
{
    return "Audio Output";
}

// Parameter Info

FrameLib_Sink::ParameterInfo FrameLib_Sink::sParamInfo;

FrameLib_Sink::ParameterInfo::ParameterInfo()
{
    add("Sets the internal buffer length in the units specified by the units parameter.");
    add("Sets the time units used to determine the buffer length.");
}

// Helpers

void FrameLib_Sink::copyAndZero(double *output, unsigned long offset, unsigned long size)
{
    if (size)
    {
        std::copy(mBuffer.begin() + offset, mBuffer.begin() + (offset + size), output);
        std::fill_n(mBuffer.begin() + offset, size, 0.0);
        
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
    
    size = round(size + MAX_VECTOR_SIZE);
    
    if (size != bufferSize())
        mBuffer.resize(size);
    
    std::fill_n(mBuffer.begin(), bufferSize(), 0.0);
    
    mCounter = 0;
}

void FrameLib_Sink::blockProcess(double **ins, double **outs, unsigned long vecSize)
{
    double *output = outs[0];
    
    // Safety
    
    if (vecSize > bufferSize())
        return;
    
    // Calculate first segment size and copy segments
    
    unsigned long size = ((mCounter + vecSize) > bufferSize()) ? bufferSize() - mCounter : vecSize;
    
    copyAndZero(output, mCounter, size);
    copyAndZero(output + size, 0, vecSize - size);
}

void FrameLib_Sink::process()
{
    unsigned long sizeIn;
    
    FrameLib_TimeFormat inputTime = getInputFrameTime(0);
    FrameLib_TimeFormat blockStartTime = getBlockStartTime();
    double *input = getInput(0, &sizeIn);
    
    // Calculate time offset
    
    unsigned long offset = round(inputTime - blockStartTime);
    
    // Safety
    
    if (!sizeIn || inputTime < blockStartTime || (offset + sizeIn) > bufferSize())
        return;
    
    // Calculate actual offset into buffer
    
    offset += mCounter;
    offset = (offset < bufferSize()) ? offset : offset - bufferSize();
    
    // Calculate first segment size and copy segments
    
    unsigned long size = ((offset + sizeIn) > bufferSize()) ? bufferSize() - offset : sizeIn;
    
    addToBuffer(input, offset, size);
    addToBuffer(input + size, 0, sizeIn - size);
}
