
#include "FrameLib_FilterTags.h"

// Constructor

FrameLib_FilterTags::FrameLib_FilterTags(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy)
: FrameLib_Processor(context, proxy, &sParamInfo, 1, 2)
{
    mParameters.addInt(kNumFilters, "num_filters", 1);
    mParameters.setClip(1, maxNumFilters);
    mParameters.setInstantiation();
    
    // Read in once to get number of strings needed
    
    mParameters.setErrorReportingEnabled(false);
    mParameters.set(serialisedParameters);
    mParameters.setErrorReportingEnabled(true);
    
    // If no number of inputs is specified explicity then examine the serialised parameters to determine the number needed
    
    if (!mParameters.changed(kNumFilters))
    {
        for (int i = 0; i < maxNumFilters; i++)
        {
            if (serialisedParameters->find(FrameLib_StringMaker<>(i)) != serialisedParameters->end() || serialisedParameters->find(FrameLib_StringMaker<>("tag_", i + 1, 2)) != serialisedParameters->end())
                mParameters.set(kNumFilters, (long) (i + 1));
        }
    }
    
    // Read number of ins and setup parameters
    
    mNumFilters = mParameters.getInt(kNumFilters);
    
    for (unsigned long i = 0; i < mNumFilters; i++)
        mParameters.addString(kFilters + i, FrameLib_StringMaker<>("tag_", i + 1, 2), i);
    
    // Read in again to get parameter names
    
    mParameters.set(serialisedParameters);
    
    // Setup IO
    
    setInputMode(0, false, true, false, FrameType::Tagged);
    setOutputType(0, FrameType::Tagged);
    setOutputType(1, FrameType::Tagged);
    
    addParameterInput();
}

// Info

std::string FrameLib_FilterTags::objectInfo(bool verbose)
{
    return formatInfo("Filters tagged frames using a set of specified tags: "
                      "The filtered output contains any items from the input that match the given tags. "
                      "The residual output contains any items from the input that do not match the given tags. "
                      "The number of tags can be set explicitly by parameter. "
                      "Alternatively, it can be set implicitly by the tag parameters present at instantiation.",
                      "Filters tagged frames using a set of specified tags.", verbose);
}

std::string FrameLib_FilterTags::inputInfo(unsigned long idx, bool verbose)
{
    if (idx)
        return parameterInputInfo(verbose);
    else
        return "Tagged Input";
}

std::string FrameLib_FilterTags::outputInfo(unsigned long idx, bool verbose)
{
    if (idx)
        return "Residual Output";
    else
        return "Filtered Output";
}

// Parameter Info

FrameLib_FilterTags::ParameterInfo FrameLib_FilterTags::sParamInfo;

FrameLib_FilterTags::ParameterInfo::ParameterInfo()
{
    const int strBufSize = 64;
    char str[strBufSize];
    
    add("Sets the number of tags.");
    
    for (int i = 0; i < maxNumFilters; i++)
    {
        snprintf(str, strBufSize, "Sets tag number %d.", i + 1);
        add(str);
    }
}

// Filter

int FrameLib_FilterTags::filterIndex(FrameLib_Parameters::Serial::Iterator &it)
{
    for (unsigned long i = 0; i < mNumFilters; i++)
        if (it.matchTag(mParameters.getString(kFilters + i)))
            return 0;
    
    return 1;
}

// Process

void FrameLib_FilterTags::process()
{
    const FrameLib_Parameters::Serial *input = getInput(0);

    requestOutputSize(0, 0);
    requestOutputSize(1, 0);
    
    if (!input)
    {
        allocateOutputs();
        return;
    }
    
    for (auto it = input->begin(); it != input->end(); it++)
        requestAddedOutputSize(filterIndex(it), it.getSize());
    
    if (allocateOutputs())
    {
        FrameLib_Parameters::Serial *outputs[2] = {getOutput(0), getOutput(1)};

        for (auto it = input->begin(); it != input->end(); it++)
            outputs[filterIndex(it)]->write(it);
    }
}
