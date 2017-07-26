
#ifndef FRAMELIB_RANDOM_H
#define FRAMELIB_RANDOM_H

#include "FrameLib_DSP.h"
#include "FrameLib_RandGen.h"

class FrameLib_Random : public FrameLib_Processor
{
    enum ParameterList {kLength, kMode};
    enum Modes {kInLength, kRequestedLength};

public:
    
    FrameLib_Random (FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner) : FrameLib_Processor(context, 1, 1)
    {
        mParameters.addInt(kLength, "length", 64, 0);
        mParameters.setMin(0);

        mParameters.addEnum(kMode, "mode", 1);
        mParameters.addEnumItem(kInLength, "input");
        mParameters.addEnumItem(kRequestedLength, "requested");
        
        mParameters.set(serialisedParameters);
    }
    
protected:
    
    void process()
    {
        unsigned long sizeIn, sizeOut;
        
        getInput(0, &sizeIn);
        
        sizeOut = ((Modes) mParameters.getInt(kMode)) == kInLength ? sizeIn : mParameters.getInt(kLength);
        requestOutputSize(0, sizeOut);
        allocateOutputs();
        
        double *output = getOutput(0, &sizeOut);
        
        for (unsigned int i = 0; i < sizeOut; i++)
            output[i] = mRandom.randDouble();
    }
    
private:
    
    FrameLib_RandGen mRandom;
};

#endif
