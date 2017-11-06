
#include "FrameLib_FilterParam.h"

// Constructor

FrameLib_FilterParam::FrameLib_FilterParam(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner) : FrameLib_Processor(context, owner, &sParamInfo, 1, 2)
{
    char argStr[10];
    char nameStr[10];
    
    mParameters.addInt(kNumFilters, "num_filters", 1);
    mParameters.setClip(1, maxNumFilters);
    mParameters.setInstantiation();
    
    // Read in once to get number of strings needed
    
    mParameters.set(serialisedParameters);
    
    // If no number of inputs is specified explicityly then examine the serialised parameters to determine the number needed
    
    if (!mParameters.changed(kNumFilters))
    {
        for (int i = 0; i < maxNumFilters; i++)
        {
            sprintf(argStr, "%d", i);
            sprintf(nameStr, "param_%02d", i + 1);
            if (serialisedParameters->find(argStr) != serialisedParameters->end() || serialisedParameters->find(nameStr) != serialisedParameters->end())
                mParameters.set(kNumFilters, (long) (i + 1));
        }
    }
    
    // Read number of ins and setup parameters
    
    mNumFilters = mParameters.getInt(kNumFilters);
    
    for (int i = 0; i < mNumFilters; i++)
    {
        sprintf(nameStr, "param_%02d", i + 1);
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

std::string FrameLib_FilterParam::objectInfo(bool verbose)
{
    return formatInfo("Filters tagged frames into two parts given a number of parameter names: "
                      "The filtered output contains any items from the input that match the given parameterparameter namesnames. "
                      "The residual output contains any items from the input that do not match the given tags",
                      "Filters tagged frames into two parts given a number of parameter names.", verbose);
}

std::string FrameLib_FilterParam::inputInfo(unsigned long idx, bool verbose)
{
    return formatInfo("Parameter Input # - takes tagged input", "Parameter Input #", idx, verbose);
}

std::string FrameLib_FilterParam::outputInfo(unsigned long idx, bool verbose)
{
    if (idx)
        return formatInfo("Residual Output", "Residual Output", verbose);
    else
        return formatInfo("Filtered Output", "Filtered Output", verbose);
}

// Parameter Info

FrameLib_FilterParam::ParameterInfo FrameLib_FilterParam::sParamInfo;

FrameLib_FilterParam::ParameterInfo::ParameterInfo()
{
    add("Sets the number of parameter names.");
}

// Filter

bool FrameLib_FilterParam::filter(FrameLib_Parameters::Serial::Iterator &it)
{
    for (int i = 0; i < mNumFilters; i++)
        if (it.matchTag(mParameters.getString(kFilters + i)))
            return true;
    
    return false;
}

// Process

void FrameLib_FilterParam::process()
{
    unsigned long sizeOut1 = 0;
    unsigned long sizeOut2 = 0;
    
    FrameLib_Parameters::Serial *input = getInput(0);
    
    for (FrameLib_Parameters::Serial::Iterator it = input->begin(); it != input->end(); it++)
    {
        unsigned long size = it.getSize();
        
        if (filter(it))
            sizeOut1 += size;
        else
            sizeOut2 += size;
    }
    
    requestOutputSize(0, sizeOut1);
    requestOutputSize(1, sizeOut2);
    allocateOutputs();
    
    FrameLib_Parameters::Serial *output1 = getOutput(0);
    FrameLib_Parameters::Serial *output2 = getOutput(1);
    
    for (FrameLib_Parameters::Serial::Iterator it = input->begin(); it != input->end(); it++)
    {
        if (filter(it) && output1)
            output1->write(it);
        else if (output2)
            output2->write(it);
    }
}
