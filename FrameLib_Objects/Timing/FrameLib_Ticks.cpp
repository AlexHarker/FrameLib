
#include "FrameLib_Ticks.h"

FrameLib_Ticks::FrameLib_Ticks(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy)
: FrameLib_Processor(context, proxy, &sParamInfo, 3, 1)
, mCounter(0)
{
    mParameters.addInt(kLimit, "limit", 10, 0);
    mParameters.setMin(0);
    
    mParameters.addEnum(kDirection, "direction", 1);
    mParameters.addEnumItem(kUp, "up");
    mParameters.addEnumItem(kDown, "down");
    
    mParameters.addInt(kReset, "reset", -1, 2);
    mParameters.setMin(-1);

    mParameters.addEnum(kMode, "mode", 3);
    mParameters.addEnumItem(kRun, "run");
    mParameters.addEnumItem(kLoop, "loop");
    mParameters.addEnumItem(kPause, "pause");
    mParameters.addEnumItem(kStop, "stop");
    
    mParameters.addEnum(kIdleMode, "idle", 4);
    mParameters.addEnumItem(kRepeat, "repeat");
    mParameters.addEnumItem(kEmpty, "empty");
    
    mParameters.set(serialisedParameters);
    
    setInputMode(1, false, false, false);
    
    setParameterInput(2);
}

std::string FrameLib_Ticks::objectInfo(bool verbose)
{
    return formatInfo("Counts incoming frames: "
                      "Counting is between zero and one less than the limit parameter. "
                      "Counting can be either up or down. "
                      "Couting can be run (once), looped, pauseed or stopped. "
                      "Triggers at the reset input set the count to the value of the reset parameter. "
                      "Counts incoming frames.", verbose);
}

std::string FrameLib_Ticks::inputInfo(unsigned long idx, bool verbose)
{
    switch (idx)
    {
        case 0:
            return formatInfo("Trigger Input - triggers output", "Trigger Input", verbose);
        case 1:
            return formatInfo("Reset Input - resets count without triggering output", "Reset Input", verbose);
        default:
            return parameterInputInfo(verbose);
    }
}

std::string FrameLib_Ticks::outputInfo(unsigned long idx, bool verbose)
{
    return "Count";
}

// Parameter Info

FrameLib_Ticks::ParameterInfo FrameLib_Ticks::sParamInfo;

FrameLib_Ticks::ParameterInfo::ParameterInfo()
{
    add("Sets the count limit.");
    add("Sets the direction of the counting.");
    add("Sets the reset value. "
        "A value of -1 can be used for direction independent reset. "
        "This results in reset to 0 when counting up and (limit - 1) when counting down."
        );
    add("Sets the mode. "
         "run - the count continues until it hits the limit or zero. "
         "loop - the count loops between zeor and the limit minus one. "
         "pause - the count is paused until the mode is changed. "
         "stop - the count returns to the reset value until the mode is changed."
        );
    add("Sets the idle mode (the output when stopped, paused or out of bounds). "
        "repeat - repeat the current count. "
        "empty - output an empty frame."
        );
}

// Object Reset

void FrameLib_Ticks::objectReset()
{
    mCounter = -1;
    mLastResetTime = FrameLib_TimeFormat(0);
}

void FrameLib_Ticks::process()
{
    Direction dir = mParameters.getEnum<Direction>(kDirection);
    Modes mode = mParameters.getEnum<Modes>(kMode);
    unsigned long sizeOut;
    long limit = mParameters.getInt(kLimit);
    bool reset = mCounter == -1;
    bool idle = false;
    
    switch (mode)
    {
        case kRun:
            mCounter = (dir == kUp) ? mCounter + 1 : mCounter - 1;
            break;
            
        case kLoop:
            if (dir == kUp)
                mCounter = ++mCounter < limit ? mCounter : 0;
            else
                mCounter = --mCounter < 0 ? limit - 1 : mCounter;
            break;
            
        case kPause:
            idle = true;
            break;
            
        case kStop:
            idle = true;
            mCounter = mParameters.getInt(kReset);
            break;
    }
        
    if (reset || mLastResetTime != getInputFrameTime(1))
    {
        mLastResetTime = getInputFrameTime(1);
        mCounter = mParameters.getInt(kReset);
        
        if (mCounter == -1)
        {
            if (dir == kUp)
                mCounter = 0;
            else
                mCounter = limit - 1;
        }
    }
    
    // Can't reliably check run idle state until here
    
    if (mode == kRun)
        idle = mCounter < 0 || mCounter >= limit;
    
    // Now we clip the counter
    
    mCounter = std::min(std::max(0L, mCounter), limit - 1);
    
    sizeOut = idle && mParameters.getEnum<IdleModes>(kIdleMode) == kEmpty ? 0 : 1;
    requestOutputSize(0, sizeOut);
    
    if (allocateOutputs())
    {
        double *output = getOutput(0, &sizeOut);
        output[0] = mCounter;
    }
}
