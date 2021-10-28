
#include "FrameLib_Tag.h"

// Constructor

FrameLib_Tag::FrameLib_Tag(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy)
: FrameLib_Processor(context, proxy, &sParamInfo)
{
    mParameters.addInt(kNumIns, "num_ins", 1);
    mParameters.setClip(1, maxNumIns);
    mParameters.setInstantiation();
    
    mParameters.addEnum(kEmptyMode, "empty");
    mParameters.addEnumItem(kIgnore, "ignore");
    mParameters.addEnumItem(kReset, "reset");

    // Read in once to get number of strings needed
    
    mParameters.setErrorReportingEnabled(false);
    mParameters.set(serialisedParameters);
    mParameters.setErrorReportingEnabled(true);

    // If no number of inputs is specified explicity then examine the serialised parameters to determine the number needed
    
    if (!mParameters.changed(kNumIns))
    {
        for (int i = 0; i < maxNumIns; i++)
        {
            if (serialisedParameters->find(FrameLib_StringMaker<>(i)) != serialisedParameters->end() || serialisedParameters->find(FrameLib_StringMaker<>("tag_", i + 1, 2)) != serialisedParameters->end())
                mParameters.set(kNumIns, (long) (i + 1));
        }
    }
    
    // Read number of ins and setup parameters
    
    mNumIns = mParameters.getInt(kNumIns);
    
    for (unsigned long i = 0; i < mNumIns; i++)
        mParameters.addString(kTags + i, FrameLib_StringMaker<>("tag_", i + 1, 2), i);
    
    // Read in again to get parameter tags
    
    mParameters.set(serialisedParameters);
    
    // Setup IO

    setIO(mNumIns + 1, 1);
    
    setInputMode(mNumIns, false, true, false, kFrameTagged);
    setOutputType(0, kFrameTagged);
    
    addParameterInput();
}

// Info

std::string FrameLib_Tag::objectInfo(bool verbose)
{
    return formatInfo("Tags vectors with tags either in order to set parameters or for routing purposes: "
                      "Tag inputs each deal with one tag and take input vectors. "
                      "The number of tag inputs can be set explicitly by parameter. "
                      "Alternatively, it can be set implicitly by the tag parameters present at instantiation. "
                      "A further input takes pre-tagged input to concatanated with other inputs after tagging. "
                      "All inputs except the parameter input trigger output.",
                      "Tags vectors with tags either in order to set parameters or for routing purposes.", verbose);
}

std::string FrameLib_Tag::inputInfo(unsigned long idx, bool verbose)
{
    if (idx == mNumIns + 1)
        return parameterInputInfo(verbose);
    if (idx == mNumIns)
        return formatInfo("Pre-Tagged Input - for concatenation with other inputs", "Tagged Input", verbose);
    else
        return formatInfo("Input For Tag #", idx);

}

std::string FrameLib_Tag::outputInfo(unsigned long idx, bool verbose)
{
    return "Output";
}

// Parameter Info

FrameLib_Tag::ParameterInfo FrameLib_Tag::sParamInfo;

FrameLib_Tag::ParameterInfo::ParameterInfo()
{
    const int strBufSize = 64;
    char str[strBufSize];
    
    add("Sets the number of inputs (and hence the number of tags).");

    add("Sets the behaviour when empty frames are received: "
        "ignore - empty frames are ignored. "
        "reset - empty frames create empty tags to reset parameters to defaults.");

    for (int i = 0; i < maxNumIns; i++)
    {
        snprintf(str, strBufSize, "Sets the tag for input %d.", i + 1);
        add(str);
    }
}

// Process

void FrameLib_Tag::process()
{    
    const Serial *preTagged = getInput(mNumIns);
    unsigned long sizeIn;
    
    EmptyModes mode = mParameters.getEnum<EmptyModes>(kEmptyMode);
    
    requestOutputSize(0, 0);
    
    for (unsigned long i = 0; i < mNumIns; i++)
    {
        getInput(i, &sizeIn);
        if (sizeIn || mode == kReset)
            requestAddedOutputSize(0, Serial::calcSize(mParameters.getString(kTags + i), sizeIn));
    }
    
    requestAddedOutputSize(0, Serial::calcSize(preTagged));
    
    if (allocateOutputs())
    {
        FrameLib_Parameters::Serial *output = getOutput(0);
        
        for (unsigned long i = 0; i < mNumIns; i++)
        {
            const double *input = getInput(i, &sizeIn);
            if (sizeIn || mode == kReset)
                output->write(mParameters.getString(kTags + i), input, sizeIn);
        }
        
        output->write(preTagged);
    }
}
