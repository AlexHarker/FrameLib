
#include "FrameLib_Store.h"

// Constructor

FrameLib_Store::FrameLib_Store(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy)
: FrameLib_Processor(context, proxy, &sParamInfo, 1, 1)
{
    mParameters.addString(kName, "name", 0);
    mParameters.setInstantiation();
        
    mParameters.set(serialisedParameters);
    
    enableOrderingConnections();

    mName = mParameters.getString(kName);
    mStorage = registerStorage(mName.c_str());
    
    setInputMode(0, false, true, false, kFrameAny);
    setOutputType(0, kFrameAny);
}

// Info

std::string FrameLib_Store::objectInfo(bool verbose)
{
    return formatInfo("Stores frames remotely for later recall: "
                      "The storage location is determined by the name parameter. "
                      "The output / ordering input can be used to explicitly control ordering between related objects.",
                      "Stores frames remotely for later recall.", verbose);
}

std::string FrameLib_Store::inputInfo(unsigned long idx, bool verbose)
{
    return "Input to Store";
}

std::string FrameLib_Store::outputInfo(unsigned long idx, bool verbose)
{
    return formatInfo("Ordering Output - used to control ordering", "Ordering Output", verbose);;
}

// Stream Awareness

void FrameLib_Store::setStream(void *streamOwner, unsigned long stream)
{
    mStorage = registerStorage(numberedString(mName.c_str(), stream).c_str());
}

// Parameter Info

FrameLib_Store::ParameterInfo FrameLib_Store::sParamInfo;

FrameLib_Store::ParameterInfo::ParameterInfo()
{
    add("Sets the name of the storage to use.");
}

// Object Rest

void FrameLib_Store::objectReset()
{
    FrameLib_ContextAllocator::Storage::Access access(mStorage);

    access.resize(false, 0);
}

// Process

void FrameLib_Store::process()
{
    // Threadsafety
    
    FrameLib_ContextAllocator::Storage::Access access(mStorage);

    // Resize storage
    
    FrameType type = getInputCurrentType(0);
    unsigned long size;
    
    if (type == kFrameTagged)
        size = getInput(0)->size();
    else
        getInput(0, &size);
    
    access.resize(type == kFrameTagged, size);
    
    // Prepare and allocate outputs
    
    prepareCopyInputToOutput(0, 0);
    allocateOutputs();
    
    // Copy to storage
    
    if (type == kFrameNormal)
    {
        const double *input = getInput(0, &size);
        double *storage = access.getVector();
        copyVector(storage, input, std::min(access.getVectorSize(), size));
    }
    else
    {
        FrameLib_Parameters::Serial *storage = access.getTagged();
        if (storage)
            storage->write(getInput(0));
    }
    
    // Copy to output
    
    copyInputToOutput(0, 0);
}
