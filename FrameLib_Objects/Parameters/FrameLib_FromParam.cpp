
#include "FrameLib_FromParam.h"

// Constructor

FrameLib_FromParam::FrameLib_FromParam(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner) : FrameLib_Processor(context, owner, &sParamInfo)
{
    char argStr[10];
    char nameStr[10];
    
    mParameters.addInt(kNumOuts, "num_outs", 1);
    mParameters.setClip(1, maxNumOuts);
    mParameters.setInstantiation();
    
    // Read in once to get number of strings needed
    
    mParameters.set(serialisedParameters);

    // If no number of inputs is specified explicityly then examine the serialised parameters to determine the number needed
    
    if (!mParameters.changed(kNumOuts))
    {
        for (int i = 0; i < maxNumOuts; i++)
        {
            sprintf(argStr, "%d", i);
            sprintf(nameStr, "param_%02d", i + 1);
            if (serialisedParameters->find(argStr) != serialisedParameters->end() || serialisedParameters->find(nameStr) != serialisedParameters->end())
                mParameters.set(kNumOuts, (long) (i + 1));
        }
    }
    
    // Read number of ins and setup parameters
    
    mNumOuts = mParameters.getInt(kNumOuts);
    
    for (int i = 0; i < mNumOuts; i++)
    {
        sprintf(nameStr, "param_%02d", i + 1);
        mParameters.addString(kNames + i, nameStr, i);
        mParameters.setInstantiation();
    }
    
    // Read in again to get parameter names
    
    mParameters.set(serialisedParameters);
    
    // Setup IO

    setIO(1, mNumOuts);
    
    setInputMode(0, false, true, false, kFrameTagged);
}

// Info

std::string FrameLib_FromParam::objectInfo(bool verbose)
{
    return formatInfo("Separates vectors from a tagged frame according to parameter names: "
                   "A variable number of outputs is available, each of which deal will a specific parameter name.",
                   "Separates vectors from a tagged frame according to parameter names.", verbose);
}

std::string FrameLib_FromParam::inputInfo(unsigned long idx, bool verbose)
{
    return formatInfo("Parameter Input - takes tagged input", "Parameter Input", verbose);
}

std::string FrameLib_FromParam::outputInfo(unsigned long idx, bool verbose)
{
    return formatInfo("Vector Ouput #", "Vector Ouput #", idx, verbose);
}

// Parameter Info

FrameLib_FromParam::ParameterInfo FrameLib_FromParam::sParamInfo;

FrameLib_FromParam::ParameterInfo::ParameterInfo()
{
    char str[256];
    
    add("Sets the number of object outputs (and hence the number of parameters.");
    
    for (int i = 0; i < maxNumOuts; i++)
    {
        sprintf(str, "Sets the parameter name for output %d.", i + 1);
        add(str);
    }
}

// Process

void FrameLib_FromParam::process()
{    
    unsigned long sizeOut;
    
    FrameLib_Parameters::Serial *input = getInput(0);

    if (input)
    {
        for (unsigned long i = 0; i < mNumOuts; i++)
            requestOutputSize(i, input->getVectorSize(mParameters.getString(kNames + i)));
    }
    else
    {
        for (unsigned long i = 0; i < mNumOuts; i++)
            requestOutputSize(i, 0);
    }
    
    allocateOutputs();
    
    for (unsigned long i = 0; i < mNumOuts; i++)
    {
        double *output = getOutput(i, &sizeOut);
        if (output)
            input->read(mParameters.getString(kNames + i), output, sizeOut);
    }
}
