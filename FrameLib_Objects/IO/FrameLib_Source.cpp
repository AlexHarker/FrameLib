
#include "FrameLib_Source.h"

// FIX - source is only sample accurate (not subsample) - add a function to interpolate if necessary

// Constructor

FrameLib_Source::FrameLib_Source(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy)
: FrameLib_AudioInput(context, proxy, &sParamInfo, 2, 1, 1)
{
    // FIX - defaults for when the units are not in samples!
    
    mParameters.addDouble(kBufferSize, "buffer_size", 16384, 0);
    mParameters.setMin(0.0);
    mParameters.setInstantiation();
    
    mParameters.addInt(kLength, "length", 4096, 1);
    mParameters.setMin(0);
    
    mParameters.addEnum(kUnits, "units", 2);
    mParameters.addEnumItem(kSamples, "samples");
    mParameters.addEnumItem(kMS, "ms");
    mParameters.addEnumItem(kSeconds, "seconds");
    
    mParameters.addDouble(kDelay, "delay", 0);
    mParameters.setMin(0);
    
    mParameters.set(serialisedParameters);
        
    mLength = convertTimeToSamples(mParameters.getValue(kLength));
    
    setParameterInput(1);
    
    objectReset();
}

// Info

std::string FrameLib_Source::objectInfo(bool verbose)
{
    return formatInfo("Captures audio from the host environment and outputs the most recent values as frames: "
                      "The size of captured frames is variable. "
                      "Latency is equivalent to the length of the captured frame. "
                      "The length of the internal buffer determines the maximum frame length.",
                      "Captures audio from the host environment and outputs the most recent values as frames.", verbose);
}

std::string FrameLib_Source::inputInfo(unsigned long idx, bool verbose)
{
    if (idx)
        return parameterInputInfo(verbose);
    else
        return formatInfo("Trigger Input - triggers capture to output", "Trigger Input", verbose);
}

std::string FrameLib_Source::outputInfo(unsigned long idx, bool verbose)
{
    return "Captured Output";
}

std::string FrameLib_Source::audioInfo(unsigned long idx, bool verbose)
{
    return "Audio Input";
}

// Parameter Info

FrameLib_Source::ParameterInfo FrameLib_Source::sParamInfo;

FrameLib_Source::ParameterInfo::ParameterInfo()
{
    add("Sets the internal buffer size in the units specified by the units parameter.");
    add("Sets the length of output frames in the units specified by the units parameter.");
    add("Sets the time units used to determine the buffer size and output length.");
    add("Sets the input delay in the units specified by the units parameter: "
        "N.B. - there is a minimum delay or latency of the output length.");
}

// Helpers

unsigned long FrameLib_Source::convertTimeToSamples(double time)
{    
    switch (static_cast<Units>(mParameters.getInt(kUnits)))
    {
        case kSamples:  break;
        case kMS:       time = msToSamples(time);       break;
        case kSeconds:  time = secondsToSamples(time);  break;
    }
    
    return roundToUInt(time);
}

void FrameLib_Source::copy(const double *input, unsigned long offset, unsigned long size)
{
    if (size)
    {
        copyVector(&mBuffer[offset], input, size);
        mCounter = offset + size;
    }
}

// Object Reset, Block Process, Update and Process

void FrameLib_Source::objectReset()
{
    unsigned long size = convertTimeToSamples(mParameters.getValue(kBufferSize)) + mMaxBlockSize;
    
    if (size != bufferSize())
        mBuffer.resize(size);
    
    zeroVector(mBuffer.data(), bufferSize());
    
    mCounter = 0;
}

void FrameLib_Source::blockProcess(const double * const *ins, double **outs, unsigned long blockSize)
{    
    // Safety
    
    if (blockSize > bufferSize())
        return;
    
    // Calculate first segment size and copy segments
    
    unsigned long size = ((mCounter + blockSize) > bufferSize()) ? bufferSize() - mCounter : blockSize;
    
    copy(ins[0], mCounter, size);
    copy(ins[0] + size, 0, blockSize - size);
}

void FrameLib_Source::update()
{
    mLength = convertTimeToSamples(mParameters.getValue(kLength));
}

void FrameLib_Source::process()
{
    unsigned long sizeOut = mLength;
    
    FrameLib_TimeFormat frameTime = getFrameTime();
    unsigned long delayTime = convertTimeToSamples(mParameters.getValue(kDelay));
    delayTime = std::max(sizeOut, delayTime);
    
    // Calculate output size
    
    requestOutputSize(0, sizeOut);
    allocateOutputs();
    double *output = getOutput(0, &sizeOut);
    
    // Calculate time offset
    
    unsigned long offset = roundToUInt(getBlockEndTime() - frameTime) + delayTime;
    
    // Safety
    
    if (!sizeOut || offset > bufferSize())
    {
        zeroVector(output, sizeOut);
        return;
    }
    
    // Calculate actual offset into buffer
    
    offset = (offset <= mCounter) ? mCounter - offset : mCounter + bufferSize() - offset;
    
    // Calculate first segment size and copy segments
    
    unsigned long size = ((offset + sizeOut) > bufferSize()) ? bufferSize() - offset : sizeOut;
    
    copyVector(output, &mBuffer[offset], size);
    copyVector(output + size, mBuffer.data(), (sizeOut - size));
}
