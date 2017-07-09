
#ifndef FRAMELIB_INTERVAL_H
#define FRAMELIB_INTERVAL_H

#include "FrameLib_DSP.h"


class FrameLib_Interval : public FrameLib_Scheduler
{
    enum ParameterList {kInterval, kUnits};
    enum Units {kSamples, kMS, kSeconds};

public:
    
    FrameLib_Interval(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner) : FrameLib_Scheduler(context, 1, 1)
    {
        // FIX - safety on minimum time?
        
        mParameters.addDouble(kInterval, "interval", 16, 0);
        mParameters.setMin(0);
        
        mParameters.addEnum(kUnits, "units");
        mParameters.addEnumItem(kSamples, "samples");
        mParameters.addEnumItem(kMS, "ms");
        mParameters.addEnumItem(kSeconds, "seconds");
        
        mParameters.set(serialisedParameters);
    }
    
protected:
    
    SchedulerInfo schedule(bool newFrame, bool noOutput)
    {
        FrameLib_Parameters::Serial *serialised = getInput(0);
        
        if (serialised)
            mParameters.set(serialised);
        
        FrameLib_TimeFormat interval = mParameters.getValue(kInterval);
        
        switch ((Units) (mParameters.getValue(kUnits)))
        {
            case kMS:
                interval *= mSamplingRate / 1000.0;
                break;
                
            case kSeconds:
                interval *= mSamplingRate;
                break;
                
            case kSamples:
                break;
        }

        return SchedulerInfo(interval, true, true);
    }
};

#endif
