
#ifndef FRAMELIB_SUBFRAME_H
#define FRAMELIB_SUBFRAME_H

#include "FrameLib_DSP.h"

class FrameLib_Subframe : public FrameLib_Processor
{
	enum AttributeList {kStart, kEnd, kUnits};

    enum Units {kSamples, kRatio};
    
public:
	
    FrameLib_Subframe(DSPQueue *queue, FrameLib_Attributes::Serial *serialisedAttributes) : FrameLib_Processor(queue, 1, 1, 0, 0)
    {
        mAttributes.addDouble(kStart, "start", 0.0, 0);
        mAttributes.setMin(0.0);

        mAttributes.addDouble(kEnd, "end", 0.0, 1);
        mAttributes.setMin(0.0);
        
        mAttributes.addEnum(kUnits, "units", 2);
        mAttributes.addEnumItem(kSamples, "samples");
        mAttributes.addEnumItem(kRatio, "ratios");
        
        mAttributes.set(serialisedAttributes);
    }
    
protected:
    
    void process ()
	{
        // Get Input
        
        unsigned long sizeIn, sizeOut;
        double *input = getInput(0, &sizeIn);

        unsigned long start, end;
        
        Units units = (Units) mAttributes.getInt(kUnits);
        
        if (units == kSamples)
        {
            start = mAttributes.getInt(kStart);
            end = mAttributes.getInt(kEnd);
        }
        else
        {
            start = round(mAttributes.getValue(kStart) * sizeIn);
            end = round(mAttributes.getValue(kEnd) * sizeIn);
        }
        
        start = start > sizeIn ? sizeIn : start;
        end = end > sizeIn ? sizeIn : end;
        end = end < start ? start : end;
        
        requestOutputSize(0, end - start);
        allocateOutputs();
        
        double *output = getOutput(0, &sizeOut);
        
        // Copy to output
        
        memcpy(output, input + start, sizeOut * sizeof(double));
    }
};

#endif
