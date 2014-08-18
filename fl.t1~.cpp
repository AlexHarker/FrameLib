
#include "FrameLib_DSP.h"

class FrameLib_T1 : public FrameLib_Processor
{
    
public:
    
    FrameLib_T1(DSPQueue *queue, FrameLib_Memory *allocator, unsigned long nIns) : FrameLib_Processor(queue, allocator, nIns, 2)
    {
    }
    
    FrameLib_T1(FrameLib_T1 &object) : FrameLib_Processor(object)
    {
    }
    
    FrameLib_Block *copy()
    {
        return new FrameLib_T1(*this);
    }
    
protected:
    
    void process()
    {
        for (unsigned int i = 0; i < getNumOuts(); i++)
        {
            requestOutputSize(i, (128 + (rand() % 128)));
        }
        
        allocateOutput();
    }
};

#define OBJECT_CREATE new FrameLib_Expand(getConnectionQueue(), new FrameLib_T1(getDSPQueue(), getAllocator(), numIO))
#define OBJECT_NAME "fl.t1~"

#include "Framelib_Max.h"
