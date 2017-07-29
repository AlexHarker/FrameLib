
#include "FrameLib_Recall.h"

// Constructor / Destructor

FrameLib_Recall::FrameLib_Recall(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner) : FrameLib_Processor(context, 2, 1)
{
    mParameters.addString(kName, "name", 0);
    
    mParameters.set(serialisedParameters);
    
    mStorage = mAllocator->registerStorage(mParameters.getString(kName));
}

FrameLib_Recall::~FrameLib_Recall()
{
    mAllocator->releaseStorage(mParameters.getString(kName));
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
