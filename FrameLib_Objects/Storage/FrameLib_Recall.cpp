
#include "FrameLib_Recall.h"

// Constructor

FrameLib_Recall::FrameLib_Recall(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy)
: FrameLib_Processor(context, proxy, &sParamInfo, 1, 1)
{
    mParameters.addString(kName, "name", 0);
    mParameters.setInstantiation();
        
    mParameters.set(serialisedParameters);
    
    enableOrderingConnections();
    
    mName = mParameters.getString(kName);
    mStorage = registerStorage(mName.c_str());
    
    setOutputType(0, FrameType::Any);
}

// Info

std::string FrameLib_Recall::objectInfo(bool verbose)
{
    return formatInfo("Recall a remotely stored frame for use: "
                      "The frame to be recalled is determined by the storage specifed by the name parameter. "
                      "The ordering input can be used to explicitly control ordering between related objects. "
                      "In a multistream scenario each stream has separate storage for a given name.",
                      "Recall a remotely stored frame for use.", verbose);
}

std::string FrameLib_Recall::inputInfo(unsigned long idx, bool verbose)
{
    return formatInfo("Trigger Input - triggers recall", "Trigger Input", verbose);
}

std::string FrameLib_Recall::outputInfo(unsigned long idx, bool verbose)
{
    return "Output";
}

// Stream Awareness

void FrameLib_Recall::setStream(void *streamOwner, unsigned long stream)
{
    mStorage = registerStorage(numberedString(mName.c_str(), stream).c_str());
}

// Parameter Info

FrameLib_Recall::ParameterInfo FrameLib_Recall::sParamInfo;

FrameLib_Recall::ParameterInfo::ParameterInfo()
{
    add("Sets the name of the storage to use. "
        "Note that names are local to the current context.");
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

    if (requestType == FrameType::Vector)
        size = access.getVectorSize();
    else
        size = access.getTaggedSize();
    
    // Setup outputs
    
    setCurrentOutputType(0, requestType);
    requestOutputSize(0, size);
    allocateOutputs();
    
    // Copy from storage to output
    
    if (getOutputCurrentType(0) == FrameType::Vector)
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
