
#include "FrameLib_Join.h"

// Constructor

FrameLib_Join::FrameLib_Join(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner) : FrameLib_Processor(context)
{
    mParameters.addInt(kNumIns, "numins", 2, 0);
    mParameters.setClip(2, 32);
    mParameters.setInstantiation();
    
    mParameters.addBoolArray(kTriggers, "triggers", 1.0, 32);
    mParameters.setInstantiation();
    
    mParameters.set(serialisedParameters);
    
    setIO(mParameters.getInt(kNumIns), 1);
    
    const double *triggers = mParameters.getArray(kTriggers);
    
    // Set up triggers
    
    for (unsigned long i = 0; i < getNumIns(); i++)
        inputMode(i, false, triggers[i], false);
}

// Process

void FrameLib_Join::process()
{
    // Get Input
    
    unsigned long sizeIn;
    unsigned long sizeOut = 0;
    unsigned long offset = 0;
    
    // Calculate size
    
    for (unsigned long i = 0; i < getNumIns(); i++)
    {
        getInput(i, &sizeIn);
        sizeOut += sizeIn;
    }
    
    requestOutputSize(0, sizeOut);
    allocateOutputs();
    
    double *output = getOutput(0, &sizeOut);
    
    // Copy to outputs
    
    if (sizeOut)
    {
        for (unsigned long i = 0; i < getNumIns(); i++)
        {
            double *input = getInput(i, &sizeIn);
            memcpy(output + offset, input, sizeIn * sizeof(double));
            offset += sizeIn;
        }
    }
}