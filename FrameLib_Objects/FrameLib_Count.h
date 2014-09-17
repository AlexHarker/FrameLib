
#ifndef FRAMELIB_COUNT_H
#define FRAMELIB_COUNT_H

#include "FrameLib_DSP.h"

class FrameLib_Count : public FrameLib_Processor
{
    enum AttributeList {kLength, kMode};
    enum Modes {kInLength, kRequestedLength};

public:
    
    FrameLib_Count (DSPQueue *queue, FrameLib_Attributes::Serial *serialisedAttributes) : FrameLib_Processor(queue, 1, 1)
    {
        mAttributes.addDouble(kLength, "length", 64.0, 0);
        mAttributes.setMin(0.0);
        mAttributes.addEnum(kMode, "mode", 1);
        mAttributes.addEnumItem(kInLength, "input");
        mAttributes.addEnumItem(kRequestedLength, "requested");
        
        mAttributes.set(serialisedAttributes);
    }
    
protected:
    
    void process()
    {
        unsigned long sizeIn, sizeOut;
        
        getInput(0, &sizeIn);
        
        sizeOut = ((Modes) mAttributes.getInt(kMode)) == kInLength ? sizeIn : mAttributes.getInt(kLength);
        requestOutputSize(0, sizeOut);
        allocateOutputs();
        
        double *output = getOutput(0, &sizeOut);
        
        for (unsigned int i = 0; i < sizeOut; i++)
            output[i] = (double) i;
    }
};

#endif
