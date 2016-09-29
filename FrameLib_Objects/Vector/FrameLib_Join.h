
#ifndef FrameLib_Join_H
#define FrameLib_Join_H

#include "FrameLib_DSP.h"

// FIX - time variable triggers

class FrameLib_Join : public FrameLib_Processor
{
	enum AttributeList {kNumIns, kTriggers};
    
public:
	
    FrameLib_Join(FrameLib_Context context, FrameLib_Attributes::Serial *serialisedAttributes, void *owner) : FrameLib_Processor(context)
    {
        mAttributes.addDouble(kNumIns, "numins", 2.0, 0);
        mAttributes.setClip(2.0, 32.0);
        
        mAttributes.addArray(kTriggers, "triggers", 1.0, 32);
        
        mAttributes.set(serialisedAttributes);
        
        setIO(mAttributes.getInt(kNumIns), 1);
        
        double *triggers = mAttributes.getArray(kTriggers);
        
        // Set up triggers
        
        for (unsigned long i = 0; i < getNumIns(); i++)
            inputMode(i, FALSE, triggers[i], FALSE);
    }
    
protected:
    
    void process ()
	{
        // Get Input 
        
        unsigned long sizeIn;
        unsigned long sizeOut = 0;
        unsigned long offset = 0;

       // Calculate size
        
        for (unsigned long i = 0; i < getNumIns(); i++)
        {
            getInput(i, &sizeIn);
            sizeOut += sizeIn;
        }

        requestOutputSize(0, sizeOut);
        allocateOutputs();
        
        double *output = getOutput(0, &sizeOut);
        
        // Copy to outputs

        if (sizeOut)
        {
            for (unsigned long i = 0; i < getNumIns(); i++)
            {
                double *input = getInput(i, &sizeIn);
                memcpy(output + offset, input, sizeIn * sizeof(double));
                offset += sizeIn;
            }
        }
    }
};

#endif
