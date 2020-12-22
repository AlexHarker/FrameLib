
#include "FrameLib_Untag.h"

// Constructor

FrameLib_Untag::FrameLib_Untag(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy)
: FrameLib_Processor(context, proxy, &sParamInfo)
{
    const int strBufSize = 10;

    char argStr[strBufSize];
    char tagStr[strBufSize];
    
    mParameters.addInt(kNumOuts, "num_outs", 1);
    mParameters.setClip(1, maxNumOuts);
    mParameters.setInstantiation();
    
    // Read in once to get number of strings needed
    
    mParameters.setErrorReportingEnabled(false);
    mParameters.set(serialisedParameters);
    mParameters.setErrorReportingEnabled(true);

    // If no number of outputs is specified explicity then examine the serialised parameters to determine the number needed
    
    if (!mParameters.changed(kNumOuts))
    {
        for (int i = 0; i < maxNumOuts; i++)
        {
            snprintf(argStr, strBufSize, "%d", i);
            snprintf(tagStr, strBufSize, "tag_%02d", i + 1);
            if (serialisedParameters->find(argStr) != serialisedParameters->end() || serialisedParameters->find(tagStr) != serialisedParameters->end())
                mParameters.set(kNumOuts, (long) (i + 1));
        }
    }
    
    // Read number of ins and setup parameters
    
    mNumOuts = mParameters.getInt(kNumOuts);
    
    for (unsigned long i = 0; i < mNumOuts; i++)
    {
        snprintf(tagStr, strBufSize, "tag_%02lu", i + 1);
        mParameters.addString(kTags + i, tagStr, i);
    }
    
    // Read in again to get parameter tags
    
    mParameters.set(serialisedParameters);
    
    // Setup IO

    setIO(1, mNumOuts);
    
    setInputMode(0, false, true, false, kFrameTagged);
    
    addParameterInput();
}

// Info

std::string FrameLib_Untag::objectInfo(bool verbose)
{
    return formatInfo("Retrieves vectors from a tagged frame according to a specified set of tags: "
                      "Each output deals with one tag and outputs vectors at the input with that tag. "
                      "Outputs will be empty if the tag is not present at the input with a vector value. "
                      "The number of outputs can be set explicitly by parameter. "
                      "Alternatively, it can be set implicitly by the tag parameters present at instantiation.",
                      "Retrieves vectors from a tagged frame according to a specified set of tags.", verbose);
}

std::string FrameLib_Untag::inputInfo(unsigned long idx, bool verbose)
{
    if (idx)
        return parameterInputInfo(verbose);
    else
        return "Tagged Input";
}

std::string FrameLib_Untag::outputInfo(unsigned long idx, bool verbose)
{
    return formatInfo("Vector Output #", "Vector Output #", idx, verbose);
}

// Parameter Info

FrameLib_Untag::ParameterInfo FrameLib_Untag::sParamInfo;

FrameLib_Untag::ParameterInfo::ParameterInfo()
{
    const int strBufSize = 256;
    char str[strBufSize];
    
    add("Sets the number of outputs (and hence the number of tags).");
    
    for (int i = 0; i < maxNumOuts; i++)
    {
        snprintf(str, strBufSize, "Sets the tag related to output %d.", i + 1);
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
            requestOutputSize(i, input->getVectorSize(mParameters.getString(kTags + i)));
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
            input->read(mParameters.getString(kTags + i), output, sizeOut);
    }
}
