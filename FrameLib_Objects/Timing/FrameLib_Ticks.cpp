
#include "FrameLib_Ticks.h"

FrameLib_Ticks::FrameLib_Ticks(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner) : FrameLib_Processor(context, owner, &sParamInfo, 2, 1)
{
    mParameters.addInt(kLimit, "limit", 10, 0);
    mParameters.setMin(0);
    
    mParameters.addInt(kSetValue, "value", 0, 0);
    mParameters.setMin(0);
    
    mParameters.addEnum(kLimitMode, "limitmode", 1);
    mParameters.addEnumItem(kProgress, "progress");
    mParameters.addEnumItem(kRestart, "restart");
    
    mParameters.addEnum(kRunMode, "runmode", 2);
    mParameters.addEnumItem(kRun, "run");
    mParameters.addEnumItem(kPause, "pause");
    mParameters.addEnumItem(kStop, "stop");
    
    mParameters.set(serialisedParameters);
    setParameterInput(1);
    
    counter = 0;
    previousLimit = 0;
    previousValue = 0;
    valueInPause = false;
    limitInPause = false;
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
    return "Current Count";
}

// Parameter Info

FrameLib_Ticks::ParameterInfo FrameLib_Ticks::sParamInfo;

FrameLib_Ticks::ParameterInfo::ParameterInfo()
{
    add("Sets the counter threshold.");
    add("Sets a new starting value at next trigger");
    add("Sets the mode for changes in threshold.");
    add("Sets the run mode.");
}

void FrameLib_Ticks::process()
{
    requestOutputSize(0, 1);
    ModesRun mode_run = (ModesRun) mParameters.getValue(kRunMode);
    ModesLimit mode_limit = (ModesLimit) mParameters.getValue(kLimitMode);
    
    if (allocateOutputs())
    {
        unsigned long sizeOut;
        double *output = getOutput(0, &sizeOut);
        unsigned long currentLimit = mParameters.getValue(kLimit);
        unsigned long currentValue = mParameters.getValue(kSetValue);
        
        switch (mode_run)
        {
            case kRun:
                if (currentValue != previousValue || valueInPause == true)
                {
                    if (currentValue < currentLimit)
                        counter = currentValue;
                }
                switch (mode_limit)
                {
                    case kProgress:
                        break;
                    case kRestart:
                        if (currentLimit != previousLimit || limitInPause == true)
                        {
                            counter = 0;
                        }
                        break;
                }
                if (counter >= currentLimit)
                    counter = 0;
                
                output[0] = counter;
                counter++;
                limitInPause = false; //reset states
                valueInPause = false;
                break;
                
            case kPause:
                if (currentValue != previousValue) //means that a new starting value has been set while in pause
                    valueInPause = true;
                
                if (counter >= currentLimit) //it still resets at N-1, so you can't pause at N
                    counter = 0;
                
                output[0] = counter;
                switch (mode_limit)
                {
                    case kProgress:
                        limitInPause = false;
                        break;
                    case kRestart:
                        if(currentLimit != previousLimit) //means that limit has been changed while in pause
                        {
                            limitInPause = true;
                        }
                        break;
                }
                break;
                
            case kStop:
                if(currentValue != previousValue)
                    valueInPause = true;
                
                counter = 0;
                output[0] = counter;
                break;
        }
        
        previousValue = currentValue;
        previousLimit = currentLimit;
    }
}
