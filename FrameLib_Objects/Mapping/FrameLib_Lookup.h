
#ifndef FRAMELIB_LOOKUP_H
#define FRAMELIB_LOOKUP_H

#include "FrameLib_DSP.h"

class FrameLib_Lookup : public FrameLib_Processor
{
    
public:
    
    FrameLib_Lookup (FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner) : FrameLib_Processor(context, 2, 1)
    {
        // FIX - loads of different mode options here (mapping of positions + padding values
        
        mParameters.set(serialisedParameters);
        
        inputMode(1, false, false, false);
    }
    
private:
    
    void process()
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
};

#endif
