
#include "FrameLib_Select.h"

// Constructor

FrameLib_Select::FrameLib_Select(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner) : FrameLib_Processor(context)
{
    mParameters.addDouble(kNumIns, "numins", 2, 0);
    mParameters.setClip(2, 32);
    mParameters.setInstantiation();
    mParameters.addInt(kActiveIn, "input", 0, 1);
    
    // FIX - clip??
    
    mParameters.set(serialisedParameters);
    
    mNumIns = mParameters.getInt(kNumIns);
    mActiveIn = mParameters.getInt(kActiveIn);
    
    setIO(mNumIns + 1, 1);
    
    for (unsigned long i = 0; i < mNumIns; i++)
        inputMode(i, false, i == mActiveIn, true);
    
    inputMode(mNumIns, true, false, false, kFrameTagged);
}

// Update and Process

void FrameLib_Select::update()
{
    FrameLib_Parameters::Serial *serialised = getInput(mNumIns);
    
    if (serialised)
    {
        mParameters.set(serialised);
        
        mActiveIn = mParameters.getInt(kActiveIn);
        
        for (unsigned long i = 0; i < mNumIns; i++)
            updateTrigger(i, i == mActiveIn);
        
        // FIX - negative values truncated etc. - make sure this is safe
        
    }
}

void FrameLib_Select::process()
{
    // FIX - this calls into question the validity of the DSP - it needs checking - I think it will keep memory around...
    
    unsigned long size;
    
    double *input = getInput(mActiveIn, &size);
    
    requestOutputSize(0, size);
    allocateOutputs();
    
    double *output = getOutput(0, &size);
    
    copyVector(output, input, size);
}