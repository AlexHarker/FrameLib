#include "FrameLib_Find.h"
#include <algorithm>
#include <vector>
#include <limits>

// Constructor

FrameLib_Find::FrameLib_Find(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy) : FrameLib_Processor(context, proxy, &sParamInfo, 2, 1)
{
    mParameters.addDouble(kRange, "range", 0.0, 0);
    mParameters.set(serialisedParameters);
    addParameterInput();
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
    
    // -- // method for finding within a range // -- //
    if (size2 > 0) {
        for (int j = 0; j < size1; j++) {
            if (input1[j] >= (input2[0] - range) && input1[j] <= (input2[0] + range)) {
                requestOutputSize(0, sizeout);
                allocateOutputs();
                double *output = getOutput(0, &sizeout);
                output[0] = j;
                break;
            }
            else {
                requestOutputSize(0, sizeout);
                allocateOutputs();
                double *output = getOutput(0, &sizeout);
                output[0] = -1;
            }
        }
    }
}
