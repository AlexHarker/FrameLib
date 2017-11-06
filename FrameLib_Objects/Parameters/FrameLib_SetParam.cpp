
#include "FrameLib_SetParam.h"

// Constructor

FrameLib_SetParam::FrameLib_SetParam(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner) : FrameLib_Processor(context, owner, &sParamInfo)
{
    char argStr[10];
    char nameStr[10];
    
    mParameters.addInt(kNumIns, "num_ins", 1);
    mParameters.setClip(1, maxNumIns);
    mParameters.setInstantiation();
    
    // Read in once to get number of strings needed
    
    mParameters.set(serialisedParameters);

    // If no number of inputs is specified explicityly then examine the serialised parameters to determine the number needed
    
    if (!mParameters.changed(kNumIns))
    {
        for (int i = 0; i < maxNumIns; i++)
        {
            sprintf(argStr, "%d", i);
            sprintf(nameStr, "param_%02d", i + 1);
            if (serialisedParameters->find(argStr) != serialisedParameters->end() || serialisedParameters->find(nameStr) != serialisedParameters->end())
                mParameters.set(kNumIns, (long) (i + 1));
        }
    }
    
    // Read number of ins and setup parameters
    
    mNumIns = mParameters.getInt(kNumIns);
    
    for (int i = 0; i < mNumIns; i++)
    {
        sprintf(nameStr, "param_%02d", i + 1);
        mParameters.addString(kNames + i, nameStr, i);
        mParameters.setInstantiation();
    }
    
    // Read in again to get parameter names
    
    mParameters.set(serialisedParameters);
    
    // Setup IO

    setIO(mNumIns + 1, 1);
    
    setInputMode(mNumIns, false, true, false, kFrameTagged);
    setOutputType(0, kFrameTagged);
}

// Info

std::string FrameLib_SetParam::objectInfo(bool verbose)
{
    return formatInfo("Tags vectors with parameter names ready to send to the parameter input of an object: "
                   "A variable number of inputs is available, each of which deal will a specific parameter name. "
                   "The final input takes tagged input which is concatanated with other inputs after tagging. All inputs trigger output.",
                   "Tags vectors with parameter names ready to send to the parameter input of an object.", verbose);
}

std::string FrameLib_SetParam::inputInfo(unsigned long idx, bool verbose)
{
    if (idx == mNumIns)
        return formatInfo("Parameter Input - takes tagged input for concatenation with other inputs", "Parameter Input", verbose);
    else
        return formatInfo("Input for Parameter #", "Input  for Parameter #", idx, verbose);

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
    
    for (int i = 0; i < maxNumIns; i++)
    {
        sprintf(str, "Sets the parameter name for input %d.", i + 1);
        add(str);
    }
}

// Process

void FrameLib_SetParam::process()
{    
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
    
    if (output)
    {
        for (unsigned long i = 0; i < mNumIns; i++)
        {
            double *input = getInput(i, &sizeIn);
            output->write(mParameters.getString(kNames + i), input, sizeIn);
        }
        
        output->write(preTagged);
    }
}
