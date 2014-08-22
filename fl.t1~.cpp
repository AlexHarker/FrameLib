
#include "FrameLib_DSP.h"

class FrameLib_T1 : public FrameLib_Processor
{
    enum AttributeList {kArg0, kInputs};

public:
    
    FrameLib_T1(DSPQueue *queue, FrameLib_Attributes::Serial *serialisedAttributes) : FrameLib_Processor(queue)
    {
        mAttributes.addDouble(kArg0, "0", 1);
        mAttributes.addDouble(kInputs, "inputs", 1);
        
        mAttributes.set(serialisedAttributes);
        
        if (!mAttributes.changed(kInputs))
            mAttributes.set(kInputs, mAttributes.getValue(kArg0));
        
        setIO(mAttributes.getValue(kInputs), 2);
    }
        
protected:
    
    void process()
    {
        unsigned long numOuts = getNumOuts();
        
        for (unsigned int i = 0; i < numOuts; i++)
        {
            requestOutputSize(i, (128 + (rand() % 512)));
        }
        
        allocateOutput();
    }
};

#define OBJECT_CLASS FrameLib_Expand <FrameLib_T1>
#define OBJECT_NAME "fl.t1~"

#include "Framelib_Max.h"
