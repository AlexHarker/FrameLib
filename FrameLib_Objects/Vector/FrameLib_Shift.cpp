
#include "FrameLib_Shift.h"

// Constructor

FrameLib_Shift::FrameLib_Shift(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner) : FrameLib_Processor(context, 1, 1)
{
    mParameters.addDouble(kShift, "shift", 0.0, 0);
    
    mParameters.addDouble(kPadding, "padding", 0.0, 1);
    
    mParameters.addEnum(kMode, "mode", 2);
    mParameters.addEnumItem(kPad, "pad");
    mParameters.addEnumItem(kWrap, "wrap");
    
    mParameters.addEnum(kUnits, "units", 3);
    mParameters.addEnumItem(kSamples, "samples");
    mParameters.addEnumItem(kRatio, "ratios");
    
    mParameters.set(serialisedParameters);
}

// Process

void FrameLib_Shift::process()
{
    // Get Input
    
    long shift;
    unsigned long sizeIn, sizeOut;
    double *input = getInput(0, &sizeIn);
    double padValue = mParameters.getValue(kPadding);
    Units units = (Units) mParameters.getInt(kUnits);
    
    requestOutputSize(0, sizeIn);
    allocateOutputs();
    
    double *output = getOutput(0, &sizeOut);
    
    // Calculate shift amount
    
    if (units == kSamples)
        shift = mParameters.getInt(kShift);
    else
        shift = round(mParameters.getValue(kShift) * sizeIn);
    
    unsigned long absShift = std::abs(shift);
    
    // Copy to output (with padding if necessary)
    
    if (sizeOut)
    {
        if ((Modes) mParameters.getInt(kMode) == kWrap)
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

