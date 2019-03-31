#include "FrameLib_Find.h"
#include <algorithm>
#include <vector>

// Constructor

FrameLib_Find::FrameLib_Find(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy) : FrameLib_Processor(context, proxy, nullptr, 2, 1)
{
//    mParameters.addInt(kMatches, "matches", 1, 0);
    setIO(2, 1);
//    mParameters.set(serialisedParameters);
    
//    setParameterInput(1);
}

// Info

std::string FrameLib_Find::objectInfo(bool verbose)
{
    return formatInfo("Finds values in a given frame.",
                   "Finds values in a given frame.", verbose);
}

std::string FrameLib_Find::inputInfo(unsigned long idx, bool verbose)
{
    switch (idx)
    {
        case 0: return "Frame to search through";
        case 1: return "Value(s) to search for";
    }
}

std::string FrameLib_Find::outputInfo(unsigned long idx, bool verbose)
{
    switch (idx)
    {
        case 0: return "Indices of found values";
    }
}

// Process

void FrameLib_Find::process()
{
    // set size at load or set it after finding?
    unsigned long size1, size2; // second inlet frame will always be 1 value - make it many values later
    unsigned long sizeout = 1;

    // get the inputs of each inlet
    const double *input1 = getInput(0, &size1);
    const double *input2 = getInput(1, &size2);
    
    // request the output size
    requestOutputSize(0, sizeout);
    allocateOutputs();

    double *output = getOutput(0, &sizeout);
    
    // put the input into temporary memory
    std::vector<double> temp;
    temp.resize(size1);
    for (int i = 0; i < size1; i++) {
        temp[i] = input1[i];
    }
    
    std::vector<double>::iterator it = std::find(temp.begin(), temp.end(), input2[0]);
    
    if (it != temp.end()) {
        // Get index of element from iterator
        int index = std::distance(temp.begin(), it);
        output[0] = index;
    }
}
