
#ifndef FRAMELIB_REGISTER_H
#define FRAMELIB_REGISTER_H

#include "FrameLib_DSP.h"

class FrameLib_Register : public FrameLib_Processor
{
	enum AttributeList {kMode};

    enum Modes {kStore, kPass};
    
public:
	
    FrameLib_Register(DSPQueue *queue, FrameLib_Attributes::Serial *serialisedAttributes) : FrameLib_Processor(queue, 2, 1, 0, 0)
    {
        mAttributes.addEnum(kMode, "mode", 0);
        mAttributes.addEnumItem(kStore, "store");
        mAttributes.addEnumItem(kPass, "pass");
        
        mAttributes.set(serialisedAttributes);
        
        Modes mode = (Modes) mAttributes.getInt(kMode);
        
        if (mode == kStore)
            inputMode(1, FALSE, FALSE, FALSE);
        else
            inputMode(1, FALSE, TRUE, FALSE);
    }
    
protected:
    
    void process ()
	{
        // Get Register Input (we can ignore the first input as it is just a trigger)
        
        unsigned long sizeIn, sizeOut;
        double *input = getInput(1, &sizeIn);

        requestOutputSize(0, sizeIn);
        allocateOutputs();
        
        double *output = getOutput(0, &sizeOut);
        
        // Copy to output
        
        memcpy(output, input, sizeOut * sizeof(double));
    }
};

#endif