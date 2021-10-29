
#include "FrameLib_LookupString.h"

// Constructor

FrameLib_LookupString::FrameLib_LookupString(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy)
: FrameLib_Processor(context, proxy, &sParamInfo)
{
    mParameters.addInt(kNumItems, "num_items", 1);
    mParameters.setClip(1, maxNumItems);
    mParameters.setInstantiation();
    
    mParameters.addString(kTag, "tag", 0);

    mParameters.addEnum(kEmptyMode, "empty");
    mParameters.addEnumItem(kIgnore, "ignore");
    mParameters.addEnumItem(kReset, "reset");

    // Read in once to get number of items needed
    
    mParameters.setErrorReportingEnabled(false);
    mParameters.set(serialisedParameters);
    mParameters.setErrorReportingEnabled(true);

    // If no number of inputs is specified explicity then examine the serialised parameters to determine the number needed
    
    if (!mParameters.changed(kNumItems))
    {
        for (int i = 0; i < maxNumItems; i++)
        {
            if (serialisedParameters->find(FrameLib_StringMaker<>(i + 1)) != serialisedParameters->end() || serialisedParameters->find(FrameLib_StringMaker<>("item_", i + 1, 2)) != serialisedParameters->end())
                mParameters.set(kNumItems, (long) (i + 1));
        }
    }
    
    // Read number of ins and setup parameters
    
    mNumItems = mParameters.getInt(kNumItems);
    
    for (unsigned long i = 0; i < mNumItems; i++)
        mParameters.addString(kItems + i, FrameLib_StringMaker<>("items_", i + 1, 2), i + 1);
    
    // Read in again to get item strings
    
    mParameters.set(serialisedParameters);
    
    // Setup IO

    setIO(1, 1);
    
    setOutputType(0, kFrameTagged);
    
    addParameterInput();
}

// Info

std::string FrameLib_LookupString::objectInfo(bool verbose)
{
    return formatInfo("Tags vectors with tags either in order to set parameters or for routing purposes: "
                      "Tag inputs each deal with one tag and take input vectors. "
                      "The number of tag inputs can be set explicitly by parameter. "
                      "Alternatively, it can be set implicitly by the tag parameters present at instantiation. "
                      "A further input takes pre-tagged input to concatanated with other inputs after tagging. "
                      "All inputs except the parameter input trigger output.",
                      "Tags vectors with tags either in order to set parameters or for routing purposes.", verbose);
}

std::string FrameLib_LookupString::inputInfo(unsigned long idx, bool verbose)
{
    if (idx)
        return parameterInputInfo(verbose);
    else
        return formatInfo("Input", idx);

}

std::string FrameLib_LookupString::outputInfo(unsigned long idx, bool verbose)
{
    return "Output";
}

// Parameter Info

FrameLib_LookupString::ParameterInfo FrameLib_LookupString::sParamInfo;

FrameLib_LookupString::ParameterInfo::ParameterInfo()
{
    const int strBufSize = 64;
    char str[strBufSize];
    
    add("Sets the number of items.");
    add("Sets the output tag.");

    add("Sets the behaviour when empty frames are received: "
        "ignore - empty frames are ignored. "
        "reset - empty frames create empty tags to reset parameters to defaults.");

    for (int i = 0; i < maxNumItems; i++)
    {
        snprintf(str, strBufSize, "Sets the string for item %d.", i + 1);
        add(str);
    }
}

// Process

void FrameLib_LookupString::process()
{
    unsigned long sizeIn;
    
    EmptyModes mode = mParameters.getEnum<EmptyModes>(kEmptyMode);
    
    const double *input = getInput(0, &sizeIn);
    
    const char *tag = mParameters.getString(kTag);
    const char *str = nullptr;
        
    if (!sizeIn && mode == kIgnore)
        return;
    
    if (sizeIn)
    {
        const double limit = static_cast<double>(mNumItems - 1);
        double index = std::max(0.0, std::min(input[0], limit));
                                   
        str = mParameters.getString(kItems +  static_cast<unsigned long>(index));
        requestOutputSize(0, Serial::calcSize(tag, str));

    }
    else
        requestOutputSize(0, Serial::calcSize(tag, 0UL));
  
    if (allocateOutputs())
    {
        FrameLib_Parameters::Serial *output = getOutput(0);
        
        if (!sizeIn)
            output->write(tag, nullptr, 0);
        else
            output->write(tag, str);
    }
}
