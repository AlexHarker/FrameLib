
#include "FrameLib_Register.h"

// Constructor

FrameLib_Register::FrameLib_Register(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy)
: FrameLib_Processor(context, proxy, &sParamInfo, 2, 1)
{
    mParameters.addEnum(kMode, "mode", 0);
    mParameters.addEnumItem(kStore, "store");
    mParameters.addEnumItem(kPass, "pass");
    mParameters.setInstantiation();
        
    mParameters.set(serialisedParameters);
    
    Modes mode = mParameters.getEnum<Modes>(kMode);
    
    if (mode == kStore)
        setInputMode(1, false, false, false, kFrameAny);
    else
        setInputMode(1, false, true, false, kFrameAny);
}

// Info

std::string FrameLib_Register::objectInfo(bool verbose)
{
    return formatInfo("Store and recall frames locally: "
                      "The left input triggers recall, whilst the right input stores a frame. "
                      "The mode paramter is used to set whether the right input causes output.",
                      "Store and recall frames locally.", verbose);
}

std::string FrameLib_Register::inputInfo(unsigned long idx, bool verbose)
{
    if (idx)
        return "Frame to Store";
    else
        return "Trigger Input";
}

std::string FrameLib_Register::outputInfo(unsigned long idx, bool verbose)
{
    return "Output";
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
    // Copy register input (we can ignore the first input as it is just a trigger)
    
    prepareCopyInputToOutput(1, 0);
    allocateOutputs();
    copyInputToOutput(1, 0);
}
