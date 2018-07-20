
#include "FrameLib_PhaseWrap.h"
#include <cmath>

// Constructor

FrameLib_PhaseWrap::FrameLib_PhaseWrap(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy) : FrameLib_Processor(context, proxy, nullptr, 1, 1)
{}

// Info

std::string FrameLib_PhaseWrap::objectInfo(bool verbose)
{
    return formatInfo("PhaseWraps the order of a single frame's contents.", verbose);
}

std::string FrameLib_PhaseWrap::inputInfo(unsigned long idx, bool verbose)
{
    return formatInfo("Input Frame", "Input Frame", verbose);
}

std::string FrameLib_PhaseWrap::outputInfo(unsigned long idx, bool verbose)
{
    return "PhaseWrapd Frame";
}

// Process

void FrameLib_PhaseWrap::process()
{

    double neg_pi = M_PI * -1;
	unsigned long size;
	const double *input = getInput(0, &size);
    double temp_val;

	requestOutputSize(0, size);
	allocateOutputs();

	double *output = getOutput(0, &size);
    
    for (unsigned long i = 0; i < size; i++) {
        temp_val = input[i];
        
        while (temp_val > M_PI | temp_val < neg_pi * -1) {
            if (temp_val > M_PI) {
                temp_val = neg_pi + (temp_val - M_PI);
            }
            else if (temp_val < neg_pi) {
                temp_val = M_PI - (neg_pi - input[i]);
            }
         output[i] = temp_val;
        }
    }
    
}
