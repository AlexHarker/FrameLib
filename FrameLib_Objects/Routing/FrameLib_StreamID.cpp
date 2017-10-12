
#include "FrameLib_StreamID.h"

// Constructor

FrameLib_StreamID::FrameLib_StreamID(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner)
: FrameLib_Processor(context, owner, NULL, 1, 1), mStream(1)
{
    mParameters.set(serialisedParameters);
}

// Info

std::string FrameLib_StreamID::objectInfo(bool verbose)
{
    return formatInfo("Outputs the channel number on each channel of a multichannel stream when triggered: Any input frame outputs the channel number (counting from one) on the relevant frame stream.",
                   "Outputs the channel number on each channel of a multichannel stream when triggered.", verbose);
}

std::string FrameLib_StreamID::inputInfo(unsigned long idx, bool verbose)
{
    return formatInfo("Trigger Input - triggers output channel numbers", "Trigger Input", verbose);
}

std::string FrameLib_StreamID::outputInfo(unsigned long idx, bool verbose)
{
    return "Channel Numbers Output";
}

// Stream Awareness

void FrameLib_StreamID::setStream(unsigned long stream)
{
    mStream = stream;
}

// Process

void FrameLib_StreamID::process()
{
    unsigned long size;
    
    requestOutputSize(0, 1);
    allocateOutputs();
    
    double *output = getOutput(0, &size);
    
    if (size)
        output[0] = mStream;
}
