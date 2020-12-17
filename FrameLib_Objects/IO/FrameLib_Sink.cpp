
#include "FrameLib_Sink.h"

// Constructor

FrameLib_Sink::FrameLib_Sink(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy)
: FrameLib_AudioOutput(context, proxy, &sParamInfo, 2, 0, 1)
, FrameLib_IO_Helper(static_cast<FrameLib_DSP&>(*this))
{
    double bufferSizeDefault = getBufferSizeDefault(serialisedParameters, 250000, 5);

    mParameters.addDouble(kBufferSize, "buffer_size", bufferSizeDefault, 0);
    mParameters.setMin(0);
    mParameters.setInstantiation();
    
    mParameters.addEnum(kUnits, "units", 1);
    mParameters.addEnumItem(kSamples, "samples");
    mParameters.addEnumItem(kMS, "ms");
    mParameters.addEnumItem(kSeconds, "seconds");
    mParameters.setInstantiation();
    
    mParameters.addDouble(kDelay, "delay", 0);
    mParameters.setMin(0);
    
    mParameters.addEnum(kInterpolation, "interp");
    mParameters.addEnumItem(kNone, "none");
    mParameters.addEnumItem(kLinear, "linear");
    mParameters.addEnumItem(kHermite, "hermite");
    mParameters.addEnumItem(kBSpline, "bspline");
    mParameters.addEnumItem(kLagrange, "lagrange");
    
    mParameters.set(serialisedParameters);
    
    setParameterInput(1);

    objectReset();
}

// Info

std::string FrameLib_Sink::objectInfo(bool verbose)
{
    return formatInfo("Outputs audio frames to the host environment by pasting them into an overlap-add buffer: "
                      "The length of the internal buffer determines the maximum frame length. "
                      "Output suffers no latency.",
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
    add("Sets the internal buffer size in the units specified by the units parameter.");
    add("Sets the time units used to determine the buffer size and delay.");
    add("Sets the delay before output in the units specified by the units parameter.");
    add("Sets the interpolation mode: "
        "none - no interpolation"
        "linear - linear interpolation. "
        "hermite - cubic hermite interpolation. "
        "bspline - cubic bspline interpolation. "
        "lagrange - cubic lagrange interpolation.");
}

// Helpers

double FrameLib_Sink::convertTimeToSamples(double time)
{
    switch (mParameters.getEnum<Units>(kUnits))
    {
        case kSamples:  return time;
        case kMS:       return msToSamples(time);
        case kSeconds:  return secondsToSamples(time);
    }
}

unsigned long FrameLib_Sink::convertTimeToIntSamples(double time)
{
    return roundToUInt(convertTimeToSamples(time));
}

void FrameLib_Sink::copyAndZero(double *output, unsigned long offset, unsigned long size)
{
    if (size)
    {
        copyVector(output, &mBuffer[offset], size);
        zeroVector(&mBuffer[offset], size);
        
        mCounter = offset + size;
    }
}

void FrameLib_Sink::addToBuffer(const double *input, unsigned long offset, unsigned long size)
{
    for (unsigned long i = 0; i < size; i++)
        mBuffer[i + offset] += input[i];
}

// Object Reset, Block Process and Process

void FrameLib_Sink::objectReset()
{
    size_t size = convertTimeToIntSamples(mParameters.getValue(kBufferSize));
    
    // Limit the buffer size ensuring there are enough additional samples for interpolation and the max block size

    size = limitBufferSize(size, mSamplingRate) + mMaxBlockSize + 3UL;

    if (size != bufferSize())
        mBuffer.resize(size);
    
    zeroVector(mBuffer.data(), bufferSize());
    
    mCounter = 0;
}

void FrameLib_Sink::blockProcess(const double * const *ins, double **outs, unsigned long blockSize)
{    
    // Safety
    
    if (blockSize > bufferSize() || isTimedOut())
    {
        zeroVector(outs[0], blockSize);
        return;
    }
    
    // Calculate first segment size and copy segments
    
    unsigned long size = ((mCounter + blockSize) > bufferSize()) ? bufferSize() - mCounter : blockSize;
    
    copyAndZero(outs[0], mCounter, size);
    copyAndZero(outs[0] + size, 0, blockSize - size);
}

void FrameLib_Sink::process()
{
    InterpType interpType = kInterpNone;
    
    unsigned long sizeIn, sizeFrame, offset;
    
    FrameLib_TimeFormat frameTime = getFrameTime();
    FrameLib_TimeFormat delayTime(convertTimeToSamples(mParameters.getValue(kDelay)));
    FrameLib_TimeFormat blockStartTime = getBlockStartTime();
    FrameLib_TimeFormat timeOffset = delayTime + frameTime - blockStartTime;
    
    const double *input = getInput(0, &sizeIn);
    const double *frame = input;

    // Determine necessary interpolation
    
    if (sizeIn && timeOffset.fracVal())
    {
        switch (mParameters.getEnum<Interpolation>(kInterpolation))
        {
            case kNone:         break;
            case kLinear:       interpType = kInterpLinear;             break;
            case kHermite:      interpType = kInterpCubicHermite;       break;
            case kBSpline:      interpType = kInterpCubicBSpline;       break;
            case kLagrange:     interpType = kInterpCubicLagrange;      break;
        }
    }
    
    bool cubic = interpType != kInterpNone && interpType != kInterpLinear;
    unsigned long interpSize = interpType != kInterpNone ? (cubic ? 3 : 1) : 0;
    auto interpolated = allocAutoArray<double>(sizeIn + interpSize);

    // Calculate time offset and interpolate if needed

    if (interpType == kInterpNone)
    {
        offset = roundToUInt(timeOffset);
        sizeFrame = sizeIn;
    }
    else
    {
        offset = static_cast<unsigned long>(timeOffset.intVal());
        sizeFrame = sizeIn + interpSize;
        frame = interpolated.get();

        uint64_t interpOffset = cubic ? 1 : 0;
        double position = -static_cast<double>(FrameLib_TimeFormat(interpOffset, timeOffset.fracVal()));
        
        interpolate_zeropad(Fetcher(input, sizeIn), interpolated.get(), sizeFrame, position, interpType);
    }

    // Safety
    
    if (!sizeFrame || frameTime < blockStartTime || (offset + sizeFrame) > bufferSize())
        return;
    
    // Calculate actual offset into buffer
    
    offset += mCounter;
    offset = (offset < bufferSize()) ? offset : offset - bufferSize();
    
    // Calculate first segment size and copy segments
    
    unsigned long size = ((offset + sizeFrame) > bufferSize()) ? bufferSize() - offset : sizeFrame;
    
    addToBuffer(frame, offset, size);
    addToBuffer(frame + size, 0, sizeFrame - size);
}
