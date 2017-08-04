
#include "FrameLib_Register.h"

// FIX - needs to work for either frame type...

// Constructor

FrameLib_Register::FrameLib_Register(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner) : FrameLib_Processor(context, &sParamInfo, 2, 1)
{
    mParameters.addEnum(kMode, "mode", 0);
    mParameters.addEnumItem(kStore, "store");
    mParameters.addEnumItem(kPass, "pass");
    mParameters.setInstantiation();
        
    mParameters.set(serialisedParameters);
    
    Modes mode = (Modes) mParameters.getInt(kMode);
    
    if (mode == kStore)
        inputMode(1, false, false, false, kFrameAny);
    else
        inputMode(1, false, true, false);
}

// Info

std::string FrameLib_Register::objectInfo(bool verbose)
{
    return getInfo("Store and recall a vector frame locally: The left input triggers recall, whilst the right input stores (with or without output).",
                   "Store and recall a vector frame locally.", verbose);
}

std::string FrameLib_Register::inputInfo(unsigned long idx, bool verbose)
{
    if (idx)
        return getInfo("Frame to Store - output is optional based on the mode parameter", "Frame to Store", verbose);
    else
        return getInfo("Trigger Input", "Trigger Input", verbose);
}

std::string FrameLib_Register::outputInfo(unsigned long idx, bool verbose)
{
    return "Output Frames";
}

// Parameter Info

FrameLib_Register::ParameterInfo FrameLib_Register::sParamInfo;

FrameLib_Register::ParameterInfo::ParameterInfo()
{
    add("Sets the behaviour of the right input: "
        "store - stores without output. "
        "pass - stores and outputs.");
}

// Process

void FrameLib_Register::process()
{
    // Get Register Input (we can ignore the first input as it is just a trigger)
    
    unsigned long sizeIn, sizeOut;
    double *input = getInput(1, &sizeIn);
    
    requestOutputSize(0, sizeIn);
    allocateOutputs();
    
    double *output = getOutput(0, &sizeOut);
    
    // Copy to output
    
    copyVector(output, input, sizeOut);
}