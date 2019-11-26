
#include "FrameLib_FilterTags.h"

// Constructor

FrameLib_FilterTags::FrameLib_FilterTags(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy) : FrameLib_Processor(context, proxy, &sParamInfo, 1, 2)
{
    const int strBufSize = 10;
    
    char argStr[strBufSize];
    char nameStr[strBufSize];
    
    mParameters.addInt(kNumFilters, "num_filters", 1);
    mParameters.setClip(1, maxNumFilters);
    mParameters.setInstantiation();
    
    // Read in once to get number of strings needed
    
    mParameters.setErrorReportingEnabled(false);
    mParameters.set(serialisedParameters);
    mParameters.setErrorReportingEnabled(true);
    
    // If no number of inputs is specified explicityly then examine the serialised parameters to determine the number needed
    
    if (!mParameters.changed(kNumFilters))
    {
        for (int i = 0; i < maxNumFilters; i++)
        {
            snprintf(argStr, strBufSize, "%d", i);
            snprintf(nameStr, strBufSize, "tag_%02d", i + 1);
            if (serialisedParameters->find(argStr) != serialisedParameters->end() || serialisedParameters->find(nameStr) != serialisedParameters->end())
                mParameters.set(kNumFilters, (long) (i + 1));
        }
    }
    
    // Read number of ins and setup parameters
    
    mNumFilters = mParameters.getInt(kNumFilters);
    
    for (unsigned long i = 0; i < mNumFilters; i++)
    {
        snprintf(nameStr, strBufSize, "tag_%02lu", i + 1);
        mParameters.addString(kFilters + i, nameStr, i);
        mParameters.setInstantiation();
    }
    
    // Read in again to get parameter names
    
    mParameters.set(serialisedParameters);
    
    // Setup IO
    
    setInputMode(0, false, true, false, kFrameTagged);
    setOutputType(0, kFrameTagged);
    setOutputType(1, kFrameTagged);
}

// Info

std::string FrameLib_FilterTags::objectInfo(bool verbose)
{
    return formatInfo("Filters tagged frames into two parts given a number of tags: "
                      "The number of tags is specified either explicitly with a parameter or implicitly by which arguments or tag parameters are present. "
                      "The filtered output contains any items from the input that match the given tags. "
                      "The residual output contains any items from the input that do not match the given tags",
                      "Filters tagged frames into two parts given a number of tags.", verbose);
}

std::string FrameLib_FilterTags::inputInfo(unsigned long idx, bool verbose)
{
    return formatInfo("Parameter Input # - takes tagged input", "Parameter Input #", idx, verbose);
}

std::string FrameLib_FilterTags::outputInfo(unsigned long idx, bool verbose)
{
    if (idx)
        return formatInfo("Residual Output", "Residual Output", verbose);
    else
        return formatInfo("Filtered Output", "Filtered Output", verbose);
}

// Parameter Info

FrameLib_FilterTags::ParameterInfo FrameLib_FilterTags::sParamInfo;

FrameLib_FilterTags::ParameterInfo::ParameterInfo()
{
    add("Sets the number of parameter names.");
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
