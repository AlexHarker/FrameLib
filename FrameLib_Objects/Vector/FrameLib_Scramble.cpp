
#include "FrameLib_Scramble.h"
#include <algorithm>
#include <random>
#include <iterator>

// Constructor

FrameLib_Scramble::FrameLib_Scramble(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy) : FrameLib_Processor(context, proxy, nullptr, 1, 1)
{}

// Info

std::string FrameLib_Scramble::objectInfo(bool verbose)
{
    return "Scrambles the order of a single frame's contents.";
}

std::string FrameLib_Scramble::inputInfo(unsigned long idx, bool verbose)
{
    return "Input Frame";
}

std::string FrameLib_Scramble::outputInfo(unsigned long idx, bool verbose)
{
    return "Scrambled Frame";
}

// Process

void FrameLib_Scramble::process()
{
    unsigned long size;
    const double *input = getInput(0, &size);
    std::random_device rd;
    std::mt19937 g(rd());
    
    // Temporary memory because you cannot past const to the std::random_shuffle
    double *temp = alloc<double>(size);
    copyVector(temp, input, size);
    
    requestOutputSize(0, size);
    allocateOutputs();
    
    double *output = getOutput(0, &size);
    
    if (output) {
        std::shuffle(temp, temp+size, g);
        copyVector(output, temp, size);
    }
    dealloc(temp);
}

