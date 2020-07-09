
#include "FrameLib_Lag.h"

// Constructor

FrameLib_Lag::FrameLib_Lag(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy) : FrameLib_Processor(context, proxy, &sParamInfo, 3, 1), FrameLib_RingBuffer(this)
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
    unsigned long numFrames = mParameters.getInt(kNumFrames);
    
    unsigned long sizeIn, sizeOut;
    const double *input = getInput(0, &sizeIn);
    
    bool forceReset = mLastResetTime != getInputFrameTime(1);

    if (forceReset || getFrameLength() != sizeIn || getNumFrames() != maxFrames)
    {
        resize(maxFrames, sizeIn);
        mLastResetTime = getInputFrameTime(1);
    }
    
    numFrames = std::min(numFrames, maxFrames);

    if (mode == kValid)
        numFrames = std::min(numFrames, getValidFrames());

    requestOutputSize(0, getFrameLength());
    allocateOutputs();
    double *output = getOutput(0, &sizeOut);
    
    if (numFrames <= getValidFrames())
    {
        const double *frame = numFrames ? getFrame(numFrames) : input;
        copyVector(output, frame, sizeOut);
    }
    else
        std::fill_n(output, sizeOut, mParameters.getValue(kDefault));
        
    write(input, sizeIn);
}
