
#ifndef FRAMELIB_PAD_H
#define FRAMELIB_PAD_H

#include "FrameLib_DSP.h"

class FrameLib_Pad : public FrameLib_Processor
{
	enum AttributeList {kPadding, kStart, kEnd, kUnits};

    enum Units {kSamples, kRatio};
    
public:
	
    FrameLib_Pad(DSPQueue *queue, FrameLib_Attributes::Serial *serialisedAttributes, void *owner) : FrameLib_Processor(queue, 1, 1)
    {
        mAttributes.addDouble(kPadding, "padding", 0.0, 0);
        
        mAttributes.addDouble(kStart, "start", 0.0, 1);
        mAttributes.setMin(0.0);

        mAttributes.addDouble(kEnd, "end", 0.0, 2);
        mAttributes.setMin(0.0);
        
        mAttributes.addEnum(kUnits, "units", 3);
        mAttributes.addEnumItem(kSamples, "samples");
        mAttributes.addEnumItem(kRatio, "ratios");
        
        mAttributes.set(serialisedAttributes);
    }
    
protected:
    
    void process ()
	{
        // Get Input
        
        unsigned long sizeIn, sizeOut, padStart, padEnd;
        double *input = getInput(0, &sizeIn);
        double padValue = mAttributes.getValue(kPadding);
        Units units = (Units) mAttributes.getInt(kUnits);
        
        // Calculate pad amounts
        
        if (units == kSamples)
        {
            padStart = mAttributes.getInt(kStart);
            padEnd = mAttributes.getInt(kEnd);
        }
        else
        {
            padStart = round(mAttributes.getValue(kStart) * sizeIn);
            padEnd = round(mAttributes.getValue(kEnd) * sizeIn);
        }
        
        requestOutputSize(0, padStart + sizeIn + padEnd);
        allocateOutputs();
        
        double *output = getOutput(0, &sizeOut);
        
        // Copy to output with padding
        
        if (sizeOut)
        {
            for (unsigned long i = 0; i < padStart; i++)
                output[i] = padValue;
           
            memcpy((output + padStart), input, sizeIn * sizeof(double));
            
            for (unsigned long i = padStart + sizeIn; i < (padStart + sizeIn + padEnd); i++)
                output[i] = padValue;
        }
    }
};

#endif
