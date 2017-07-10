
#ifndef FRAMELIB_PAD_H
#define FRAMELIB_PAD_H

#include "FrameLib_DSP.h"

class FrameLib_Pad : public FrameLib_Processor
{
	enum ParameterList {kPadding, kStart, kEnd, kUnits};

    enum Units {kSamples, kRatio};
    
public:
	
    FrameLib_Pad(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner) : FrameLib_Processor(context, 1, 1)
    {
        mParameters.addDouble(kPadding, "padding", 0.0, 0);
        
        mParameters.addDouble(kStart, "start", 0.0, 1);
        mParameters.setMin(0.0);

        mParameters.addDouble(kEnd, "end", 0.0, 2);
        mParameters.setMin(0.0);
        
        mParameters.addEnum(kUnits, "units", 3);
        mParameters.addEnumItem(kSamples, "samples");
        mParameters.addEnumItem(kRatio, "ratios");
        
        mParameters.set(serialisedParameters);
    }
    
protected:
    
    void process ()
	{
        // Get Input
        
        unsigned long sizeIn, sizeOut, padStart, padEnd;
        double *input = getInput(0, &sizeIn);
        double padValue = mParameters.getValue(kPadding);
        Units units = (Units) mParameters.getInt(kUnits);
        
        // Calculate pad amounts
        
        if (units == kSamples)
        {
            padStart = mParameters.getInt(kStart);
            padEnd = mParameters.getInt(kEnd);
        }
        else
        {
            padStart = round(mParameters.getValue(kStart) * sizeIn);
            padEnd = round(mParameters.getValue(kEnd) * sizeIn);
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
