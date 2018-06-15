
#include "FrameLib_StreamID.h"

// Constructor

FrameLib_StreamID::FrameLib_StreamID(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy)
: FrameLib_Processor(context, proxy, nullptr, 1, 1), mStream(1)
{}

// Info

std::string FrameLib_StreamID::objectInfo(bool verbose)
{
    return formatInfo("Outputs consecutively numbered stream ids for each stream in of a multi-stream connection when triggered: Any input frame outputs the stream number (counting from one) on the relevant frame stream.",
                   "Outputs consecutively numbered stream ids for each stream in of a multi-stream connection when triggered.", verbose);
}

std::string FrameLib_StreamID::inputInfo(unsigned long idx, bool verbose)
{
    return formatInfo("Trigger Input - triggers output stream numbers", "Trigger Input", verbose);
}

std::string FrameLib_StreamID::outputInfo(unsigned long idx, bool verbose)
{
    return "Stream Number Output";
}

// Stream Awareness

void FrameLib_StreamID::setStream(void *streamOwner, unsigned long stream)
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
