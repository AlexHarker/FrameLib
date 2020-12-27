
#include "FrameLib_Pattern.h"

// Constructor

FrameLib_Pattern::FrameLib_Pattern(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy)
: FrameLib_Processor(context, proxy, &sParamInfo, 3, 1), mPosition(0)
{
    mParameters.addEnum(kMode, "mode", 0);
    mParameters.addEnumItem(kRun, "run");
    mParameters.addEnumItem(kLoop, "loop");
    mParameters.addEnumItem(kPause, "pause");
    mParameters.addEnumItem(kStop, "stop");
    
    mParameters.addEnum(kDirection, "direction", 1);
    mParameters.addEnumItem(kForward, "forward");
    mParameters.addEnumItem(kBackward, "backward");
    
    mParameters.addInt(kReset, "reset", -1, 2);
    mParameters.setMin(-1);
    
    mParameters.addEnum(kIdleMode, "idle", 3);
    mParameters.addEnumItem(kRepeat, "repeat");
    mParameters.addEnumItem(kEmpty, "empty");
    
    setInputMode(1, false, false, false);
    setInputMode(2, false, false, false);
    addParameterInput();
    
    mParameters.set(serialisedParameters);
}

// Info

std::string FrameLib_Pattern::objectInfo(bool verbose)
{
    return formatInfo("Step through a frame of values (or pattern) one at a time: "
                      "The pattern input takes the pattern frame without triggering output. "
                      "Triggers step through the pattern either forwards or backwards. "
                      "The pattern can be run (once), looped, paused or stopped. "
                      "Triggers at the reset input set the position in the pattern using the reset parameter. "
                      "The position is also reset whenever a new pattern is received.",
                      "Step through a frame of values (or pattern) one at a time.", verbose);
}

std::string FrameLib_Pattern::inputInfo(unsigned long idx, bool verbose)
{
    switch (idx)
    {
        case 0: return formatInfo("Trigger Input - triggers output", "Trigger Input", verbose);;
        case 1: return formatInfo("Pattern Input - set the pattern without output", "Pattern Input", verbose);
        case 2: return formatInfo("Reset Input - resets without output", "Reset Input", verbose);
        default: return parameterInputInfo(verbose);
    }
}

std::string FrameLib_Pattern::outputInfo(unsigned long idx, bool verbose)
{
    return "Output";
}

// Parameter Info

FrameLib_Pattern::ParameterInfo FrameLib_Pattern::sParamInfo;

FrameLib_Pattern::ParameterInfo::ParameterInfo()
{
    add("Sets the mode: "
        "run - the pattern progresses until it hits the final value. "
        "loop - the pattern loops. "
        "pause - the pattern is paused until the mode is changed. "
        "stop - the pattern returns to the reset position until the mode is changed.");
    add("Sets the direction of progress.");
    add("Sets the reset position as a sample index. "
        "A value of -1 can be used for direction independent reset. "
        "For forward direction reading this results in a reset to the first position. "
        "For backward direction reading this results in a reset to the last position.");
    add("Sets the idle output (the output when stopped, paused or out of bounds): "
        "repeat - repeat the current value. "
        "empty - output an empty frame.");
}

// Object Reset

void FrameLib_Pattern::objectReset()
{
    mPosition = -1;
    mLastResetTime = FrameLib_TimeFormat(0);
}

// Process

void FrameLib_Pattern::process()
{
    Direction dir = mParameters.getEnum<Direction>(kDirection);
    Modes mode = mParameters.getEnum<Modes>(kMode);
    bool reset = mPosition == -1;
    bool idle = false;
    unsigned long sizeIn, sizeOut;
    
    const double *input = getInput(1, &sizeIn);
    
    const long limit = std::max(static_cast<long>(sizeIn), 1L);
    
    switch (mode)
    {
        case kRun:
            mPosition = (dir == kForward) ? mPosition + 1 : mPosition - 1;
            break;
            
        case kLoop:
            if (dir == kForward)
                mPosition = ++mPosition < limit ? mPosition : 0;
            else
                mPosition = --mPosition < 0 ? limit - 1 : mPosition;
            break;
            
        case kPause:
            idle = true;
            break;
            
        case kStop:
            mPosition = mParameters.getInt(kReset);
            idle = true;
            break;
    }
    
    if (reset || mLastResetTime != getInputFrameTime(2))
    {
        mLastResetTime = getInputFrameTime(2);
        mPosition = mParameters.getInt(kReset);
        
        if (mPosition == -1)
        {
            if (dir == kForward)
                mPosition = 0;
            else
                mPosition = limit - 1;
        }
    }

    // Can't reliably check run idle state until here
    
    if (mode == kRun)
        idle = mPosition < 0 || mPosition >= limit;
    
    // Now we safely clip the position
    
    mPosition = std::min(std::max(0L, mPosition), limit - 1);

    sizeOut = idle && mParameters.getEnum<IdleModes>(kIdleMode) == kEmpty ? 0 : 1;
    requestOutputSize(0, sizeOut);
    
    if (allocateOutputs())
    {
        double *output = getOutput(0, &sizeOut);
        output[0] = input ? input[mPosition] : 0.0;
    }
}
