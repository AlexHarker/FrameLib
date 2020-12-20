
#include "FrameLib_StreamID.h"

// Constructor

FrameLib_StreamID::FrameLib_StreamID(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy)
: FrameLib_Processor(context, proxy, nullptr, 1, 1)
, mStream(1)
{
    mParameters.set(serialisedParameters);
}

// Info

std::string FrameLib_StreamID::objectInfo(bool verbose)
{
    return formatInfo("Outputs the stream id for each stream in a multistream connection: "
                      "Any input triggers a single value output - the stream id (counting from one).",
                      "Outputs the stream id for each stream in a multistream connection.", verbose);
}

std::string FrameLib_StreamID::inputInfo(unsigned long idx, bool verbose)
{
    return formatInfo("Trigger Input - triggers output", "Trigger Input", verbose);
}

std::string FrameLib_StreamID::outputInfo(unsigned long idx, bool verbose)
{
    return "Output";
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
