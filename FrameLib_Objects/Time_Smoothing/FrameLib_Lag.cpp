
#include "FrameLib_Lag.h"
#include "FrameLib_PaddedVector.h"

// Constructor

FrameLib_Lag::FrameLib_Lag(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy) : FrameLib_Processor(context, proxy, &sParamInfo, 3, 2), FrameLib_RingBuffer(this)
{
    mParameters.addInt(kMaxFrames, "max_frames", 10, 0);
    mParameters.setMin(1);
    
    mParameters.addInt(kNumFrames, "num_frames", 1, 1);
    mParameters.setMin(0);
    
    mParameters.addDouble(kDefault, "default", 0.0, 2);
    
    mParameters.addEnum(kMode, "mode");
    mParameters.addEnumItem(kDefault, "default");
    mParameters.addEnumItem(kValid, "valid");
    
    mParameters.set(serialisedParameters);

    setInputMode(1, false, false, false);

    setParameterInput(2);
}

// Info

std::string FrameLib_Lag::objectInfo(bool verbose)
{
    return formatInfo("Outputs vector-type frames with a lag set as an integer number of frames: Frames are expected to be of a uniform size, else the buffer is reset. The output is the same size as the input.",
                   "Outputs vector-type frames with a lag set as an integer number of frames.", verbose);
}

std::string FrameLib_Lag::inputInfo(unsigned long idx, bool verbose)
{
    switch (idx)
    {
        case 0:     return "Input";
        case 1:     return "Reset Input";
        default:    return parameterInputInfo(verbose);
    }
}

std::string FrameLib_Lag::outputInfo(unsigned long idx, bool verbose)
{
    if (idx)
        return "Buffer Full";
    else
        return "Output";
}

// Parameter Info

FrameLib_Lag::ParameterInfo FrameLib_Lag::sParamInfo;

FrameLib_Lag::ParameterInfo::ParameterInfo()
{
    add("Sets the maximum number of frames that can be set as a lag time.");
    add("Sets the current lag as an integer number of frames.");
    add("Sets the default output value.");
    add("Sets the mode used when there are insufficient frames stored: "
        "default - output a frame of default values. "
        "valid - output the oldest valid frame.");
}

// Object Reset

void FrameLib_Lag::objectReset()
{
    FrameLib_RingBuffer::reset();
    mLastResetTime = FrameLib_TimeFormat(0);
}

// Process

void FrameLib_Lag::process()
{
    Modes mode = static_cast<Modes>(mParameters.getInt(kMode));
    
    unsigned long maxFrames = mParameters.getInt(kMaxFrames);
    unsigned long requestedFrames = mParameters.getInt(kNumFrames);
    
    unsigned long sizeIn, sizeReset, sizeOut, sizeValid;
    const double *input = getInput(0, &sizeIn);
    const double *resetInput = getInput(1, &sizeReset);
    
    bool forceReset = mLastResetTime != getInputFrameTime(1);

    if (forceReset || getFrameLength() != sizeIn || getNumFrames() != maxFrames)
    {
        resize(maxFrames, sizeIn);
        mLastResetTime = getInputFrameTime(1);
    }
    
    requestedFrames = std::min(requestedFrames, maxFrames);
    unsigned long numFrames = requestedFrames;
    unsigned long validFrames = getValidFrames();
    
    if (mode == kValid)
        numFrames = std::min(numFrames, validFrames);

    requestOutputSize(0, getFrameLength());
    requestOutputSize(1, 1);
    allocateOutputs();
    double *outputValue = getOutput(0, &sizeOut);
    double *outputValid = getOutput(1, &sizeValid);
    
    if (numFrames <= validFrames)
    {
        const double *frame = numFrames ? getFrame(numFrames) : input;
        copyVector(outputValue, frame, sizeOut);
    }
    else
    {
        PaddedVector pad(resetInput, sizeReset, mParameters.getValue(kDefault));
        
        for (unsigned long i = 0; i < sizeOut; i++)
            outputValue[i] = pad[i];
    }
    
    if (sizeValid)
        outputValid[0] = requestedFrames > validFrames ? 1 : 0;
    
    write(input, sizeIn);
}
