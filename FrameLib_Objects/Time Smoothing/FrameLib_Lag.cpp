
#include "FrameLib_Lag.h"

FrameLib_Lag::FrameLib_Lag(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner) : FrameLib_Processor(context, owner, &sParamInfo, 2, 1), FrameLib_RingBuffer(context)
{
    mParameters.addInt(kMaxFrames, "max_frames", 10, 0);
    mParameters.setMin(1);
    serialisedParameters->read(&mParameters);
    
    mParameters.addInt(kNumFrames, "num_frames", 10, 1);
    mParameters.setMin(0);
    serialisedParameters->read(&mParameters);
    
    setParameterInput(1);
}

// Info

std::string FrameLib_Lag::objectInfo(bool verbose)
{
    return formatInfo("Outputs the current time: Time is reported in the specified units. Output is a single value.",
                   "Outputs the current time.", verbose);
}

std::string FrameLib_Lag::inputInfo(unsigned long idx, bool verbose)
{
    return formatInfo("Trigger Input - input frames generate output", "Trigger Input", verbose);
}

std::string FrameLib_Lag::outputInfo(unsigned long idx, bool verbose)
{
    return "Output Values";
}

// Parameter Info

FrameLib_Lag::ParameterInfo FrameLib_Lag::sParamInfo;

FrameLib_Lag::ParameterInfo::ParameterInfo()
{
    add("Sets the time units used to for output.");
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
