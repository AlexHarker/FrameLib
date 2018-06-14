
#include "FrameLib_Untag.h"

// Constructor

FrameLib_Untag::FrameLib_Untag(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy) : FrameLib_Processor(context, proxy, &sParamInfo)
{
    char argStr[10];
    char nameStr[10];
    
    mParameters.addInt(kNumOuts, "num_outs", 1);
    mParameters.setClip(1, maxNumOuts);
    mParameters.setInstantiation();
    
    // Read in once to get number of strings needed
    
    mParameters.setErrorReportingEnabled(false);
    mParameters.set(serialisedParameters);
    mParameters.setErrorReportingEnabled(true);

    // If no number of inputs is specified explicityly then examine the serialised parameters to determine the number needed
    
    if (!mParameters.changed(kNumOuts))
    {
        for (int i = 0; i < maxNumOuts; i++)
        {
            sprintf(argStr, "%d", i);
            sprintf(nameStr, "tag_%02d", i + 1);
            if (serialisedParameters->find(argStr) != serialisedParameters->end() || serialisedParameters->find(nameStr) != serialisedParameters->end())
                mParameters.set(kNumOuts, (long) (i + 1));
        }
    }
    
    // Read number of ins and setup parameters
    
    mNumOuts = mParameters.getInt(kNumOuts);
    
    for (int i = 0; i < mNumOuts; i++)
    {
        sprintf(nameStr, "tag_%02d", i + 1);
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

std::string FrameLib_Untag::objectInfo(bool verbose)
{
    return formatInfo("Separates vectors from a tagged frame according to a given set of tags: "
                      "A variable number of outputs is available, each of which deal will a specific tag. "
                      "The number of outputs is specified either explicitly with a parameter or implicitly by which arguments or tag parameters are present.",
                      "Separates vectors from a tagged frame according to a given set of names.", verbose);
}

std::string FrameLib_Untag::inputInfo(unsigned long idx, bool verbose)
{
    return formatInfo("Parameter Input - takes tagged input", "Parameter Input", verbose);
}

std::string FrameLib_Untag::outputInfo(unsigned long idx, bool verbose)
{
    return formatInfo("Vector Output #", "Vector Output #", idx, verbose);
}

// Parameter Info

FrameLib_Untag::ParameterInfo FrameLib_Untag::sParamInfo;

FrameLib_Untag::ParameterInfo::ParameterInfo()
{
    char str[256];
    
    add("Sets the number of outputs (and hence the number of tags.");
    
    for (int i = 0; i < maxNumOuts; i++)
    {
        sprintf(str, "Sets the tag related to output %d.", i + 1);
        add(str);
    }
}

// Process

void FrameLib_Untag::process()
{    
    unsigned long sizeOut;
    
    const FrameLib_Parameters::Serial *input = getInput(0);

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
