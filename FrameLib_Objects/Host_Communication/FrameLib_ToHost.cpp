
#include "FrameLib_ToHost.h"

// FrameLib_ToHost Class

// Constructor

FrameLib_ToHost::FrameLib_ToHost(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy) : FrameLib_Processor(context, proxy, NULL, 1, 0), mProxy(dynamic_cast<Proxy *>(proxy))
{
    mParameters.set(serialisedParameters);
    
    setInputMode(0, false, true, false, kFrameAny);

    if (mProxy)
        mProxy->mObject = this;
}

// Info

std::string FrameLib_ToHost::objectInfo(bool verbose)
{
    return formatInfo("Turn frames into host messages. The maximum length of the output message may be limited by the host environment",
                      "Turn frames into host messages.", verbose);
}

std::string FrameLib_ToHost::inputInfo(unsigned long idx, bool verbose)
{
    return formatInfo("Input Frames - converted to host message", "Input Frames", verbose);
}

std::string FrameLib_ToHost::outputInfo(unsigned long idx, bool verbose)
{
    return "Host Output Messages";
}

// Process

void FrameLib_ToHost::process()
{
    if (mProxy)
    {
        if (getInputCurrentType(0) == kFrameNormal)
        {
            unsigned long sizeIn;
            const double *input = getInput(0, &sizeIn);
            
            mProxy->output(input, sizeIn);
        }
        else
        {
            const FrameLib_Parameters::Serial *input = getInput(0);
            
            mProxy->output(input);
        }
    }
}
