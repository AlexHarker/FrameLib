
#include "FrameLib_Lookup.h"

FrameLib_Lookup::FrameLib_Lookup(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner) : FrameLib_Processor(context, 2, 1)
{
    // FIX - loads of different mode options here (mapping of positions + padding values
    
    mParameters.set(serialisedParameters);
    
    inputMode(1, false, false, false);
}

// Info

const char *FrameLib_Lookup::objectInfo(bool verbose)
{
    return getInfo("Use one frame as a lookup table for another: The left input frame is used to lookup values from the last values received as the right input frame. "
                   "The output is the same size as the left input, which is interpreted as a set of sample positions used to read the right input. "
                   "Only the left input triggers output. ",
                   "Use one frame as a lookup table for another.", verbose);
}

const char *FrameLib_Lookup::inputInfo(unsigned long idx, bool verbose)
{
    if (idx)
        return getInfo("Frame for Table - values are retrieved from this frame / does not trigger output", "Frame for Table", verbose);
    else
        return getInfo("Values to Lookup - interpreted as sample positions into the table / right input", "Values to Lookup", verbose);
}

const char *FrameLib_Lookup::outputInfo(unsigned long idx, bool verbose)
{
    return getInfo("Output Frame", "Output Frame - values after look up", verbose);
}

// Process

void FrameLib_Lookup::process()
{
    unsigned long sizeIn1, sizeIn2, sizeOut;
    
    double *input1 = getInput(0, &sizeIn1);
    double *input2 = getInput(1, &sizeIn2);
    
    requestOutputSize(0, sizeIn1);
    allocateOutputs();
    
    double *output = getOutput(0, &sizeOut);
    
    for (unsigned long i = 0; i < sizeOut; i++)
    {
        long pos = input1[i];
        
        output[i] = (pos >= 0) && (pos < sizeIn2) ? input2[pos] : 0.0;
    }
}
