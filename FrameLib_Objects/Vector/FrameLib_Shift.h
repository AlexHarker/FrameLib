
#ifndef FRAMELIB_SHIFT_H
#define FRAMELIB_SHIFT_H

#include "FrameLib_DSP.h"

class FrameLib_Shift : public FrameLib_Processor
{
	enum AttributeList {kShift, kPadding, kMode, kUnits};

    enum Modes {kPad, kWrap};
    enum Units {kSamples, kRatio};
    
public:
	
    FrameLib_Shift(DSPQueue *queue, FrameLib_Attributes::Serial *serialisedAttributes) : FrameLib_Processor(queue, 1, 1, 0, 0)
    {
        mAttributes.addDouble(kShift, "shift", 0.0, 0);
        
        mAttributes.addDouble(kPadding, "padding", 0.0, 1);
        
        mAttributes.addEnum(kMode, "mode", 2);
        mAttributes.addEnumItem(kPad, "pad");
        mAttributes.addEnumItem(kWrap, "wrap");
        
        mAttributes.addEnum(kUnits, "units", 3);
        mAttributes.addEnumItem(kSamples, "samples");
        mAttributes.addEnumItem(kRatio, "ratios");
        
        mAttributes.set(serialisedAttributes);
    }
    
protected:
    
    void process ()
	{
        // Get Input
        
        long shift;
        unsigned long sizeIn, sizeOut;
        double *input = getInput(0, &sizeIn);
        double padValue = mAttributes.getValue(kPadding);
        Units units = (Units) mAttributes.getInt(kUnits);
        
        requestOutputSize(0, sizeIn);
        allocateOutputs();
        
        double *output = getOutput(0, &sizeOut);
        
        // Calculate shift amount
        
        if (units == kSamples)
            shift = mAttributes.getInt(kShift);
        else
            shift = round(mAttributes.getValue(kShift) * sizeIn);
        
        unsigned long absShift = abs(shift);
        
        // Copy to output (with padding if necessary)
        
        if (sizeOut)
        {
            if ((Modes) mAttributes.getInt(kMode) == kWrap)
            {
                // Wrap in case of large shift sizes
                
                absShift = absShift % sizeIn;
                
                if (shift >= 0)
                {
                    memcpy(output, input + (sizeIn - absShift), absShift * sizeof(double));
                    memcpy((output + absShift), input, (sizeIn - absShift) * sizeof(double));
                }
                else
                {
                    memcpy(output, input + absShift, (sizeIn - absShift) * sizeof(double));
                    memcpy((output + (sizeIn - absShift)), input, absShift * sizeof(double));
                }
            }
            else
            {
                // Limit shift to maximum (all padding - doesn't matter which direction)
                
                shift = (absShift >= sizeIn) ? sizeIn : shift;
                
                if (shift >= 0)
                {
                    for (unsigned long i = 0; i < absShift; i++)
                        output[i] = padValue;
                    memcpy((output + absShift), input, (sizeIn - absShift) * sizeof(double));
                }
                else
                {
                    memcpy(output, input + absShift, (sizeIn - absShift) * sizeof(double));
                    for (unsigned long i = (sizeIn - absShift); i < sizeIn; i++)
                        output[i] = padValue;
                }   
            }
        }
    }
};

#endif
