
#include "FrameLib_Store.h"

// Constructor / Destructor

FrameLib_Store::FrameLib_Store(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner) : FrameLib_Processor(context, &sParamInfo, 2, 1)
{
    mParameters.addString(kName, "name", 0);
    mParameters.setInstantiation();
        
    mParameters.set(serialisedParameters);
    
    mStorage = mAllocator->registerStorage(mParameters.getString(kName));
}

FrameLib_Store::~FrameLib_Store()
{
    mAllocator->releaseStorage(mParameters.getString(kName));
}

// Info

std::string FrameLib_Store::objectInfo(bool verbose)
{
    return getInfo("Stores a vector frame in named memory for recall: The output can be used to control ordering/synchronisation.",
                   "Stores a vector frame in named memory for recall.", verbose);
}

std::string FrameLib_Store::inputInfo(unsigned long idx, bool verbose)
{
    if (idx)
        return getInfo("Synchronisation Input - use to control ordering", "Synchronisation Input", verbose);
    else
        return "Input to Store";
}

std::string FrameLib_Store::outputInfo(unsigned long idx, bool verbose)
{
    return "Synchronisation Output";
}

// Parameter Info

FrameLib_Store::ParameterInfo FrameLib_Store::sParamInfo;

FrameLib_Store::ParameterInfo::ParameterInfo()
{
    add("Sets the name of the memory location to use.");
}

// Object Rest

void FrameLib_Store::objectReset()
{
    mStorage->resize(0);
}

// Process

void FrameLib_Store::process()
{
    // Get Input
    
    unsigned long sizeIn, sizeOut;
    double *input = getInput(0, &sizeIn);
    
    mStorage->resize(sizeIn);
    
    requestOutputSize(0, sizeIn);
    allocateOutputs();
    
    double *output = getOutput(0, &sizeOut);
    
    // Copy to storage and output
    
    if (mStorage->getSize() == sizeOut)
        copyVector(mStorage->getData(), input, sizeOut);
    
    copyVector(output, input, sizeOut);
}
