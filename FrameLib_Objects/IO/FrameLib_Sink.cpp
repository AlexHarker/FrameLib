
#include "FrameLib_Sink.h"

// Constructor

FrameLib_Sink::FrameLib_Sink(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy)
: FrameLib_AudioOutput(context, proxy, &sParamInfo, 2, 0, 1)
, FrameLib_IO_Helper(static_cast<FrameLib_DSP&>(*this))
, mProxy(castProxy<Proxy>(proxy))
, mClear(false)
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
    
    if (mProxy)
        mProxy->registerObject(this, this, 0);
}

FrameLib_Sink::~FrameLib_Sink()
{
    if (mProxy)
        mProxy->unregisterObject(this, this, 0);
}

// Info

std::string FrameLib_Sink::objectInfo(bool verbose)
{
    return formatInfo("Outputs frames to the host environment as audio by overlap-adding into an output buffer: "
                      "The length of the internal buffer determines the maximum frame length. "
                      "Output suffers no latency but an optional delay may be added if desired. "
                      "If a delay is applied then the maximum frame length will be reduced by the delay time.",
                      "Outputs frames to the host environment as audio by overlap-adding into an output buffer.", verbose);
}

std::string FrameLib_Sink::inputInfo(unsigned long idx, bool verbose)
{
    if (idx)
        return parameterInputInfo(verbose);
    else
        return formatInfo("Input - overlapped to the output", "Input", verbose);
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
    add("Sets the outpu delay in the units specified by the units parameter.");
    add("Sets the interpolation mode: "
        "none - no interpolation. "
        "linear - linear interpolation. "
        "hermite - cubic hermite interpolation (incurs a minimum delay or latency of one sample). "
        "bspline - cubic bspline interpolation (incurs a minimum delay or latency of one sample). "
        "lagrange - cubic lagrange interpolation (incurs a minimum delay or latency of one sample).");
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

    assert("This code should never run");
    
    return time;
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
    // Clear
    
    if (mClear)
    {
        std::fill(mBuffer.begin(), mBuffer.end(), 0.0);
        mClear = false;
    }
    
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
    auto interpIsCubic = [](InterpType type) { return type != InterpType::None && type != InterpType::Linear; };
    
    InterpType interpType = InterpType::None;
    
    unsigned long sizeIn, sizeFrame, offset;
    
    const double *input = getInput(0, &sizeIn);
    const double *frame = input;

    // Determine necessary interpolation
    
    if (sizeIn)
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
    
    FrameLib_TimeFormat frameTime = getFrameTime();
    FrameLib_TimeFormat delayTime(convertTimeToSamples(mParameters.getValue(kDelay)));
    FrameLib_TimeFormat blockStartTime = getBlockStartTime();
    FrameLib_TimeFormat timeOffset = delayTime + frameTime - blockStartTime;
    
    // Safety
    
    if (frameTime < getBlockEndTime())
        return;
    
    if (!checkOutput(sizeIn, delayTime, bufferSize(), mMaxBlockSize + 3UL))
        return;
    
    // Change interpolation to none if not needed and adjust offset for latency if cubic requested
    
    if (!timeOffset.fracVal())
    {
        if (interpIsCubic(interpType) && !delayTime.intVal())
            timeOffset += FrameLib_TimeFormat(1, 0);
        
        interpType = InterpType::None;
    }
    
    // If interpolation is cubic reduce latency if not required (delay is 1 or greater)
    
    bool cubic = interpIsCubic(interpType);
    
    if (cubic && delayTime.intVal())
        timeOffset -= FrameLib_TimeFormat(1, 0);
        
    unsigned long interpSize = interpType != InterpType::None ? (cubic ? 3 : 1) : 0;
    auto interpolated = allocAutoArray<double>(sizeIn + interpSize);

    // Calculate time offset and interpolate if needed

    if (interpType == InterpType::None)
    {
        offset = roundToUInt(timeOffset);
        sizeFrame = sizeIn;
    }
    else
    {
        offset = static_cast<unsigned long>(timeOffset.intVal());
        sizeFrame = sizeIn + interpSize;
        frame = interpolated.data();

        uint64_t interpOffset = cubic ? 1 : 0;
        double position = -static_cast<double>(FrameLib_TimeFormat(interpOffset, timeOffset.fracVal()));
        
        interpolate_zeropad(Fetcher(input, sizeIn), interpolated.data(), sizeFrame, position, interpType);
    }
    
    // Calculate actual offset into buffer
    
    offset += mCounter;
    offset = (offset < bufferSize()) ? offset : offset - bufferSize();
    
    // Calculate first segment size and copy segments
    
    unsigned long size = ((offset + sizeFrame) > bufferSize()) ? bufferSize() - offset : sizeFrame;
    
    addToBuffer(frame, offset, size);
    addToBuffer(frame + size, 0, sizeFrame - size);
}
