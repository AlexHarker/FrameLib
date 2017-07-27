
#ifndef FRAMELIB_SETPARAM_H
#define FRAMELIB_SETPARAM_H

#include "FrameLib_DSP.h"

class FrameLib_SetParam : public FrameLib_Processor
{
    enum ParameterList {kNumIns, kNames};
    
public:
    
    FrameLib_SetParam (FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner) : FrameLib_Processor(context)
    {
        mParameters.addInt(kNumIns, "numins", 1, 0);
        mParameters.setClip(1, 32);
        mParameters.setInstantiation();
        
        mParameters.addString(kNames + 0, "name1", 1);
        mParameters.addString(kNames + 1, "name2", 2);
        mParameters.addString(kNames + 2, "name3", 3);
        mParameters.addString(kNames + 3, "name4", 4);
        mParameters.addString(kNames + 4, "name5", 5);
        mParameters.addString(kNames + 5, "name6", 6);
        mParameters.addString(kNames + 6, "name7", 7);
        mParameters.addString(kNames + 7, "name8", 8);
        mParameters.addString(kNames + 8, "name9", 9);
        mParameters.addString(kNames + 9, "name10", 10);
        
        mParameters.set(serialisedParameters);
        mNumIns = mParameters.getInt(kNumIns);

        setIO(mNumIns + 1, 1);
        
        inputMode(mNumIns, false, true, false, kFrameTagged);
        outputMode(0, kFrameTagged);
    }
    
protected:
    
    void process()
    {
        // FIX - use of types for sizes should all be size_t?
        
        unsigned long sizeIn, sizeOut;
        
        sizeOut = 0;
        
        for (unsigned long i = 0; i < mNumIns; i++)
        {
            getInput(i, &sizeIn);
            sizeOut += FrameLib_Parameters::Serial::calcSize(mParameters.getString(kNames + i), sizeIn);
        }
        
        FrameLib_Parameters::Serial *preTagged = getInput(mNumIns);
        sizeOut += FrameLib_Parameters::Serial::calcSize(preTagged);
        
        requestOutputSize(0, sizeOut);
        allocateOutputs();
        
        FrameLib_Parameters::Serial *output = getOutput(0);
        
        for (unsigned long i = 0; i < mNumIns; i++)
        {
            double *input = getInput(i, &sizeIn);
            output->write(mParameters.getString(kNames + i), input, sizeIn);
        }
        
        output->write(preTagged);
    }
    
private:
    
    unsigned long mNumIns;
};

#endif
