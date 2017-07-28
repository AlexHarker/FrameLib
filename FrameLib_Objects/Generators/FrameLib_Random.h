
#ifndef FRAMELIB_RANDOM_H
#define FRAMELIB_RANDOM_H

#include "FrameLib_DSP.h"
#include "FrameLib_RandGen.h"

class FrameLib_Random : public FrameLib_Processor, private FrameLib_Info
{
    enum ParameterList { kLength, kMode };
    enum Modes { kInLength, kRequestedLength };

    struct ParameterInfo : public FrameLib_Parameters::Info
    {
        ParameterInfo()
        {
            add("Sets the length of the output when the mode is set to requested.");
            add("Controls how the output length is determined: "
                "input - the output frame size will match the input size. "
                "requested - the output frame size is set by the length parameter.");
        }
    };
    
public:
    
    FrameLib_Random (FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner) : FrameLib_Processor(context, 1, 1)
    {
        mParameters.addInt(kLength, "length", 1, 0);
        mParameters.setMin(0);

        mParameters.addEnum(kMode, "mode", 1);
        mParameters.addEnumItem(kInLength, "input");
        mParameters.addEnumItem(kRequestedLength, "requested");
        
        mParameters.set(serialisedParameters);
        
        mParameters.setInfo(getParameterInfo());
    }
    
    const char *objectInfo(bool verbose)
    {
        return getInfo("Generates frames of random values in the range [0-1]: The size of the output is dependent on the mode. "
                       "The output size may either be set as a parameter, or be set to match that of the triggering input.",
                       "Generates frames of random values in the range [0-1].", verbose);
    }
    
    const char *inputInfo(unsigned long idx, bool verbose)  { return getInfo("Trigger Frame", "Trigger Frame - triggers generation of output", verbose); }
    const char *outputInfo(unsigned long idx, bool verbose) { return "Frame of Random Values"; }
    
private:
    
    void process()
    {
        unsigned long sizeIn, sizeOut;
        
        getInput(0, &sizeIn);
        
        sizeOut = ((Modes) mParameters.getInt(kMode)) == kInLength ? sizeIn : mParameters.getInt(kLength);
        requestOutputSize(0, sizeOut);
        allocateOutputs();
        
        double *output = getOutput(0, &sizeOut);
        
        for (unsigned long i = 0; i < sizeOut; i++)
            output[i] = mRandom.randDouble();
    }
        
    ParameterInfo *getParameterInfo()
    {
        static ParameterInfo info;
        return &info;
    }
    
    FrameLib_RandGen mRandom;
};

#endif
