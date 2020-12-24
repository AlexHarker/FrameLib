
#include "FrameLib_Sort.h"
#include "FrameLib_Sort_Functions.h"

// Constructor

FrameLib_Sort::FrameLib_Sort(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy)
: FrameLib_Processor(context, proxy, &sParamInfo, 2, 1)
{
    mParameters.addEnum(kOrder, "order", 0);
    mParameters.addEnumItem(kUp, "up");
    mParameters.addEnumItem(kDown, "down");
    
    mParameters.addBool(kOutputIndices, "indices", false, 1);
    
    mParameters.set(serialisedParameters);
    
    setParameterInput(1);
}

// Info

std::string FrameLib_Sort::objectInfo(bool verbose)
{
    return formatInfo("Sorts an input frame into ascending or descending order. "
                      "The sort can either be on values or indices as set by the indices parameter.",
                      "Sorts an input frame into ascending or descending order.", verbose);
}

std::string FrameLib_Sort::inputInfo(unsigned long idx, bool verbose)
{
    if (idx)
        return parameterInputInfo(verbose);
    else
        return "Input";
}

std::string FrameLib_Sort::outputInfo(unsigned long idx, bool verbose)
{
    return "Output";
}

// Parameter Info

FrameLib_Sort::ParameterInfo FrameLib_Sort::sParamInfo;

FrameLib_Sort::ParameterInfo::ParameterInfo()
{
    add("Sets the ordering of the sorted output.");
    add("Performs the sort on the indices of the input rather than the values.");
}

// Process

void FrameLib_Sort::process()
{
    unsigned long size;
    const double *input = getInput(0, &size);
    
    requestOutputSize(0, size);
    allocateOutputs();
    
    double *output = getOutput(0, &size);
    
    if (!mParameters.getBool(kOutputIndices))
    {
        switch (mParameters.getEnum<Orders>(kOrder))
        {
            case kUp:       sortAscending(output, input, size);     break;
            case kDown:     sortDescending(output, input, size);    break;
        }
    }
    else
    {
        auto indices = allocAutoArray<unsigned long>(size);
        
        if (indices)
        {
            switch (mParameters.getEnum<Orders>(kOrder))
            {
                case kUp:       sortIndicesAscending(indices, input, size);     break;
                case kDown:     sortIndicesDescending(indices, input, size);    break;
            }
            
            for (unsigned long i = 0; i < size; i++)
                output[i] = static_cast<double>(indices[i]);
        }
        else
        {
            zeroVector(output, size);
            getReporter()(kErrorObject, getProxy(), "couldn't allocate temporary memory");
        }
    }
}
