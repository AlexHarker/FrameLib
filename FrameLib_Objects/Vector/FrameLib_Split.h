
#ifndef FRAMELIB_SPLIT_H
#define FRAMELIB_SPLIT_H

#include "FrameLib_DSP.h"

// FIX - consider multi-output version later

class FrameLib_Split : public FrameLib_Processor
{
	enum AttributeList {kSplit, kUnits};

    enum Units {kSamples, kRatio};
    
public:
	
    FrameLib_Split(FrameLib_Context context, FrameLib_Attributes::Serial *serialisedAttributes, void *owner) : FrameLib_Processor(context, 1, 2)
    {
        mAttributes.addDouble(kSplit, "split", 0.0, 0);
        mAttributes.setMin(0.0);
        
        mAttributes.addEnum(kUnits, "units", 1);
        mAttributes.addEnumItem(kSamples, "samples");
        mAttributes.addEnumItem(kRatio, "ratios");
        
        mAttributes.set(serialisedAttributes);
    }
    
protected:
    
    void process ()
	{
        // Get Input 
        
        unsigned long sizeIn, sizeOut1, sizeOut2, split;
        double *input = getInput(0, &sizeIn);
        Units units = (Units) mAttributes.getInt(kUnits);
        
        // Calculate split point
        
        if (units == kSamples)
            split = mAttributes.getInt(kSplit);
        else
            split = round(mAttributes.getValue(kSplit) * sizeIn);
        
        split = split > sizeIn ? sizeIn : split;

        requestOutputSize(0, split);
        requestOutputSize(1, sizeIn - split);
        allocateOutputs();
        
        double *output1 = getOutput(0, &sizeOut1);
        double *output2 = getOutput(1, &sizeOut2);
        
        // Copy to outputs
        
        memcpy(output1, input, sizeOut1 * sizeof(double));
        memcpy(output2, input + split, sizeOut2 * sizeof(double));
    }
};

#endif
