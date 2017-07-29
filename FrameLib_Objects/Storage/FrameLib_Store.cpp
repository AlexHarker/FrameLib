
#include "FrameLib_Store.h"

// Constructor / Destructor

FrameLib_Store::FrameLib_Store(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner) : FrameLib_Processor(context, 2, 1)
{
    mParameters.addString(kName, "name", 0);
    
    mParameters.set(serialisedParameters);
    
    mStorage = mAllocator->registerStorage(mParameters.getString(kName));
}

FrameLib_Store::~FrameLib_Store()
{
    mAllocator->releaseStorage(mParameters.getString(kName));
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
        memcpy(mStorage->getData(), input, sizeOut * sizeof(double));
    
    memcpy(output, input, sizeOut * sizeof(double));
}
