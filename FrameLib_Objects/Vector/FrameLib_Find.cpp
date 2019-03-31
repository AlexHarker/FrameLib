#include "FrameLib_Find.h"
#include <algorithm>
#include <vector>
#include <iostream>
#include <cmath>

// Constructor

FrameLib_Find::FrameLib_Find(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy) : FrameLib_Processor(context, proxy, &sParamInfo, 3, 1)
{
    // setIO(3, 1);
    mParameters.addDouble(kRange, "range", 0.0, 0);
    mParameters.set(serialisedParameters);
//    setParameterInput(1);
}

// Info

std::string FrameLib_Find::objectInfo(bool verbose)
{
    return formatInfo("Finds indices of values in a frame, given by another frame.",
                   "Finds values in a given frame.", verbose);
}

std::string FrameLib_Find::inputInfo(unsigned long idx, bool verbose)
{
    switch (idx)
    {
        case 0: return "Frame to search through";
        case 1: return "Value(s) to search for";
        case 2: return parameterInputInfo(verbose);
    }
}

std::string FrameLib_Find::outputInfo(unsigned long idx, bool verbose)
{
    return "Indices of found values";
}

// Parameter Info

FrameLib_Find::ParameterInfo FrameLib_Find::sParamInfo;

FrameLib_Find::ParameterInfo::ParameterInfo()
{
    add("Sets a range to look either side of your target value.");
}

// Process

void FrameLib_Find::process()
{
    // set size at load or set it after finding?
    unsigned long size1, size2; // second inlet frame will always be 1 value - make it many values later
    unsigned long sizeout = 1;
    double range = mParameters.getValue(kRange);

    // get the inputs of each inlet
    const double *input1 = getInput(0, &size1);
    const double *input2 = getInput(1, &size2);
    
    // request the output size
    requestOutputSize(0, sizeout);
    allocateOutputs();

    double *output = getOutput(0, &sizeout);
    
    if (input2) {
        
        // put the input into temporary memory
        std::vector<double> temp;
        temp.resize(size1);
        for (int i = 0; i < size1; i++) {
            temp[i] = input1[i];
        }
    
        for (int j = 0; j < temp.size(); j++) {
            if (temp[j] >= (input2[0] - range) && temp[j] <= (input2[0] + range)) {
                output[0] = j;
                std::cout << j << "\n";
                std::cout << output[0] << "\n";
            }
            else if (temp[j] < (input2[0] - range) || temp[j] > (input2[0] + range)) {
                output[0] = std::nan("1");
            }
        };
    }
    
}
