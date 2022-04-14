
#include "FrameLib_Join.h"

// Constructor

FrameLib_Join::FrameLib_Join(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy)
: FrameLib_Processor(context, proxy, &sParamInfo)
{
    mParameters.addInt(kNumIns, "num_ins", 2, 0);
    mParameters.setClip(2, maxNumIns);
    mParameters.setInstantiation();
    
    mParameters.addVariableBoolArray(kTriggers, "trigger_ins", true, maxNumIns, maxNumIns);
    mParameters.setInstantiation();
    
    mParameters.set(serialisedParameters);
    
    mNumIns = mParameters.getInt(kNumIns);
    
    setIO(mNumIns, 1);
    
    const double *triggers = mParameters.getArray(kTriggers);
    unsigned long triggersSize = mParameters.getArraySize(kTriggers);
        
    // Set up triggers
    
    for (unsigned long i = 0; i < mNumIns; i++)
        setInputMode(i, false, (i < triggersSize) && triggers[i], false);
}

// Info

std::string FrameLib_Join::objectInfo(bool verbose)
{
    return formatInfo("Concatenates all input frames into a single output frame: "
                      "Each input may trigger output (or not), as set by the triggers_in parameter.",
                      "Concatenates all input frames into a single output frame.", verbose);
}

std::string FrameLib_Join::inputInfo(unsigned long idx, bool verbose)
{
    return formatInfo("Input #", idx);
}

std::string FrameLib_Join::outputInfo(unsigned long idx, bool verbose)
{
    return "Output";
}

// Parameter Info

FrameLib_Join::ParameterInfo FrameLib_Join::sParamInfo;

FrameLib_Join::ParameterInfo::ParameterInfo()
{
    add("Sets the number of inputs.");
    add("Sets which inputs trigger output using an array of bools. "
        "By default all inputs trigger output.");
}

// Process

void FrameLib_Join::process()
{
    // Get Input
    
    unsigned long sizeIn;
    unsigned long sizeOut = 0;
    unsigned long offset = 0;
    
    // Calculate size
    
    for (unsigned long i = 0; i < mNumIns; i++)
    {
        getInput(i, &sizeIn);
        sizeOut += sizeIn;
    }
    
    requestOutputSize(0, sizeOut);
    allocateOutputs();
    
    double *output = getOutput(0, &sizeOut);
    
    // Copy to outputs
    
    if (sizeOut)
    {
        for (unsigned long i = 0; i < mNumIns; i++)
        {
            const double *input = getInput(i, &sizeIn);
            copyVector(output + offset, input, sizeIn);
            offset += sizeIn;
        }
    }
}
