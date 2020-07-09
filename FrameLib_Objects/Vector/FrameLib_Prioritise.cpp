
#include "FrameLib_Prioritise.h"

// Constructor

FrameLib_Prioritise::FrameLib_Prioritise(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy)
: FrameLib_Processor(context, proxy, &sParamInfo)
{
    mParameters.addInt(kNumIns, "num_ins", 2, 0);
    mParameters.setClip(2, 32);
    mParameters.setInstantiation();
    
    mParameters.addEnum(kMode, "mode", 1);
    mParameters.addEnumItem(kLow, "low");
    mParameters.addEnumItem(kHigh, "high");
    mParameters.setInstantiation();
    
    mParameters.set(serialisedParameters);
    
    setIO(mParameters.getInt(kNumIns), 1);
}

// Info

std::string FrameLib_Prioritise::objectInfo(bool verbose)
{
    return formatInfo("Apply a fixed priority ranking to synchronous input frames: "
                      "Frames arriving asynchronously are sent to the output. "
                      "When frames arrive synchronously only one is sent to the ouput."
                      "The input number is used to proritise synchonous inputs. "
                      "Either higher or lower numbered inputs can be prioritised.",
                      "Apply a fixed priority ranking to synchronous input frames.", verbose);
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
    add("Set whether to prioritise lower or higher numbered inputs.");
}

// Process

void FrameLib_Prioritise::process()
{
    FrameLib_TimeFormat now = getFrameTime();
    unsigned long input = 0;
        
    // Find the prioritised input
    
    Modes mode = (Modes) mParameters.getInt(kMode);
    
    switch (mode)
    {
        case kLow:
        {
            for ( ; input < getNumIns(); input++)
                if (now == getInputFrameTime(input))
                    break;
            
            assert(input < getNumIns() && "No current input found");
            break;
        }
        
        case kHigh:
        {
            for (input = getNumIns(); input > 0; input--)
                if (now == getInputFrameTime(input - 1))
                    break;
            
            assert(input > 0 && "No current input found");
            input--;
            break;
        }
    }
    
    prepareCopyInputToOutput(input, 0);
    allocateOutputs();
    copyInputToOutput(input, 0);
}
