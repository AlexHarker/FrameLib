
#include "FrameLib_DSP.h"

class FrameLib_T2 : public FrameLib_Processor
{
    enum AttributeList {kArg0, kOutputs};
        
public:
    
    FrameLib_T2(DSPQueue *queue, FrameLib_Memory *allocator, FrameLib_Attributes::Serial *serialisedAttributes) : FrameLib_Processor(queue, allocator)
    {
        mAttributes.addDouble(kArg0, "0", 1);
        mAttributes.addDouble(kOutputs, "outputs", 1);
        
        mAttributes.set(serialisedAttributes);
        
        if (!mAttributes.changed(kOutputs))
            mAttributes.set(kOutputs, mAttributes.getValue(kArg0));
        
        setIO(2, mAttributes.getValue(kOutputs));
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

#define OBJECT_CREATE new FrameLib_Expand<FrameLib_T2>(getConnectionQueue(), getDSPQueue(), getAllocator(), serialisedAttributes)
#define OBJECT_NAME "fl.t2~"

#include "Framelib_Max.h"
