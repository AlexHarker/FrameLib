
#include "FrameLib_Pattern.h"

// Constructor

FrameLib_Pattern::FrameLib_Pattern(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy) : FrameLib_Processor(context, proxy, &sParamInfo, 3, 1), mPosition(0)
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
    return formatInfo("Splits a frame sequentially into smaller frames based on the trigger input.", verbose);
}

std::string FrameLib_Pattern::inputInfo(unsigned long idx, bool verbose)
{
    switch (idx)
    {
        case 0: return "Trigger Input";
        case 1: return "Pattern Input";
        default:
            return parameterInputInfo(verbose);
    }
}

std::string FrameLib_Pattern::outputInfo(unsigned long idx, bool verbose)
{
    return "Current Frame From Pattern";
}

// Parameter Info

FrameLib_Pattern::ParameterInfo FrameLib_Pattern::sParamInfo;

FrameLib_Pattern::ParameterInfo::ParameterInfo()
{
    add("Set the playback mode. once - the pattern is played once only. loop - the pattern loops infinitely." );
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
    Direction dir = static_cast<Direction>(mParameters.getInt(kDirection));
    Modes mode = static_cast<Modes>(mParameters.getInt(kMode));
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

    sizeOut = idle && mParameters.getInt(kIdleMode) == kEmpty ? 0 : 1;
    requestOutputSize(0, sizeOut);
    
    if (allocateOutputs())
    {
        double *output = getOutput(0, &sizeOut);
        output[0] = input ? input[mPosition] : 0.0;
    }
}
