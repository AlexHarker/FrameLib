
#include "FrameLib_DSP.h"

class FrameLib_Interval : public FrameLib_Scheduler
{
    
public:
    
    FrameLib_Interval(DSPQueue *queue, FrameLib_Memory *allocator, double interval) : FrameLib_Scheduler(queue, allocator, 0, 1)
    {
        mInterval = interval;
    }
    
    FrameLib_Interval(FrameLib_Interval &object) : FrameLib_Scheduler(object)
    {
        mInterval = object.mInterval;
    }
    
    FrameLib_Block *copy()
    {
        return new FrameLib_Interval(*this);
    }
    
protected:
    
    SchedulerInfo schedule(bool newFrame, bool noOutput)
    {
        return SchedulerInfo(mInterval, TRUE, TRUE);
    }
    
private:
    
    FrameLib_TimeFormat mInterval;
};

#define OBJECT_CREATE new FrameLib_Expand(getConnectionQueue(), new FrameLib_Interval(getDSPQueue(), getAllocator(), schedspeed))
#define OBJECT_NAME "fl.interval~"

#include "Framelib_Max.h"
