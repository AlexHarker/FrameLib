
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
    
    mParameters.setInfo(&sParamInfo);
    
    mNumIns = mParameters.getInt(kNumIns);
    mActiveIn = mParameters.getInt(kActiveIn);
    
    setIO(mNumIns + 1, 1);
    
    for (unsigned long i = 0; i < mNumIns; i++)
        inputMode(i, false, i == mActiveIn, true, kFrameAny);
    
    setParameterInput(mNumIns);
}

// Info

const char *FrameLib_Select::objectInfo(bool verbose)
{
    return getInfo("Selects between a number of input frame streams: The number of inputs is variable. The selected input can be changed with a parameter.",
                   "Selects between a number of input frame streams.", verbose);
}

const char *FrameLib_Select::inputInfo(unsigned long idx, bool verbose)
{
    if (idx == mNumIns)
        return getInfo("Parameter Update - tagged input updates parameters", "Parameter Update", verbose);
    else
        return getInfo("Input #", "Input #", idx, verbose);
}

const char *FrameLib_Select::outputInfo(unsigned long idx, bool verbose)
{
    return "Output Frames";
}

// Parameter Info

FrameLib_Select::ParameterInfo FrameLib_Select::sParamInfo;

FrameLib_Select::ParameterInfo::ParameterInfo()
{
    add("Sets the number of object inputs.");
    add("Sets the current input (or off if out of range).");
}

// Update and Process

void FrameLib_Select::update()
{
    if (mParameters.changed(kActiveIn))
    {
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