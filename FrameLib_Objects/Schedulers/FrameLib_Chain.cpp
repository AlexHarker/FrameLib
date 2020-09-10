
#include "FrameLib_Chain.h"
#include "FrameLib_Sort_Functions.h"

FrameLib_Chain::FrameLib_Chain(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy)
: FrameLib_Scheduler(context, proxy, &sParamInfo, 2, 1)
, mPosition(0)
{
    mParameters.addEnum(kUnits, "units", 1);
    mParameters.addEnumItem(kSamples, "samples");
    mParameters.addEnumItem(kMS, "ms");
    mParameters.addEnumItem(kSeconds, "seconds");
    
    mParameters.addEnum(kMode, "mode", 2);
    mParameters.addEnumItem(kSamples, "absolute");
    mParameters.addEnumItem(kMS, "relative");
    
    mParameters.set(serialisedParameters);
    
    setParameterInput(0);
}

// Info

std::string FrameLib_Chain::objectInfo(bool verbose)
{
    return formatInfo("Schedules frames in specified times according to the input frame: The output is an empty frame.",
                   "Schedules frames in specified times according to the input frame.", verbose);
}

std::string FrameLib_Chain::inputInfo(unsigned long idx, bool verbose)
{
    if (idx)
        return parameterInputInfo(verbose);
    else
        return "Values in the input are interpreted as absolute or relative timings for future frames";
}

std::string FrameLib_Chain::outputInfo(unsigned long idx, bool verbose)
{
    return "Empty Trigger Frames";
}

// Parameter Info

FrameLib_Chain::ParameterInfo FrameLib_Chain::sParamInfo;

FrameLib_Chain::ParameterInfo::ParameterInfo()
{
    add("Sets the time units used.");
    add("Sets the mode for timings (absolute or relative)");
}

// Object Reset

void FrameLib_Chain::objectReset()
{
    mTimes = allocAutoArray<FrameLib_TimeFormat>(0);
    mPosition = 0;
    mSize = 0;
}

// Schedule

FrameLib_TimeFormat FrameLib_Chain::convertTime(double time, Units units)
{
    switch (units)
    {
        case kMS:       return msToSamples(FrameLib_TimeFormat(time));
        case kSeconds:  return secondsToSamples(FrameLib_TimeFormat(time));
        case kSamples:  return FrameLib_TimeFormat(time);
    }
    
    return FrameLib_TimeFormat();
}

FrameLib_Chain::SchedulerInfo FrameLib_Chain::schedule(bool newFrame, bool noAdvance)
{
    FrameLib_TimeFormat now = getCurrentTime();
    FrameLib_TimeFormat nextTime;
    
    unsigned long sizeIn;
    
    // Deal with a new input frame
    
    if (getInputFrameTime(0) == now)
    {
        const double* input = getInput(0, &sizeIn);
     
        mTimes = allocAutoArray<FrameLib_TimeFormat>(sizeIn);
        mSize = mTimes ? sizeIn : 0;
        mPosition = 0;
        
        Units units = mParameters.getEnum<Units>(kUnits);
                                         
        if (mTimes && mSize)
        {
            // FIX - no enum for modes
            
            if (mParameters.getInt(kMode))
            {
                for (unsigned long i = 0; i < mSize; i++)
                    mTimes[i] = convertTime(input[i], units) + FrameLib_TimeFormat(1);
  
                sortAscending(mTimes.get(), mSize);
            }
            else
            {
                FrameLib_TimeFormat accum = now;
            
                for (unsigned long i = 0; i < mSize; i++)
                    mTimes[i] = accum += convertTime(input[i], units);
            }
            
            if (mTimes[0] == now)
                newFrame = true;
        }
    }
    
    // Get the next time
    
    if (mTimes)
    {
        for (nextTime = mTimes[mPosition]; (nextTime <= now) && (mPosition < mSize - 1); )
        {
            nextTime = mTimes[++mPosition];
        }
    }
    
    if (now < nextTime)
        return SchedulerInfo(nextTime - now, newFrame, true);
    
    FrameLib_TimeFormat inputTime = getInputTime();
    FrameLib_TimeFormat blockEndTime = getBlockEndTime();
    
    if (blockEndTime < inputTime)
        return SchedulerInfo(blockEndTime - now, newFrame, false);
    
    if (now < inputTime)
        return SchedulerInfo(inputTime - now, newFrame, false);

    return SchedulerInfo();
}
