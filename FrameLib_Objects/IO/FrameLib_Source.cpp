
#include "FrameLib_Source.h"

// Constructor

FrameLib_Source::FrameLib_Source(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy)
: FrameLib_AudioInput(context, proxy, &sParamInfo, 2, 1, 1)
, FrameLib_IO_Helper(static_cast<FrameLib_DSP&>(*this))
{
    double bufferSizeDefault = getBufferSizeDefault(serialisedParameters, 16384, 1);
    
    mParameters.addDouble(kBufferSize, "buffer_size", bufferSizeDefault, 0);
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
    
    mParameters.addEnum(kInterpolation, "interp");
    mParameters.addEnumItem(kNone, "none");
    mParameters.addEnumItem(kLinear, "linear");
    mParameters.addEnumItem(kHermite, "hermite");
    mParameters.addEnumItem(kBSpline, "bspline");
    mParameters.addEnumItem(kLagrange, "lagrange");
    
    mParameters.set(serialisedParameters);
        
    mLength = convertTimeToIntSamples(mParameters.getValue(kLength));
    
    setParameterInput(1);
    
    objectReset();
}

// Info

std::string FrameLib_Source::objectInfo(bool verbose)
{
    return formatInfo("Captures audio from the host environment and outputs the most recent values as frames: "
                      "The size of captured frames is settable with the length parameter. "
                      "The length of the internal buffer determines the maximum frame length. "
                      "There is a minimum latency equivalent to the length of the captured frame. "
                      "A longer delay can be optionally selected (subject to the frame and buffer length). "
                      "Capture can optionally be interpolated (at greater CPU usage) for sub-sample accuracy.",
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
    return "Output";
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
        "Note that a minimum delay (or latency) is applied of the output length.");
    add("Sets the interpolation mode: "
        "none - no interpolation. "
        "linear - linear interpolation. "
        "hermite - cubic hermite interpolation. "
        "bspline - cubic bspline interpolation. "
        "lagrange - cubic lagrange interpolation.");
}

// Helpers

double FrameLib_Source::convertTimeToSamples(double time)
{
    switch (mParameters.getEnum<Units>(kUnits))
    {
        case kSamples:  return time;
        case kMS:       return msToSamples(time);
        case kSeconds:  return secondsToSamples(time);
    }

    assert("This code should never run");
    
    return time;
}

unsigned long FrameLib_Source::convertTimeToIntSamples(double time)
{
    return roundToUInt(convertTimeToSamples(time));
}

void FrameLib_Source::copy(const double *input, unsigned long offset, unsigned long size)
{
    if (size)
    {
        copyVector(&mBuffer[offset], input, size);
        mCounter = offset + size;
        mCounter = mCounter == bufferSize() ? 0 : mCounter;
    }
}

// Object Reset, Block Process, Update and Process

void FrameLib_Source::objectReset()
{
    size_t size = convertTimeToIntSamples(mParameters.getValue(kBufferSize));
    
    // Limit the buffer size ensuring there are enough additional samples for interpolation and the max block size
    
    size = limitBufferSize(size, mSamplingRate) + mMaxBlockSize + 2UL;
    
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
    mLength = convertTimeToIntSamples(mParameters.getValue(kLength));
}

void FrameLib_Source::process()
{
    InterpType interpType = InterpType::None;

    unsigned long sizeOut = mLength;
    
    FrameLib_TimeFormat frameTime = getFrameTime();
    FrameLib_TimeFormat delayTime(convertTimeToSamples(mParameters.getValue(kDelay)));
    delayTime = std::max(FrameLib_TimeFormat(sizeOut, 0), delayTime);
    
    // Calculate output size
    
    requestOutputSize(0, sizeOut);
    allocateOutputs();
    double *output = getOutput(0, &sizeOut);
    
    // Calculate time offset and determine required interpolation
    
    FrameLib_TimeFormat timeOffset = (getBlockEndTime() - frameTime) + delayTime;
    
    if (timeOffset.fracVal())
    {
        switch (mParameters.getEnum<Interpolation>(kInterpolation))
        {
            case kNone:         break;
            case kLinear:       interpType = InterpType::Linear;            break;
            case kHermite:      interpType = InterpType::CubicHermite;      break;
            case kBSpline:      interpType = InterpType::CubicBSpline;      break;
            case kLagrange:     interpType = InterpType::CubicLagrange;     break;
        }
    }
    
    // Safety
    
    bool timeCheck = frameTime >= getBlockStartTime() && frameTime < getBlockEndTime();
    FrameLib_TimeFormat delayCheck = delayTime - FrameLib_TimeFormat(sizeOut, 0);
    
    if (!timeCheck || !checkOutput(sizeOut, delayCheck, bufferSize(), mMaxBlockSize + 2UL))
    {
        zeroVector(output, sizeOut);
        return;
    }
    
    // Choose sample or sub-sample accuracy
    
    if (interpType == InterpType::None)
    {
        // Calculate actual offset into buffer
    
        unsigned long offset = roundToUInt(timeOffset);
        offset = (offset <= mCounter) ? mCounter - offset : mCounter + bufferSize() - offset;
    
        // Calculate first segment size and copy segments
    
        unsigned long size = ((offset + sizeOut) > bufferSize()) ? bufferSize() - offset : sizeOut;
    
        copyVector(output, &mBuffer[offset], size);
        copyVector(output + size, mBuffer.data(), (sizeOut - size));
    }
    else
    {
        double position = static_cast<double>(mCounter) - static_cast<double>(timeOffset);
    
        interpolate(Fetcher(mBuffer.data(), bufferSize()), output, sizeOut, position, interpType);
    }
}
