
#include "FrameLib_GetParam.h"

// Constructor

FrameLib_GetParam::FrameLib_GetParam(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner) :
    FrameLib_Processor(context, owner, NULL, 2, 1), mConnectedObject(NULL)
{
    // Setup IO

    setInputMode(0, false, false, false);
    setOutputMode(0, kFrameTagged);
}

// Info

std::string FrameLib_GetParam::objectInfo(bool verbose)
{
    return formatInfo("Retrieves parameter values from a connected object: "
                   "The output is a tagged frame of all parameter values from the connected object. "
                   "Only the first input triggers output.",
                   "Retrieves parameter values from a connected object.", verbose);
}

std::string FrameLib_GetParam::inputInfo(unsigned long idx, bool verbose)
{
    if (idx)
        return formatInfo("Object Connection - connect to object to query", "Object Connection", verbose);
    else
        return formatInfo("Trigger Input", "Trigger Input", verbose);

}

std::string FrameLib_GetParam::outputInfo(unsigned long idx, bool verbose)
{
    return "Tagged Output Frames";
}

// Object Reset

void FrameLib_GetParam::objectReset()
{
    // FIX - deal with aliasing issues
    
    mConnectedObject = getConnection(1).mObject;
}

// Process

void FrameLib_GetParam::process()
{    
    unsigned long size = 0;
    
    if (mConnectedObject)
        size = FrameLib_Parameters::Serial::calcSize(mConnectedObject->getParameters());
        
    requestOutputSize(0, size);
    allocateOutputs();
    
    FrameLib_Parameters::Serial *output = getOutput(0);

    if (mConnectedObject && output)
        output->write(mConnectedObject->getParameters());
}
