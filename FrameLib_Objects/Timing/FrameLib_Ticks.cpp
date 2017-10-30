
#include "FrameLib_Ticks.h"

FrameLib_Ticks::FrameLib_Ticks(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner) : FrameLib_Processor(context, owner, &sParamInfo, 2, 1)
{
    mParameters.addInt(kLimit, "limit", 10, 0);
    mParameters.setMin(0);
    
    mParameters.addBool(kLimitMode, "restart", false, 1);
    
    mParameters.addBool(kPause, "pause", false, 2);
    
    mParameters.addBool(kStop, "stop", false, 3);
    
    mParameters.set(serialisedParameters);
    setParameterInput(1);
    
    counter = 0;
    previousLimit = 0;
}

std::string FrameLib_Ticks::objectInfo(bool verbose)
{
    return formatInfo("Counts how many frames have passed up to a specified value (0 based).",
                   "Counts how many frames have passed up to a specified value (0 based).", verbose);
}

std::string FrameLib_Ticks::inputInfo(unsigned long idx, bool verbose)
{
    if(idx)
        return formatInfo("Parameter Input", "Parameter Input", verbose);
    else
        return formatInfo("Trigger Input", "Trigger Input", verbose);
}

std::string FrameLib_Ticks::outputInfo(unsigned long idx, bool verbose)
{
    return "Output Values";
}

// Parameter Info

FrameLib_Ticks::ParameterInfo FrameLib_Ticks::sParamInfo;

FrameLib_Ticks::ParameterInfo::ParameterInfo()
{
    add("Sets the counter threshold.");
    add("Sets mode when limit changes, false keeps the counting progress, true restarts from 0.");
    add("Pause the counting at the current value.");
    add("Resets to 0 and holds.");
}

void FrameLib_Ticks::process()
{
    requestOutputSize(0, 1);
    if (allocateOutputs())
    {
        unsigned long sizeOut;
        double *output = getOutput(0, &sizeOut);
        unsigned long currentLimit = mParameters.getValue(kLimit);
        
        if(mParameters.getValue(kStop) == true)
        {
            counter = 0;
        }
        
        if(currentLimit != previousLimit && mParameters.getValue(kLimitMode) == true)
        {
            counter = 0;
        }
        
        if(counter >= currentLimit)
        {
            counter = 0;
        }
        
        if(mParameters.getValue(kPause) == true)
            output[0] = counter;
        else
        {
            output[0] = counter;
            counter++;
        }
        previousLimit = currentLimit;
    }
}
