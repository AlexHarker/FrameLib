
#include "FrameLib_Change.h"

// Constructor

FrameLib_Change::FrameLib_Change(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy)
: FrameLib_Processor(context, proxy, nullptr, 1, 1)
{
    mParameters.set(serialisedParameters);
    
    setInputMode(0, true, false, true);
}

// Info

std::string FrameLib_Change::objectInfo(bool verbose)
{
    return formatInfo("Only outputs input frames which differ from the previous input: "
                      "Output is the same length as the input. "
                      "This object can be used to remove repeat frames from a stream.",
                      "Only outputs input frames which differ from the previous input.", verbose);
}

std::string FrameLib_Change::inputInfo(unsigned long idx, bool verbose)
{
    return "Input";
}

std::string FrameLib_Change::outputInfo(unsigned long idx, bool verbose)
{
    return "Output";
}

// Update

void FrameLib_Change::update()
{
    unsigned long sizeIn;
    
    const double *input = getInput(0, &sizeIn);
    
    bool changed = (mLastInput.size() != sizeIn);
        
    for (unsigned long i = 0; i < sizeIn && !changed; i++)
        changed = !((input[i] == mLastInput[i]) || (std::isnan(input[i]) && std::isnan(input[i])));
        
    updateTrigger(0, changed);
}

// Process

void FrameLib_Change::process()
{
    unsigned long sizeIn;
    
    const double *input = getInput(0, &sizeIn);
    
    mLastInput = allocAutoArray<double>(sizeIn);
    
    if (mLastInput.size())
        copyVector(mLastInput.data(), input, sizeIn);
    
    prepareCopyInputToOutput(0, 0);
    allocateOutputs();
    copyInputToOutput(0, 0);
}
