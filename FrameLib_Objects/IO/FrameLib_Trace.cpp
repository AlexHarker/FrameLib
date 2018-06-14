
#include "FrameLib_Trace.h"
#include <algorithm>

// FIX - trace is only sample accurate (not subsample) - double the buffer and add a function to interpolate if neceesary

// Constructor

FrameLib_Trace::FrameLib_Trace(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy) : FrameLib_AudioOutput(context, proxy, &sParamInfo, 2, 0, 1)
{
    mParameters.addEnum(kMode, "mode", 0);
    mParameters.addEnumItem(kFull, "full");
    mParameters.addEnumItem(kFirst, "first");
    mParameters.addEnumItem(kLast, "last");
    mParameters.addEnumItem(kSpecified, "specified");
    mParameters.addEnumItem(kRatio, "ratio");
    
    mParameters.addDouble(kBufferSize, "buffer_size", 250000, 1);
    mParameters.setMin(0.0);
    mParameters.setInstantiation();
    
    mParameters.addEnum(kUnits, "units", 2);
    mParameters.addEnumItem(kSamples, "samples");
    mParameters.addEnumItem(kMS, "ms");
    mParameters.addEnumItem(kSeconds, "seconds");
    mParameters.setInstantiation();
    
    mParameters.addDouble(kPosition, "position", 0.0);
    mParameters.setMin(0.0);
    
    mParameters.addDouble(kDelay, "delay", 0);
    mParameters.setMin(0);
    
    mParameters.set(serialisedParameters);
    
    setParameterInput(1);
    
    objectReset();
}

// Info

std::string FrameLib_Trace::objectInfo(bool verbose)
{
    return formatInfo("Outputs audio frames (or values from the frame) to the host environment without overlapping, continuing the last value till a new frame arrives: "
                   "The mode parameter determines the value(s) that are output. "
                    "This is intended for tracking control type values. The length of the internal buffer determines the maximum frame length. "
                   "Output suffers no latency.",
                   "Outputs audio frames  (or values from the frame) to the host environment without overlapping, continuing the final value till a new frame arrives.", verbose);
}

std::string FrameLib_Trace::inputInfo(unsigned long idx, bool verbose)
{
    if (idx)
        return parameterInputInfo(verbose);
    else
        return formatInfo("Frames to Output - overlapped to the output", "Frames to Output", verbose);
}

std::string FrameLib_Trace::audioInfo(unsigned long idx, bool verbose)
{
    return "Audio Output";
}

// Parameter Info

FrameLib_Trace::ParameterInfo FrameLib_Trace::sParamInfo;

FrameLib_Trace::ParameterInfo::ParameterInfo()
{
    add("Sets the mode used for output: "
        "full - outputs the entire frame in full. "
        "first - output the first sample of the frame only. "
        "last - output the last sample of the frame only. "
        "specified - output the sample specified directly by the position parameter (clipped into the frame size). "
        "ratio - output the sample specified by the position parameter as a ratio to the frame length (clipped into the frame size).");
    add("Sets the internal buffer size in the units specified by the units parameter.");
    add("Sets the time units used to determine the buffer size and delay.");
    add("Sets the position of the output sample in specified mode (in samples) or ratio mode (as a ratio of the position in the frame).");
    add("Sets the delay before output in the units specified by the units parameter.");
}

// Helpers

unsigned long FrameLib_Trace::convertTimeToSamples(double time)
{
    switch (static_cast<Units>(mParameters.getInt(kUnits)))
    {
        case kSamples:  break;
        case kMS:       time *= mSamplingRate / 1000.0;     break;
        case kSeconds:  time *= mSamplingRate;              break;
    }
    
    return round(time);
}

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

void FrameLib_Trace::writeToBuffer(const double *input, unsigned long offset, unsigned long size)
{
    copyVector(&mBuffer[offset], input, size);
    std::fill_n(mFlags.begin() + offset, size, true);
}

// Object Reset, Block Process and Process

void FrameLib_Trace::objectReset()
{
    double size = mParameters.getValue(kBufferSize);
    
    size = round(convertTimeToSamples(size)) + mMaxBlockSize;
    
    if (size != bufferSize())
    {
        mBuffer.resize(size);
        mFlags.resize(size);
    }
    
    zeroVector(mBuffer.data(), bufferSize());
    std::fill_n(mFlags.begin(), bufferSize(), false);
    
    mLastValue = 0.0;
    mCounter = 0;
}

void FrameLib_Trace::blockProcess(const double * const *ins, double **outs, unsigned long blockSize)
{    
    // Safety
    
    if (blockSize > bufferSize())
        return;
    
    // Calculate first segment size and copy segments
    
    unsigned long size = ((mCounter + blockSize) > bufferSize()) ? bufferSize() - mCounter : blockSize;
    
    copyAndZero(outs[0], mCounter, size);
    copyAndZero(outs[0] + size, 0, blockSize - size);
}

void FrameLib_Trace::process()
{
    unsigned long sizeIn;
    double inputOffset = mParameters.getValue(kPosition);
    
    FrameLib_TimeFormat frameTime = getFrameTime();
    FrameLib_TimeFormat delayTime = convertTimeToSamples(mParameters.getValue(kDelay));

    Modes mode = static_cast<Modes>(mParameters.getInt(kMode));
    
    const double *input = getInput(0, &sizeIn);
    unsigned long sizeToWrite = mode != kFull ? std::min(sizeIn, 1UL) : sizeIn;
    
    // Calculate time offset
    
    unsigned long offset = round(delayTime + frameTime - getBlockStartTime());
    
    // Safety
    
    if (!sizeIn || frameTime < getBlockStartTime() || (offset + sizeToWrite) > bufferSize())
        return;
    
    switch (mode)
    {
        case kFull:         inputOffset = 0.0;                  break;
        case kFirst:        inputOffset = 0.0;                  break;
        case kLast:         inputOffset = sizeIn - 1;           break;
        case kSpecified:                                        break;
        case kRatio:        inputOffset *= (sizeIn - 1);        break;
    }

    // Clip to a sensible range
    
    inputOffset = std::max(0.0, std::min(static_cast<double>(sizeIn - 1), inputOffset));
    input += static_cast<unsigned long>(inputOffset);
    
    // Calculate actual offset into buffer
    
    offset += mCounter;
    offset = (offset < bufferSize()) ? offset : offset - bufferSize();
    
    // Calculate first segment size and copy segments
    
    unsigned long size = ((offset + sizeToWrite) > bufferSize()) ? bufferSize() - offset : sizeToWrite;
    
    writeToBuffer(input, offset, size);
    writeToBuffer(input + size, 0, sizeToWrite - size);
}
