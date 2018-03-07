#include "FrameLib_Rev.h"

// Constructor

FrameLib_Rev::FrameLib_Rev(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner) : FrameLib_Processor(context, owner, &sParamInfo, 1, 1)
{}

// Info //

// General Info

std::string FrameLib_Rev::objectInfo(bool verbose)
{
    return formatInfo("Reverses the order of a single frame's contents.", verbose);
}

// Input Frames

std::string FrameLib_Rev::inputInfo(unsigned long idx, bool verbose)
{
    return formatInfo("Input Frames", "Input Frames", verbose);
}

// Output Frames

std::string FrameLib_Rev::outputInfo(unsigned long idx, bool verbose)
{
    return "Reversed Frame";
}

// Parameter Info

FrameLib_Rev::ParameterInfo FrameLib_Rev::sParamInfo;

FrameLib_Rev::ParameterInfo::ParameterInfo()
{
    
}

// Process //

void FrameLib_Rev::process()
{
	unsigned long size;
	const double *input = getInput(0, &size);

	requestOutputSize(0, size);
	allocateOutputs();

	double *output = getOutput(0, &size);

	for (unsigned long i = size; i > 0; i--) {
        output[i - 1] = input[size - i];
	}

}
