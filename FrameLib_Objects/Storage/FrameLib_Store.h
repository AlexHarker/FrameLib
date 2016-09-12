
#ifndef FRAMELIB_STORE_H
#define FRAMELIB_STORE_H

#include "FrameLib_DSP.h"

// FIX - Threadsafety (is threadsafe for now because is scoped within same thread for now)

class FrameLib_Store : public FrameLib_Processor
{
	enum AttributeList {kName};
    
public:
	
    FrameLib_Store(DSPQueue *queue, FrameLib_Attributes::Serial *serialisedAttributes, void *owner) : FrameLib_Processor(queue, 1, 1)
    {
        mAttributes.addString(kName, "name", "", 0);
        
        mAttributes.set(serialisedAttributes);
        
        mStorage = mAllocator->registerStorage(mAttributes.getString(kName));
    }
    
    ~FrameLib_Store()
    {
        mAllocator->releaseStorage(mAttributes.getString(kName));
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
    
    FrameLib_Local_Allocator::Storage *mStorage;
};

#endif