
#include "FrameLib_ToHost.h"

// FrameLib_ToHost Class

// Constructor

FrameLib_ToHost::FrameLib_ToHost(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy)
: FrameLib_Processor(context, proxy, nullptr, 1, 0)
, mProxy(castProxy<Proxy>(proxy))
, mStreamOwner(this)
, mStream(0)
, mID(0) 
{
    mParameters.set(serialisedParameters);
    
    setInputMode(0, false, true, false, kFrameAny);

    if (mProxy)
        mID = mProxy->registerObject(this, mStreamOwner, mStream);
}

FrameLib_ToHost::~FrameLib_ToHost()
{
    if (mProxy)
        mProxy->unregisterObject(this, mStreamOwner, mStream);
}

// Stream Awareness

void FrameLib_ToHost::setStream(void *streamOwner, unsigned long stream)
{
    if (mProxy)
    {
        mProxy->unregisterObject(this, mStreamOwner, mStream);
        mID = mProxy->registerObject(this, streamOwner, stream);
    }
    
    mStreamOwner = streamOwner;
    mStream = stream;
}

// Info

std::string FrameLib_ToHost::objectInfo(bool verbose)
{
    return formatInfo("Turn frames into host messages or control signals: "
                      "The maximum length of the output message may be limited by the host environment. "
                      "Vector and tagged frame types are both supported if supported by the host.",
                      "Turn frames into host messages or control signals.", verbose);
}

std::string FrameLib_ToHost::inputInfo(unsigned long idx, bool verbose)
{
    return formatInfo("Input - converted to host message", "Input", verbose);
}

std::string FrameLib_ToHost::outputInfo(unsigned long idx, bool verbose)
{
    return "";
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
            
            mProxy->sendToHost(mID, mStream, input, sizeIn, getFrameTime());
        }
        else
        {
            const FrameLib_Parameters::Serial *input = getInput(0);
            
            mProxy->sendToHost(mID, mStream, input, getFrameTime());
        }
    }
}
