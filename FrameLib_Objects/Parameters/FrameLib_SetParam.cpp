
#include "FrameLib_SetParam.h"

// Constructor

FrameLib_SetParam::FrameLib_SetParam (FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner) : FrameLib_Processor(context)
{
    char nameStr[7];
    
    mParameters.addInt(kNumIns, "numins", 1, 0);
    mParameters.setClip(1, 10);
    mParameters.setInstantiation();
    
    // Read in once to get number of strings needed
    
    mParameters.set(serialisedParameters);
    mNumIns = mParameters.getInt(kNumIns);

    for (int i = 0; i < mNumIns; i++)
    {
        sprintf(nameStr, "name%02d", i + 1);
        mParameters.addString(kNames + 0, nameStr, i + 1);
        mParameters.setInstantiation();
    }
    
    // Read in again to get names
    
    mParameters.set(serialisedParameters);
    mNumIns = mParameters.getInt(kNumIns);
    
    setIO(mNumIns + 1, 1);
    
    inputMode(mNumIns, false, true, false, kFrameTagged);
    outputMode(0, kFrameTagged);
}

// Info

std::string FrameLib_SetParam::objectInfo(bool verbose)
{
    return getInfo("Tags vectors with parameter names ready to send to the parameter input of an object: "
                   "A variable number of inputs is available, each of which deal will a specific parameter name. "
                   "The final input takes tagged input which is concatanated with other inputs after tagging. All inputs trigger output.",
                   "Tags vectors with parameter names ready to send to the parameter input of an object.", verbose);
}

std::string FrameLib_SetParam::inputInfo(unsigned long idx, bool verbose)
{
    if (idx == mNumIns)
        return getInfo("Parameter Input - takes tagged input for concatenation with other inputs", "Parameter Input", verbose);
    else
        return getInfo("Input for Parameter #", "Input  for Parameter #", idx, verbose);

}

std::string FrameLib_SetParam::outputInfo(unsigned long idx, bool verbose)
{
    return "Tagged Output Frames";
}

// Parameter Info

FrameLib_SetParam::ParameterInfo FrameLib_SetParam::sParamInfo;

FrameLib_SetParam::ParameterInfo::ParameterInfo()
{
    char str[256];
    
    add("Sets the number of object inputs (and hence the number of parameters.");
    for (int i = 0; i < 10; i++)
    {
        sprintf(str, "Sets the parameter name for input %d.", i + 1);
        add(str);
    }
}

// Process

void FrameLib_SetParam::process()
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
