
#include "FrameLib_Chain.h"
#include "FrameLib_Sort_Functions.h"

FrameLib_Chain::FrameLib_Chain(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy)
: FrameLib_Scheduler(context, proxy, &sParamInfo, 2, 2)
, mPosition(0)
{
    mParameters.addEnum(kUnits, "units", 1);
    mParameters.addEnumItem(kSamples, "samples");
    mParameters.addEnumItem(kMS, "ms");
    mParameters.addEnumItem(kSeconds, "seconds");
    
    mParameters.addEnum(kTimeMode, "time", 2);
    mParameters.addEnumItem(kAbsolute, "absolute");
    mParameters.addEnumItem(kRelative, "relative");
    mParameters.addEnumItem(kInterval, "interval", true);
    
    mParameters.addEnum(kMode, "mode", 3);
    mParameters.addEnumItem(kReplace, "replace");
    mParameters.addEnumItem(kAdd, "add");
    mParameters.addEnumItem(kAppend, "append");

    mParameters.set(serialisedParameters);
    
    setParameterInput(1);
}

// Info

std::string FrameLib_Chain::objectInfo(bool verbose)
{
    return formatInfo("Schedules frames in the future at times specified times in the input frame: "
                      "The output is an empty trigger frame. "
                      "Inputs interact with previously scheduled frames according to time and mode parameters."
                      "The remaining count output can be used to track the number of currently scheduled frames.",
                      "Schedules frames in the future at times specified times in the input frame.", verbose);
}

std::string FrameLib_Chain::inputInfo(unsigned long idx, bool verbose)
{
    if (idx)
        return parameterInputInfo(verbose);
    else
        return formatInfo("Input - values are interpreted as timings for future frames", "Input", verbose);
}

std::string FrameLib_Chain::outputInfo(unsigned long idx, bool verbose)
{
    if (idx)
        return formatInfo("Remaining Count - the number of scheduled frames remaining", "Remaining Count", verbose);
    else
        return formatInfo("Trigger Output - outputs empty frames", "Trigger Output", verbose);
}

// Parameter Info

FrameLib_Chain::ParameterInfo FrameLib_Chain::sParamInfo;

FrameLib_Chain::ParameterInfo::ParameterInfo()
{
    add("Sets the time units used at the input.");
    add("Sets the way in which input values are interpreted: "
        "absolute - times are interpreted as absolute values since the start of time. "
        "relative - times are interpreted as relative to the current reference time. "
        "interval - times are interpreted as intervals (deltas) relative to the current reference time. "
        "Note that the reference time depends on the mode parameter.");
    add("Sets the mode of operation: "
        "replace - cancel previously scheduled frames before scheduling those in the input frame. "
        "add - add new frames to those already scheduled (even if before those already scheduled). "
        "append - schedule new frames only after those currently scheduled have completed. "
        "Note that in replace and add modes the reference time is the time of the incoming frame. "
        "In append mode the reference time is that of the last currently scheduled frame. "
        "In append mode absolute times prior to the end of those already scheduled are ignored.");
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

// Helper to remove duplicate values

unsigned long removeDuplicates(FrameLib_TimeFormat *times, unsigned long size, unsigned long start)
{
    if (size - start < 2)
        return size;
    
    FrameLib_TimeFormat *first = times + start;
    FrameLib_TimeFormat *last = times + size;
    FrameLib_TimeFormat *result = first;
    
    while (++first != last)
    {
        if (*result == *first)
            size--;
        else if (++result != first)
            *result = *first;
    }
    
    return size;
}

FrameLib_Chain::SchedulerInfo FrameLib_Chain::schedule(bool newFrame, bool noAdvance)
{
    FrameLib_TimeFormat now = getCurrentTime();
    
    // Check for an object that has no connections (which will not schedule)
    
    if (!isConnected(0))
        return SchedulerInfo(FrameLib_TimeFormat::largest() - getValidTime(), false, true);
    
    // Deal with a new input frame
    
    if (getInputFrameTime(0) == now)
    {
        Modes mode = mParameters.getEnum<Modes>(kMode);
        TimeModes timeMode = mParameters.getEnum<TimeModes>(kTimeMode);
        Units units = mParameters.getEnum<Units>(kUnits);

        unsigned long sizeIn;

        const double* input = getInput(0, &sizeIn);

        unsigned long remain = mode != kReplace ? mSize - mPosition : 0;
        unsigned long size = remain;
        unsigned long allocSize = mode == kReplace ? sizeIn : remain + sizeIn;
        
        FrameLib_TimeFormat ref = (mode == kAppend && remain) ?  mTimes[mSize - 1]: now;
        
        auto times = allocAutoArray<FrameLib_TimeFormat>(allocSize);
        
        // Deal with a failed allocation
        
        if (!times)
        {
            remain = 0;
            size = 0;
            sizeIn = 0;
        }
        
        // Copy pending old items
        
        std::copy_n(mTimes.get() + mPosition, remain, times.get());
        
        // Write new items
        
        switch (timeMode)
        {
            case kAbsolute:
            {
                for (unsigned long i = 0; i < sizeIn; i++)
                {
                    auto t = convertTime(input[i], units) + FrameLib_TimeFormat(1);
                    
                    if (input[i] >= 0.0 && t >= ref)
                        times[size++] = t;
                }
                break;
            }
                
            case kRelative:
            {
                for (unsigned long i = 0; i < sizeIn; i++)
                {
                    auto t = convertTime(input[i], units) + ref;
                    
                    if (input[i] >= 0.0)
                        times[size++] = t;
                }
                break;
            }
                
            case kInterval:
            {
                FrameLib_TimeFormat accum = ref;
                
                for (unsigned long i = 0; i < sizeIn; i++)
                {
                    auto t = convertTime(input[i], units);
                
                    // N.B. - This text eliminates duplicates for this mode when not adding
                    
                    if (input[i] >= 0.0 && (!size || t.greaterThanZero()))
                        times[size++] = accum += t;
                }
                break;
            }
        }

        // Sort only what is needed and remove duplicates
        
        if (mode == kAdd || timeMode != kInterval)
        {
            unsigned long offset = mode == kAppend ? remain : 0;
            sortAscending(times.get() + offset, size - offset);
            size = removeDuplicates(times.get(), size, offset ? offset - 1 : 0);
        }
        
        // Check for a trigger now
        
        if (size && times[0] == now)
            newFrame = true;
        
        mTimes = std::move(times);
        mSize = size;
        mPosition = 0;
    }
    
    // Get the next time for a trigger frame
    
    while (mPosition < mSize && mTimes[mPosition] <= now)
        mPosition++;
    
    FrameLib_TimeFormat nextTriggerTime = mPosition < mSize ? mTimes[mPosition] :FrameLib_TimeFormat();
    FrameLib_TimeFormat nextInputTime = getInputValidTime(0);
    
    // Output a count for remaining items on new frames

    if (newFrame)
    {
        unsigned long sizeOut;
        
        requestOutputSize(1, 1);
        
        if (allocateOutputs())
            getOutput(1, &sizeOut)[0] = static_cast<double>(mSize - mPosition);
    }
    
    // Schedule
    
    if (nextTriggerTime <= nextInputTime && nextTriggerTime >= now)
        return SchedulerInfo(nextTriggerTime - now, newFrame, true);
    
    return SchedulerInfo(nextInputTime - now, newFrame, false);
}
