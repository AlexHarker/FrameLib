
#ifndef FRAMELIB_RANDOM_H
#define FRAMELIB_RANDOM_H

#include "FrameLib_DSP.h"
#include "FrameLib_RandGen.h"

class FrameLib_Random : public FrameLib_Processor
{
    enum AttributeList {kLength, kMode};
    enum Modes {kInLength, kRequestedLength};

public:
    
    FrameLib_Random (FrameLib_Context context, FrameLib_Attributes::Serial *serialisedAttributes, void *owner) : FrameLib_Processor(context, 1, 1)
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
            output[i] = mRandom.randDouble();
    }
    
private:
    
    FrameLib_RandGen mRandom;
};

#endif
