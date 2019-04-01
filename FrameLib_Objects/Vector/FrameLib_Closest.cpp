#include "FrameLib_Closest.h"
#include <algorithm>
#include <vector>
#include <limits>

// Constructor

FrameLib_Closest::FrameLib_Closest(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy) : FrameLib_Processor(context, proxy, &sParamInfo, 2, 1)
{
    mParameters.addInt(kMatches, "matches", 1, 0);
    mParameters.set(serialisedParameters);
    addParameterInput();
}

// Info

std::string FrameLib_Closest::objectInfo(bool verbose)
{
    return formatInfo("Finds indices of values in a frame that are closest to a given test value.",
                   "Finds indices of values in a frame that are closest to a given test value.", verbose);
}

std::string FrameLib_Closest::inputInfo(unsigned long idx, bool verbose)
{
    switch (idx)
    {
        case 0: return "Frame to search through";
        case 1: return "Value(s) to compare";
        case 2: return parameterInputInfo(verbose);
    }
}

std::string FrameLib_Closest::outputInfo(unsigned long idx, bool verbose)
{
    return "Indices of found values";
}

// Parameter Info

FrameLib_Closest::ParameterInfo FrameLib_Closest::sParamInfo;

FrameLib_Closest::ParameterInfo::ParameterInfo()
{
    add("How many values you want returned in increasing order of distance.");
}

// Process

void FrameLib_Closest::process()
{
    // set size at load or set it after finding?
    unsigned long size1, size2; // second inlet frame will always be 1 value - make it many values later
    unsigned long num_matches = mParameters.getValue(kMatches);
    unsigned long found_matches = 0;
    unsigned long temp_idx;
    
    // get the inputs of each inlet
    const double *input1 = getInput(0, &size1);
    const double *input2 = getInput(1, &size2);
    

    
    if (size2 != 0) { // only if there is a second input
        
        double *differences = alloc<double>(size1);
        double min = std::numeric_limits<double>::infinity();
        double pos_inf = std::numeric_limits<double>::infinity();
        
        for (unsigned long i = 0; i < size1; i++) {
            differences[i] = abs(input1[i] - input2[0]); //construct the differences / distances
        }
        
        if (num_matches > size1) { // ensures that you dont try to look for more outputs than there can possibly be
            num_matches = size1;
        }
        
        double *min_idx = alloc<double>(num_matches); // allocate space for all of our results
        
        while(found_matches < num_matches) { // while we don't have enough matches
            min = pos_inf;

            for (unsigned long i = 0; i < size1; i++) { // loop over our differences
                if (differences[i] < min) { // if the value in the array is less than our minimum
                    min = differences[i]; // store this as our minimum
                    min_idx[found_matches] = i; //store the index in our min_idx array
                }
            }
            temp_idx = min_idx[found_matches];
            differences[temp_idx] = pos_inf; //why cant this be min_idx[found_matches]
             // set the value at that index to infinity so the secodn minimum will be detected next
            found_matches += 1;
        }
        
        // do output
        requestOutputSize(0, num_matches);
        allocateOutputs();
        double *output = getOutput(0, &num_matches);
        copyVector(output, min_idx, num_matches);
        
        // deallocate the arrays
        dealloc(differences);
        dealloc(min_idx);
    }
}
