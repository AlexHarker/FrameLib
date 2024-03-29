
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
    
    setInputMode(0, false, true, false, FrameType::Any);
    setOutputType(0, FrameType::Any);
}

// Info

std::string FrameLib_Store::objectInfo(bool verbose)
{
    return formatInfo("Stores frames remotely for later recall: "
                      "The storage location is determined by the name parameter. "
                      "The output / ordering input can be used to explicitly control ordering between related objects. "
                      "In a multistream scenario each stream has separate storage for a given name.",
                      "Stores frames remotely for later recall.", verbose);
}

std::string FrameLib_Store::inputInfo(unsigned long idx, bool verbose)
{
    return "Input";
}

std::string FrameLib_Store::outputInfo(unsigned long idx, bool verbose)
{
    return formatInfo("Ordering Output - connect to control ordering", "Ordering Output", verbose);
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
    add("Sets the name of the storage to use. "
        "Note that names are local to the current context.");
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
    
    if (type == FrameType::Tagged)
        size = getInput(0)->size();
    else
        getInput(0, &size);
    
    access.resize(type == FrameType::Tagged, size);
    
    // Prepare and allocate outputs
    
    prepareCopyInputToOutput(0, 0);
    allocateOutputs();
    
    // Copy to storage
    
    if (type == FrameType::Vector)
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
