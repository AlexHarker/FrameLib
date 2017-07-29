
#include "FrameLib_Chop.h"

// Constructor

FrameLib_Chop::FrameLib_Chop(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner): FrameLib_Processor(context)
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

// Process

void FrameLib_Chop::process()
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
        
        copyVector(output, input + sizeCalc, sizeOut);
        sizeCalc += sizeOut;
    }
}
