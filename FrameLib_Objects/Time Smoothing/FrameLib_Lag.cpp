
#include "FrameLib_Lag.h"

FrameLib_Lag::FrameLib_Lag(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner) : FrameLib_Processor(context, owner, &sParamInfo, 2, 1), FrameLib_RingBuffer(context)
{
    mParameters.addInt(kMaxFrames, "max_frames", 10, 0);
    mParameters.setMin(1);
    serialisedParameters->read(&mParameters);
    
    mParameters.addInt(kNumFrames, "num_frames", 1, 1);
    mParameters.setMin(0);
    serialisedParameters->read(&mParameters);
    
    setParameterInput(1);
}

// Info

std::string FrameLib_Lag::objectInfo(bool verbose)
{
    return formatInfo("Outputs vector-type frames with a lag set as an integer number of frames: Frames are expected to be of a uniform size, else the buffer is reset. The output is the same size as the input.",
                   "Outputs vector-type frames with a lag set as an integer number of frames.", verbose);
}

std::string FrameLib_Lag::inputInfo(unsigned long idx, bool verbose)
{
    if (idx)
        return parameterInputInfo(verbose);
    else
        return formatInfo("Frames to Lag - output depends on previous inputs", "Frames to Lag", verbose);
}

std::string FrameLib_Lag::outputInfo(unsigned long idx, bool verbose)
{
    return "Output Values";
}

// Parameter Info

FrameLib_Lag::ParameterInfo FrameLib_Lag::sParamInfo;

FrameLib_Lag::ParameterInfo::ParameterInfo()
{
    add("Sets the maximum number of frames that can be set as a lag time.");
    add("Sets the current lag as an integer number of frames.");
}

// Process

void FrameLib_Lag::process()
{
    unsigned long maxFrames = mParameters.getInt(kMaxFrames);
    unsigned long numFrames = mParameters.getInt(kNumFrames);
    
    numFrames = numFrames > maxFrames ? maxFrames : numFrames;
    
    unsigned long sizeIn, sizeOut;
    const double *input = getInput(0, &sizeIn);
    
    if (getFrameLength() != sizeIn || getNumFrames() != maxFrames)
        resize(maxFrames, sizeIn);
    
    requestOutputSize(0, getFrameLength());
    allocateOutputs();
    double *output = getOutput(0, &sizeOut);
    
    const double *frame = numFrames ? getFrame(numFrames) : input;

    copyVector(output, frame, sizeOut);
    write(input, sizeIn);
}
