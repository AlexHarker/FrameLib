
#include "FrameLib_Lag.h"
#include "FrameLib_PaddedVector.h"

// Constructor

FrameLib_Lag::FrameLib_Lag(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy)
: FrameLib_Processor(context, proxy, &sParamInfo, 3, 2)
, FrameLib_RingBuffer(this)
{
    mParameters.addInt(kMaxFrames, "max_frames", 10, 0);
    mParameters.setMin(1);
    
    mParameters.addInt(kNumFrames, "num_frames", 1, 1);
    mParameters.setMin(0);
    
    mParameters.addDouble(kPadding, "pad", 0.0, 2);
    
    mParameters.addEnum(kStartMode, "start");
    mParameters.addEnumItem(kPad, "pad");
    mParameters.addEnumItem(kShorten, "shorten");
    
    mParameters.set(serialisedParameters);

    setInputMode(1, false, false, false);

    setParameterInput(2);
}

// Info

std::string FrameLib_Lag::objectInfo(bool verbose)
{
    return formatInfo("Delay vector type input by a whole number of frames: "
                      "Frames are expected to be of a uniform length. "
                      "The output is the same length as the input. "
                      "If the input length changes then the internal storage is reset. "
                      "The object can also be reset by sending a frame to the reset input. "
                      "The start parameter controls behaviour when insufficient frames are stored. "
                      "The underlength output indicates that insufficient frames are stored.",
                      "Delay vector type input by a whole number of frames.", verbose);
}

std::string FrameLib_Lag::inputInfo(unsigned long idx, bool verbose)
{
    switch (idx)
    {
        case 0:     return "Input";
        case 1:     return formatInfo("Reset Input - resets and sets padding frame without output", "Reset Input", verbose);
        default:    return parameterInputInfo(verbose);
    }
}

std::string FrameLib_Lag::outputInfo(unsigned long idx, bool verbose)
{
    if (idx)
        return formatInfo("Underlength - indicates insufficient frames are stored", "Underlength", verbose);
    else
        return "Output";
}

// Parameter Info

FrameLib_Lag::ParameterInfo FrameLib_Lag::sParamInfo;

FrameLib_Lag::ParameterInfo::ParameterInfo()
{
    add("Sets the maximum lag value in frames. "
        "Note that the internal buffer is reset when this changes.");
    add("Sets the current lag as an integer.");
    add("Sets the padding value.");
    add("Sets the behaviour when there are insufficient frames stored (as after a reset): "
        "pad - output a frame of default values. "
        "shorten - output the oldest input frame since the reset. "
        "Note that frames at the reset input set the frame used for padding. "
        "If the frame is too short or empty it is padded with the pad parameter.");
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
    unsigned long sizeIn, sizeReset, sizeOut, sizeEdge;

    StartModes startMode = mParameters.getEnum<StartModes>(kStartMode);
    unsigned long maxFrames = mParameters.getInt(kMaxFrames);
    unsigned long requestedFrames = mParameters.getInt(kNumFrames);
    
    const double *input = getInput(0, &sizeIn);
    const double *resetInput = getInput(1, &sizeReset);
    
    const bool forceReset = mLastResetTime != getInputFrameTime(1);
    const bool frameSizeMismatch = sizeIn != getFrameLength();
    const bool maxFramesMismatch = maxFrames != getNumFrames();
    
    if (forceReset || frameSizeMismatch || maxFramesMismatch)
    {
        resize(maxFrames, sizeIn);
        mLastResetTime = getInputFrameTime(1);
    }
    
    // Check available frames
    
    requestedFrames = std::min(requestedFrames, maxFrames);
    unsigned long numFrames = requestedFrames;
    unsigned long validFrames = getValidFrames();
    
    if (startMode == kShorten)
        numFrames = std::min(numFrames, validFrames);

    // Allocate outputs
    
    requestOutputSize(0, getFrameLength());
    requestOutputSize(1, 1);
    allocateOutputs();
    double *outputMain = getOutput(0, &sizeOut);
    double *outputEdge = getOutput(1, &sizeEdge);
    
    // Output
    
    if (numFrames <= validFrames)
    {
        const double *frame = numFrames ? getFrame(numFrames) : input;
        copyVector(outputMain, frame, sizeOut);
    }
    else
    {
        PaddedVector pad(resetInput, sizeReset, mParameters.getValue(kPadding));
        
        for (unsigned long i = 0; i < sizeOut; i++)
            outputMain[i] = pad[i];
    }
    
    if (sizeEdge)
        outputEdge[0] = requestedFrames > validFrames ? 1 : 0;
    
    write(input, sizeIn);
}
