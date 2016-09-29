
#ifndef FRAMELIB_SELECT_H
#define FRAMELIB_SELECT_H

#include "FrameLib_DSP.h"

class FrameLib_Select : public FrameLib_Processor
{
    enum AttributeList {kNumIns, kActiveIn};

public:
    
    FrameLib_Select (FrameLib_Context context, FrameLib_Attributes::Serial *serialisedAttributes, void *owner) : FrameLib_Processor(context)
    {
        mAttributes.addDouble(kNumIns, "numins", 2, 0);
        mAttributes.setClip(2, 32);
        mAttributes.addDouble(kActiveIn, "input", 0, 1);

        // FIX - clip??
        
        mAttributes.set(serialisedAttributes);
        
        mNumIns = mAttributes.getValue(kNumIns);
        mActiveIn = mAttributes.getValue(kActiveIn);
        
        setIO(mNumIns + 1, 1);
        
        for (unsigned long i = 0; i < mNumIns; i++)
            inputMode(i, FALSE, i == mActiveIn, TRUE);
        inputMode(mNumIns, TRUE, FALSE, FALSE);
    }
    
protected:
    
    void update()
    {
        FrameLib_Attributes::Serial *serialised = getInput(mNumIns);
        
        if (serialised)
        {
            mAttributes.set(serialised);
        
            mActiveIn = mAttributes.getValue(kActiveIn);
        
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
