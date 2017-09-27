
#include "FrameLib_Recall.h"

// Constructor / Destructor

FrameLib_Recall::FrameLib_Recall(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner) : FrameLib_Processor(context, owner, &sParamInfo, 2, 1)
{
    mParameters.addString(kName, "name", 0);
    mParameters.setInstantiation();
        
    mParameters.set(serialisedParameters);
    
    enableDependencyConnections();
    
    mStorage = mAllocator->registerStorage(mParameters.getString(kName));
}

FrameLib_Recall::~FrameLib_Recall()
{
    mAllocator->releaseStorage(mParameters.getString(kName));
}

// Info

std::string FrameLib_Recall::objectInfo(bool verbose)
{
    return formatInfo("Recall a vector frame from named memory for use: The second output can be used to control ordering/synchronisation.",
                   "Recall a vector frame from named memory for use.", verbose);
}

std::string FrameLib_Recall::inputInfo(unsigned long idx, bool verbose)
{
    if (idx)
        return formatInfo("Synchronisation Input - use to control ordering", "Synchronisation Input", verbose);
    else
        return formatInfo("Trigger Input - triggers recall", "Trigger Input", verbose);
}

std::string FrameLib_Recall::outputInfo(unsigned long idx, bool verbose)
{
    return "Frame Output";
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
    unsigned long sizeOut;
    
    // Ignore input (it is for triggers only)
    
    requestOutputSize(0, mStorage->getSize());
    allocateOutputs();
    
    double *output = getOutput(0, &sizeOut);
    
    copyVector(output, mStorage->getData(), sizeOut);
}
