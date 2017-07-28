
#ifndef FRAMELIB_CHOP_H
#define FRAMELIB_CHOP_H

#include "FrameLib_DSP.h"

class FrameLib_Chop : public FrameLib_Processor
{
	enum ParameterList { kNumOuts, kSize, kUnits };

    enum Units { kSamples, kRatio };
    
public:
	
    FrameLib_Chop(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner) : FrameLib_Processor(context)
    {
        mParameters.addInt(kNumOuts, "numouts", 1, 0);
        mParameters.setClip(2, 32);
        mParameters.setInstantiation();
        
        mParameters.addDouble(kSize, "size", 1.0, 1);
        mParameters.setMin(0.0);
        
        mParameters.addEnum(kUnits, "units", 1);
        mParameters.addEnumItem(kSamples, "samples");
        mParameters.addEnumItem(kRatio, "ratios");
        
        mParameters.set(serialisedParameters);
        
        mNumOuts = mParameters.getInt(kNumOuts);
        
        setIO(1, mNumOuts);
    }
    
private:
    
    void process ()
	{
        // Get Input 
        
        unsigned long sizeIn, sizeOut, sizeCalc, chop, i;
        double *input = getInput(0, &sizeIn);
        Units units = (Units) mParameters.getInt(kUnits);
        
        // Calculate chop sizes
        
        if (units == kSamples)
            chop = mParameters.getInt(kSize);
        else
            chop = round(mParameters.getValue(kSize) * sizeIn);
        
        for (i = 0, sizeCalc = sizeIn; i < mNumOuts; i++)
        {
            sizeOut = (chop <= sizeCalc) ? chop : sizeCalc;
            sizeCalc -= sizeOut;
            requestOutputSize(i, sizeOut);
        }
        
        allocateOutputs();
        
        for (i = 0, sizeCalc = 0; i < mNumOuts; i++)
        {
            // Copy to output
            
            double *output = getOutput(i, &sizeOut);
            
            memcpy(output, input + sizeCalc, sizeOut * sizeof(double));
            sizeCalc += sizeOut;
        }
    }
        
    unsigned long mNumOuts;
};

#endif
