#include "FrameLib_NonZero.h"

// Constructor

FrameLib_NonZero::FrameLib_NonZero(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy) : FrameLib_Processor(context, proxy, nullptr, 1, 1)
{}

// Info

std::string FrameLib_NonZero::objectInfo(bool verbose)
{
    return formatInfo("Outputs the indices of input samples that are non-zero. "
                      "The length of the output frame depends on the number of non-zero items in the input.",
                      "Outputs the indices of input samples that are non-zero.", verbose);
}

std::string FrameLib_NonZero::inputInfo(unsigned long idx, bool verbose)
{
    return "Input";
}

std::string FrameLib_NonZero::outputInfo(unsigned long idx, bool verbose)
{
    return "Indices";
}

// Process

void FrameLib_NonZero::process()
{
    unsigned long sizeIn;
    unsigned long sizeOut = 0;
    
    const double *input = getInput(0, &sizeIn);
    
    // Allocate temporary memory for working to avoid double looping (cheaper for large lists)
    
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
