
#ifndef FRAMELIB_SUBFRAME_H
#define FRAMELIB_SUBFRAME_H

#include "FrameLib_DSP.h"

class FrameLib_Subframe : public FrameLib_Processor
{
	enum AttributeList {kStart, kEnd, kUnits};

    enum Units {kSamples, kRatio};
    
public:
	
    FrameLib_Subframe(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner) : FrameLib_Processor(context, 1, 1)
    {
        mParameters.addDouble(kStart, "start", 0.0, 0);
        mParameters.setMin(0.0);

        mParameters.addDouble(kEnd, "end", 0.0, 1);
        mParameters.setMin(0.0);
        
        mParameters.addEnum(kUnits, "units", 2);
        mParameters.addEnumItem(kSamples, "samples");
        mParameters.addEnumItem(kRatio, "ratios");
        
        mParameters.set(serialisedParameters);
    }
    
protected:
    
    void process ()
	{
        // Get Input
        
        unsigned long sizeIn, sizeOut;
        double *input = getInput(0, &sizeIn);

        unsigned long start, end;
        
        Units units = (Units) mParameters.getInt(kUnits);
        
        if (units == kSamples)
        {
            start = mParameters.getInt(kStart);
            end = mParameters.getInt(kEnd);
        }
        else
        {
            start = round(mParameters.getValue(kStart) * sizeIn);
            end = round(mParameters.getValue(kEnd) * sizeIn);
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
