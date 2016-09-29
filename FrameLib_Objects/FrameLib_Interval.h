
#ifndef FRAMELIB_INTERVAL_H
#define FRAMELIB_INTERVAL_H

#include "FrameLib_DSP.h"


class FrameLib_Interval : public FrameLib_Scheduler
{
    enum AttributeList {kInterval, kUnits};
    enum Units {kSamples, kMS, kSeconds};

public:
    
    FrameLib_Interval(FrameLib_Context context, FrameLib_Attributes::Serial *serialisedAttributes, void *owner) : FrameLib_Scheduler(context, 1, 1)
    {
        // FIX - safety on minimum time?
        
        mAttributes.addDouble(kInterval, "interval", 16, 0);
        mAttributes.setMin(0);
        
        mAttributes.addEnum(kUnits, "units");
        mAttributes.addEnumItem(kSamples, "samples");
        mAttributes.addEnumItem(kMS, "ms");
        mAttributes.addEnumItem(kSeconds, "seconds");
        
        mAttributes.set(serialisedAttributes);
    }
    
protected:
    
    SchedulerInfo schedule(bool newFrame, bool noOutput)
    {
        FrameLib_Attributes::Serial *serialised = getInput(0);
        
        if (serialised)
            mAttributes.set(serialised);
        
        FrameLib_TimeFormat interval = mAttributes.getValue(kInterval);
        
        switch ((Units) (mAttributes.getValue(kUnits)))
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
