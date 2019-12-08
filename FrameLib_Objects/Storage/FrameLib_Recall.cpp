
#include "FrameLib_Recall.h"

// Constructor / Destructor

FrameLib_Recall::FrameLib_Recall(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy) : FrameLib_Processor(context, proxy, &sParamInfo, 1, 1)
{
    mParameters.addString(kName, "name", 0);
    mParameters.setInstantiation();
        
    mParameters.set(serialisedParameters);
    
    enableOrderingConnections();
    
    mName = mParameters.getString(kName);
    mStorage = registerStorage(mName.c_str());
    
    setOutputType(0, kFrameAny);
}

FrameLib_Recall::~FrameLib_Recall()
{
    releaseStorage(mStorage);
}

// Info

std::string FrameLib_Recall::objectInfo(bool verbose)
{
    return formatInfo("Recall a vector frame from named memory for use: The second output can be used to control ordering or synchronisation.",
                   "Recall a vector frame from named memory for use.", verbose);
}

std::string FrameLib_Recall::inputInfo(unsigned long idx, bool verbose)
{
    if (idx)
        return formatInfo("Synchronisation Input - use to control ordering", "Sync Input", verbose);
    else
        return formatInfo("Trigger Input - triggers recall", "Trigger Input", verbose);
}

std::string FrameLib_Recall::outputInfo(unsigned long idx, bool verbose)
{
    return "Frame Output";
}

// Stream Awareness

void FrameLib_Recall::setStream(void *streamOwner, unsigned long stream)
{
    releaseStorage(mStorage);
    mStorage = registerStorage(numberedString(mName.c_str(), stream).c_str());
}

// Parameter Info

FrameLib_Recall::ParameterInfo FrameLib_Recall::sParamInfo;

FrameLib_Recall::ParameterInfo::ParameterInfo()
{
    add("Sets the name of the memory location to use.");
}

// Process

void FrameLib_Recall::process()
{
    // N.B. Ignore input (it is for triggers only)

    // Threadsafety
    
    FrameLib_ContextAllocator::Storage::Access access(mStorage);

    // Get types and size
    
    FrameType requestType = access.getType();
    unsigned long size = 0;

    if (requestType == kFrameNormal)
        size = access.getVectorSize();
    else
        size = access.getTaggedSize();
    
    // Setup outputs
    
    setCurrentOutputType(0, requestType);
    requestOutputSize(0, size);
    allocateOutputs();
    
    // Copy from storage to output
    
    if (getOutputCurrentType(0) == kFrameNormal)
    {
        double *output = getOutput(0, &size);
        copyVector(output, access.getVector(), std::min(access.getVectorSize(), size));
    }
    else
    {
        FrameLib_Parameters::Serial *output = getOutput(0);
        if (output)
            output->write(access.getTagged());
    }
}
