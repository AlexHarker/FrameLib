
#include "FrameLib_SortMultiple.h"
#include "FrameLib_Sorting_Functions.h"

// Constructor

FrameLib_SortMultiple::FrameLib_SortMultiple(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy)
: FrameLib_Processor(context, proxy, &sParamInfo, 1, 1)
{
    mParameters.addInt(kNumIO, "num_io", 2, 0);
    mParameters.setClip(1, maxNumIO);
    mParameters.setInstantiation();
    
    mParameters.addInt(kSortInput, "input", 1, 1);
    mParameters.setMin(1);

    mParameters.addEnum(kOrder, "order", 2);
    mParameters.addEnumItem(kUp, "up");
    mParameters.addEnumItem(kDown, "down");
    
    mParameters.addEnum(kMismatch, "mismatch", 0);
    mParameters.addEnumItem(kPad, "pad");
    mParameters.addEnumItem(kShrink, "shrink");
    mParameters.addEnumItem(kExtend, "extend");
    
    mParameters.addDouble(kPadding, "pad", 0.0);

    mParameters.set(serialisedParameters);
    
    // Setup IO
    
    mNumIO = mParameters.getInt(kNumIO);
    setIO(mNumIO, mNumIO);
    
    addParameterInput();
}

// Info

std::string FrameLib_SortMultiple::objectInfo(bool verbose)
{
    return formatInfo("Sorts multiple input frames together based on one input: "
                      "The sort can be performed based on any input. "
                      "Sorting order can be ascending or descending.",
                      "However, the output will be no longer the sorting input."
                      "Sorts multiple input frames together based on one input.", verbose);
}

std::string FrameLib_SortMultiple::inputInfo(unsigned long idx, bool verbose)
{
    if (idx == mNumIO)
        return parameterInputInfo(verbose);
    else
        return formatInfo("Input #", idx);
}

std::string FrameLib_SortMultiple::outputInfo(unsigned long idx, bool verbose)
{
    return formatInfo("Output #", idx);
}

// Parameter Info

FrameLib_SortMultiple::ParameterInfo FrameLib_SortMultiple::sParamInfo;

FrameLib_SortMultiple::ParameterInfo::ParameterInfo()
{
    add("Sets the number of inputs and outputs.");
    add("Sets the input to sort on.");
    add("Sets the ordering of the sort.");
    add("Sets the mode used when dealing with mismatched input lengths: "
        "pad - inputs smaller than the sorting input are padded with the padding value. "
        "shrink - all inputs are treated as if the same length as the shortest input. "
        "extend - inputs smaller than the sorting input are extended by repeating their final value.");
    add("Sets the padding value.");
}

// Process

void FrameLib_SortMultiple::process()
{
    MismatchModes mismatch = mParameters.getEnum<MismatchModes>(kMismatch);
    double defaultValue = mParameters.getValue(kPadding);
    
    unsigned long size, sizeIn, sizeMin = 0, sizeMax = 0;
    long sortIdx = mParameters.getInt(kSortInput) - 1;
    
    sortIdx = std::max(0L, std::min(mNumIO - 1, sortIdx));
    
    const double *sortInput = getInput(sortIdx, &sizeIn);

    for (long i = 0; i < mNumIO; i++)
    {
        getInput(i, &size);
        sizeMin = i ? std::min(sizeMin, size) : size;
        sizeMax = i ? std::max(sizeMax, size) : size;
    }
    
    switch (mismatch)
    {
        case kPad:
        case kExtend:
            size = sizeMax;
            break;
            
        case kShrink:
            size = sizeMin;
            break;
    }

    // Limit size based on the sorting input
    
    size = std::min(sizeIn, size);
    
    // Allocate inputs
    
    for (long i = 0; i < mNumIO; i++)
        requestOutputSize(i, size);
    
    allocateOutputs();
    
    if (!size)
        return;
    
    auto indices = allocAutoArray<unsigned long>(size);

    // FIX - sort mismatches
    
    if (indices)
    {
        // Sort on the input
        
        switch (mParameters.getEnum<Orders>(kOrder))
        {
            case kUp:       sortIndicesAscending(indices, sortInput, size);     break;
            case kDown:     sortIndicesDescending(indices, sortInput, size);    break;
        }
        
        // Now order the outputs
        
        for (long i = 0; i < mNumIO; i++)
        {
            double *output = getOutput(i, &size);
            const double *input = getInput(i, &sizeIn);

            MismatchModes inputMismatch = sizeIn >= size ? kShrink : mismatch;
            
            switch (inputMismatch)
            {
                case kShrink:
                    for (long j = 0; j < size; j++)
                        output[j] = input[indices[j]];
                    break;
            
                case kPad:
                    for (long j = 0; j < size; j++)
                    {
                        long idx = indices[j];
                        output[j] = idx < sizeIn ? input[idx] : defaultValue;
                    }
                    break;

                case kExtend:
                    for (long j = 0; j < size; j++)
                        output[j] = input[std::min(indices[j], sizeIn - 1)];
                    break;
            }
        }
    }
    else
    {
        for (long i = 0; i < mNumIO; i++)
        {
            double *output = getOutput(i, &size);
            zeroVector(output, size);
        }
        getReporter()(ErrorSource::Object, getProxy(), "couldn't allocate temporary memory");
    }
}
