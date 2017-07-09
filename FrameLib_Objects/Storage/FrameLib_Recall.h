
#ifndef FRAMELIB_RECALL_H
#define FRAMELIB_RECALL_H

#include "FrameLib_DSP.h"

// FIX - Threadsafety (is threadsafe for now because is scoped within same thread for now)

class FrameLib_Recall : public FrameLib_Processor
{
    enum ParameterList {kName};
    
public:
    
    FrameLib_Recall(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner) : FrameLib_Processor(context, 1, 1)
    {
        mParameters.addString(kName, "name", "", 0);
        
        mParameters.set(serialisedParameters);
        
        mStorage = mAllocator->registerStorage(mParameters.getString(kName));
    }
    
    ~FrameLib_Recall()
    {
        mAllocator->releaseStorage(mParameters.getString(kName));
    }
    
    
protected:
    
    void process ()
    {
        unsigned long sizeOut;
        
        // Ignore input (it is for triggers only)

        requestOutputSize(0, mStorage->getSize());
        allocateOutputs();
            
        double *output = getOutput(0, &sizeOut);

        memcpy(output, mStorage->getData(), sizeOut * sizeof(double));
    }
    
private:
    
    FrameLib_LocalAllocator::Storage *mStorage;
};

#endif
