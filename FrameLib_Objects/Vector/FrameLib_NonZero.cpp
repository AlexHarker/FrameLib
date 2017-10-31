#include "FrameLib_NonZero.h"

// Constructor

FrameLib_NonZero::FrameLib_NonZero(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner) : FrameLib_Processor(context, owner, &sParamInfo, 1, 1)
{}

// Info

std::string FrameLib_NonZero::objectInfo(bool verbose)
{
    return formatInfo("Outputs indices that are non-zero",
                   "Outputs indices that are non-zero", verbose);
}

std::string FrameLib_NonZero::inputInfo(unsigned long idx, bool verbose)
{
    return formatInfo("Input Frames", "Input Frames", verbose);
}

std::string FrameLib_NonZero::outputInfo(unsigned long idx, bool verbose)
{
    return "Indices that are non-zero";
}

// Parameter Info

FrameLib_NonZero::ParameterInfo FrameLib_NonZero::sParamInfo;

FrameLib_NonZero::ParameterInfo::ParameterInfo()
{
    
}

// Process

void FrameLib_NonZero::process()
{
    unsigned long sizeIn;
    unsigned long sizeOut = 0;
    
    double *input = getInput(0, &sizeIn);
    
    // Having more memory allocated just not to loop two times to get and set the size.
    
    double *indices = alloc<double>(sizeIn * sizeof(double));
    
    for (unsigned long i = 0; i < sizeIn; i++)
    {
        if (input[i] != 0)
            indices[sizeOut++] = i;
    }
    
    requestOutputSize(0, sizeOut);
    allocateOutputs();
    double *output = getOutput(0, &sizeOut);

    copyVector(output, indices, sizeOut);
    
    dealloc(indices);
}
