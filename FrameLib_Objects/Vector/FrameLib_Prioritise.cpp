
#include "FrameLib_Prioritise.h"

// Constructor

FrameLib_Prioritise::FrameLib_Prioritise(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy) : FrameLib_Processor(context, proxy, &sParamInfo)
{
    mParameters.addInt(kNumIns, "num_ins", 2, 0);
    mParameters.setClip(2, 32);
    mParameters.setInstantiation();
    
    mParameters.set(serialisedParameters);
    
    setIO(mParameters.getInt(kNumIns), 1);
}

// Info

std::string FrameLib_Prioritise::objectInfo(bool verbose)
{
    return formatInfo("Prioritises between incoming inputs: "
                      "When inputs occur at different times all inputs are sent to the output. "
                      "When two or more inputs arrive simultaneously, the leftmost input is chosen.",
                      "Prioritises between incoming inputs.", verbose);
}

std::string FrameLib_Prioritise::inputInfo(unsigned long idx, bool verbose)
{
    return formatInfo("Input #", "Input #", idx, verbose);
}

std::string FrameLib_Prioritise::outputInfo(unsigned long idx, bool verbose)
{
    return "Output";
}

// Parameter Info

FrameLib_Prioritise::ParameterInfo FrameLib_Prioritise::sParamInfo;

FrameLib_Prioritise::ParameterInfo::ParameterInfo()
{
    add("Sets the number of inputs.");
    add("Set which inputs trigger output (by default all inputs trigger output).");
}

// Process

void FrameLib_Prioritise::process()
{
    FrameLib_TimeFormat now = getFrameTime();
    unsigned long input = 0;
        
    // Find the leftmost input
    
    for ( ; input < getNumIns(); input++)
        if (now == getInputFrameTime(input))
            break;
    
    assert(input < getNumIns() && "No current input found");
    
    prepareCopyInputToOutput(input, 0);
    allocateOutputs();
    copyInputToOutput(input, 0);
}
