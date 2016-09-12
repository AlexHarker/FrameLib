
#ifndef FRAMELIB_CHOP_H
#define FRAMELIB_CHOP_H

#include "FrameLib_DSP.h"

class FrameLib_Chop : public FrameLib_Processor
{
	enum AttributeList {kNumOuts, kSize, kUnits};

    enum Units {kSamples, kRatio};
    
public:
	
    FrameLib_Chop(DSPQueue *queue, FrameLib_Attributes::Serial *serialisedAttributes, void *owner) : FrameLib_Processor(queue)
    {
        mAttributes.addDouble(kNumOuts, "numouts", 1.0, 0);
        mAttributes.setClip(2.0, 32.0);
        
        mAttributes.addDouble(kSize, "size", 1.0, 1);
        mAttributes.setMin(0.0);
        
        mAttributes.addEnum(kUnits, "units", 1);
        mAttributes.addEnumItem(kSamples, "samples");
        mAttributes.addEnumItem(kRatio, "ratios");
        
        mAttributes.set(serialisedAttributes);
        
        mNumOuts = mAttributes.getInt(kNumOuts);
        
        setIO(1, mNumOuts);
    }
    
protected:
    
    void process ()
	{
        // Get Input 
        
        unsigned long sizeIn, sizeOut, sizeCalc, chop, i;
        double *input = getInput(0, &sizeIn);
        Units units = (Units) mAttributes.getInt(kUnits);
        
        // Calculate chop sizes
        
        if (units == kSamples)
            chop = mAttributes.getInt(kSize);
        else
            chop = round(mAttributes.getValue(kSize) * sizeIn);
        
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
    
private:
    
    unsigned long mNumOuts;
};

#endif
