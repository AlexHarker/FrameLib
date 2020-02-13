
#include "FrameLib_Ticks.h"

FrameLib_Ticks::FrameLib_Ticks(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy) : FrameLib_Processor(context, proxy, &sParamInfo, 3, 1)
{
    mParameters.addInt(kLimit, "limit", 10, 0);
    mParameters.setMin(0);
    
    mParameters.addEnum(kDirection, "direction", 1);
    mParameters.addEnumItem(kUp, "up");
    mParameters.addEnumItem(kDown, "down");
    
    mParameters.addInt(kReset, "reset", 0, 2);
    mParameters.setMin(0);

    mParameters.addEnum(kMode, "mode", 3);
    mParameters.addEnumItem(kRun, "run");
    mParameters.addEnumItem(kLoop, "loop");
    mParameters.addEnumItem(kPause, "pause");
    mParameters.addEnumItem(kStop, "stop");
    
    mParameters.set(serialisedParameters);
    
    if (!mParameters.changed(kReset) && mParameters.getInt(kDirection) == kDown)
        mParameters.set(kReset, mParameters.getInt(kLimit) - 1);
    
    setInputMode(1, false, false, false);
    
    setParameterInput(2);
    
    mCounter = 0;
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
    add("Sets the reset value.");
    add("Sets the mode. "
         "run - the count continues until it hits the limit or zero. "
         "loop - the count loops between zeor and the limit minus one. "
         "pause - the count is paused until the mode is changed. "
         "stop - the count returns to the reset value until the mode is changed. "
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
    Direction dir = static_cast<Direction>(mParameters.getInt(kDirection));
    Mode mode = static_cast<Mode>(mParameters.getInt(kMode));
    long limit = mParameters.getInt(kLimit);
    bool reset = mCounter == -1;
    
    requestOutputSize(0, 1);

    // Necessary in case we start counting down without a reset
    
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
            break;
            
        case kStop:
            mCounter = mParameters.getInt(kReset);
            break;
    }
        
    if (reset || mLastResetTime != getInputFrameTime(1))
    {
        mLastResetTime = getInputFrameTime(1);
        mCounter = mParameters.getInt(kReset);
    }
    
    if (allocateOutputs())
    {
        unsigned long sizeOut;
        double *output = getOutput(0, &sizeOut);
        output[0] = mCounter = std::min(std::max(0L, mCounter), limit - 1);
    }
}
