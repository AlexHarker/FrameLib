
#include "FrameLib_Tag.h"

// Constructor

FrameLib_Tag::FrameLib_Tag(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy) : FrameLib_Processor(context, proxy, &sParamInfo)
{
    const int strBufSize = 10;
    
    char argStr[strBufSize];
    char nameStr[strBufSize];
    
    mParameters.addInt(kNumIns, "num_ins", 1);
    mParameters.setClip(1, maxNumIns);
    mParameters.setInstantiation();
    mParameters.addEnum(kEmptyMode, "empty");
    mParameters.addEnumItem(0, "ignore");
    mParameters.addEnumItem(1, "reset");
    mParameters.setInstantiation();

    // Read in once to get number of strings needed
    
    mParameters.setErrorReportingEnabled(false);
    mParameters.set(serialisedParameters);
    mParameters.setErrorReportingEnabled(true);

    // If no number of inputs is specified explicity then examine the serialised parameters to determine the number needed
    
    if (!mParameters.changed(kNumIns))
    {
        for (int i = 0; i < maxNumIns; i++)
        {
            snprintf(argStr, strBufSize, "%d", i);
            snprintf(nameStr, strBufSize, "tag_%02d", i + 1);
            if (serialisedParameters->find(argStr) != serialisedParameters->end() || serialisedParameters->find(nameStr) != serialisedParameters->end())
                mParameters.set(kNumIns, (long) (i + 1));
        }
    }
    
    // Read number of ins and setup parameters
    
    mNumIns = mParameters.getInt(kNumIns);
    
    for (unsigned long i = 0; i < mNumIns; i++)
    {
        snprintf(nameStr, strBufSize, "tag_%02lu", i + 1);
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

std::string FrameLib_Tag::objectInfo(bool verbose)
{
    return formatInfo("Tags vectors with names ready to send to the parameter input of an object, or for routing purposes: "
                      "A variable number of inputs is available, each of which deal will a specific tag. "
                      "The number of inputs is specified either explicitly with a parameter or implicitly by which arguments or tag parameters are present."
                      "The final input takes tagged input which is concatanated with other inputs after tagging. All inputs trigger output.",
                      "Tags vectors with names ready to send to the parameter input of an object or for routing purposes.", verbose);
}

std::string FrameLib_Tag::inputInfo(unsigned long idx, bool verbose)
{
    if (idx == mNumIns)
        return formatInfo("Parameter Input - takes tagged input for concatenation with other inputs", "Parameter Input", verbose);
    else
        return formatInfo("Input for Tag #", "Input  for Tag #", idx, verbose);

}

std::string FrameLib_Tag::outputInfo(unsigned long idx, bool verbose)
{
    return "Tagged Output Frames";
}

// Parameter Info

FrameLib_Tag::ParameterInfo FrameLib_Tag::sParamInfo;

FrameLib_Tag::ParameterInfo::ParameterInfo()
{
    const int strBufSize = 256;
    char str[strBufSize];
    
    add("Sets the number of inputs (and hence the number of tags).");

    add("Sets the behaviour when empty frames are received: ignore - empty frames are ignored / reset - empty frames create empty tags to reset parameters. ");

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
    
    EmptyMode mode = (EmptyMode) mParameters.getInt(kEmptyMode);
    
    requestOutputSize(0, 0);
    
    for (unsigned long i = 0; i < mNumIns; i++)
    {
        getInput(i, &sizeIn);
        if (sizeIn || mode == kReset)
            requestAddedOutputSize(0, Serial::calcSize(mParameters.getString(kNames + i), sizeIn));
    }
    
    requestAddedOutputSize(0, Serial::calcSize(preTagged));
    
    if (allocateOutputs())
    {
        FrameLib_Parameters::Serial *output = getOutput(0);
        
        for (unsigned long i = 0; i < mNumIns; i++)
        {
            const double *input = getInput(i, &sizeIn);
            if (sizeIn || mode == kReset)
                output->write(mParameters.getString(kNames + i), input, sizeIn);
        }
        
        output->write(preTagged);
    }
}
