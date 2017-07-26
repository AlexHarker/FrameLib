
#ifndef FRAMELIB_SELECT_H
#define FRAMELIB_SELECT_H

#include "FrameLib_DSP.h"

class FrameLib_Select : public FrameLib_Processor
{
    enum ParameterList {kNumIns, kActiveIn};

public:
    
    FrameLib_Select (FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner) : FrameLib_Processor(context)
    {
        mParameters.addDouble(kNumIns, "numins", 2, 0);
        mParameters.setClip(2, 32);
        mParameters.addDouble(kActiveIn, "input", 0, 1);

        // FIX - clip??
        
        mParameters.set(serialisedParameters);
        
        mNumIns = mParameters.getValue(kNumIns);
        mActiveIn = mParameters.getValue(kActiveIn);
        
        setIO(mNumIns + 1, 1);
        
        for (unsigned long i = 0; i < mNumIns; i++)
            inputMode(i, false, i == mActiveIn, true);
        inputMode(mNumIns, true, false, false);
    }
    
protected:
    
    void update()
    {
        FrameLib_Parameters::Serial *serialised = getInput(mNumIns);
        
        if (serialised)
        {
            mParameters.set(serialised);
        
            mActiveIn = mParameters.getValue(kActiveIn);
        
            for (unsigned long i = 0; i < mNumIns; i++)
                updateTrigger(i, i == mActiveIn);
            
            // FIx - negative values trucated etc. - make sure this is safe
            
        }
    }
    
    void process()
    {
        // FIX - this calls into question the validity of the DSP - it needs checking - I think it will keep memory around...
        
        unsigned long size;
        
        double *input = getInput(mActiveIn, &size);
        
        requestOutputSize(0, size);
        allocateOutputs();
        
        double *output = getOutput(0, &size);
        
        memcpy(output, input, size * sizeof(double));
    }
    
private:
    
    unsigned long mNumIns;
    unsigned long mActiveIn;
};

#endif
