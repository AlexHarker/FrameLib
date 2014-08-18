
#include "FrameLib_DSP.h"

class FrameLib_T2 : public FrameLib_Processor
{
    
public:
    
    FrameLib_T2(DSPQueue *queue, FrameLib_Memory *allocator, unsigned long nOuts) : FrameLib_Processor(queue, allocator, 2, nOuts)
    {
    }
    
    FrameLib_T2(FrameLib_T2 &object) : FrameLib_Processor(object)
    {
    }
    
    FrameLib_Block *copy()
    {
        return new FrameLib_T2(*this);
    }
    
protected:
    
    void process()
    {
        for (unsigned int i = 0; i < getNumOuts(); i++)
        {
            requestOutputSize(i, (i + 1) * 128);
        }
        
        allocateOutput();
    }
};

#define OBJECT_CREATE new FrameLib_Expand(getConnectionQueue(), new FrameLib_T2(getDSPQueue(), getAllocator(), numIO))
#define OBJECT_NAME "fl.t2~"

#include "Framelib_Max.h"
