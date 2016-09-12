
#ifndef FRAMELIB_SETPARAM_H
#define FRAMELIB_SETPARAM_H

#include "FrameLib_DSP.h"

class FrameLib_SetParam : public FrameLib_Processor
{
    enum AttributeList {kNumIns, kNames};
    
public:
    
    FrameLib_SetParam (DSPQueue *queue, FrameLib_Attributes::Serial *serialisedAttributes, void *owner) : FrameLib_Processor(queue)
    {
        mAttributes.addDouble(kNumIns, "numins", 1, 0);
        mAttributes.setClip(1.0, 32.0);
        
        mAttributes.addString(kNames + 0, "name1", "", 1);
        mAttributes.addString(kNames + 1, "name2", "", 2);
        mAttributes.addString(kNames + 2, "name3", "", 3);
        mAttributes.addString(kNames + 3, "name4", "", 4);
        mAttributes.addString(kNames + 4, "name5", "", 5);
        mAttributes.addString(kNames + 5, "name6", "", 6);
        mAttributes.addString(kNames + 6, "name7", "", 7);
        mAttributes.addString(kNames + 7, "name8", "", 8);
        mAttributes.addString(kNames + 8, "name9", "", 9);
        mAttributes.addString(kNames + 9, "name10", "", 10);
        
        mAttributes.set(serialisedAttributes);
        mNumIns = mAttributes.getInt(kNumIns);

        setIO(mNumIns + 1, 1);
        
        outputMode(0, kOutputTagged);
    }
    
protected:
    
    void process()
    {
        // FIX - use of types for sizes should all be size_t?
        
        unsigned long sizeIn, sizeOut;
        
        sizeOut = 0;
        
        for (unsigned int i = 0; i < mNumIns; i++)
        {
            getInput(i, &sizeIn);
            sizeOut += FrameLib_Attributes::Serial::calcSize(mAttributes.getString(kNames + i), sizeIn);
        }
        
        FrameLib_Attributes::Serial *preTagged = getInput(mNumIns);
        sizeOut += FrameLib_Attributes::Serial::calcSize(preTagged);
        
        requestOutputSize(0, sizeOut);
        allocateOutputs();
        
        FrameLib_Attributes::Serial *output = getOutput(0);
        
        for (unsigned int i = 0; i < mNumIns; i++)
        {
            double *input = getInput(i, &sizeIn);
            output->write(mAttributes.getString(kNames + i), input, sizeIn);
        }
        
        output->write(preTagged);
    }
    
private:
    
    unsigned long mNumIns;
};

#endif
