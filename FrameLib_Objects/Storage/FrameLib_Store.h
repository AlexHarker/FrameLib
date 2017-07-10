
#ifndef FRAMELIB_STORE_H
#define FRAMELIB_STORE_H

#include "FrameLib_DSP.h"

// FIX - Threadsafety (is threadsafe for now because is scoped within same thread for now)

class FrameLib_Store : public FrameLib_Processor
{
	enum ParameterList {kName};
    
public:
	
    FrameLib_Store(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner) : FrameLib_Processor(context, 1, 1)
    {
        mParameters.addString(kName, "name", "", 0);
        
        mParameters.set(serialisedParameters);
        
        mStorage = mAllocator->registerStorage(mParameters.getString(kName));
    }
    
    ~FrameLib_Store()
    {
        mAllocator->releaseStorage(mParameters.getString(kName));
    }
    
    
protected:
    
    void process ()
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
    
private:
    
    FrameLib_LocalAllocator::Storage *mStorage;
};

#endif