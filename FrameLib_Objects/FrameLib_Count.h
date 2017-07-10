
#ifndef FRAMELIB_COUNT_H
#define FRAMELIB_COUNT_H

#include "FrameLib_DSP.h"

class FrameLib_Count : public FrameLib_Processor
{
    enum ParameterList {kLength, kMode};
    enum Modes {kInLength, kRequestedLength};

public:
    
    FrameLib_Count (FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner) : FrameLib_Processor(context, 2, 1)
    {
        mParameters.addDouble(kLength, "length", 64.0, 0);
        mParameters.setMin(0.0);
        mParameters.addEnum(kMode, "mode", 1);
        mParameters.addEnumItem(kInLength, "input");
        mParameters.addEnumItem(kRequestedLength, "requested");
        
        mParameters.set(serialisedParameters);
        
        inputMode(1, TRUE, FALSE, FALSE);
    }
    
protected:
    
    void update()
    {
        FrameLib_Parameters::Serial *serialised = getInput(1);
        
        if (serialised)
            mParameters.set(serialised);
    }
    
    void process()
    {
        unsigned long sizeIn, sizeOut;
        
        getInput(0, &sizeIn);
        
        sizeOut = ((Modes) mParameters.getInt(kMode)) == kInLength ? sizeIn : mParameters.getInt(kLength);
        requestOutputSize(0, sizeOut);
        allocateOutputs();
        
        double *output = getOutput(0, &sizeOut);
        
        for (unsigned int i = 0; i < sizeOut; i++)
            output[i] = (double) i;
    }
};

#endif
