
#include "FrameLib_DSP.h"

class FrameLib_T2 : public FrameLib_Processor
{
    enum AttributeList {kOutputs};
        
public:
    
    FrameLib_T2(DSPQueue *queue, FrameLib_Attributes::Serial *serialisedAttributes, void *owner) : FrameLib_Processor(queue)
    {
        mAttributes.addDouble(kOutputs, "outputs", 1, 0);
        
        mAttributes.set(serialisedAttributes);
          
        setIO(2, mAttributes.getValue(kOutputs));
    }
      
protected:
    
    void process()
    {
        for (unsigned int i = 0; i < getNumOuts(); i++)
        {
            requestOutputSize(i, (i + 1) * 128);
        }
        
        allocateOutputs();
    }
};

#define OBJECT_CLASS FrameLib_Expand <FrameLib_T2>
#define OBJECT_NAME "fl.t2~"

#include "Framelib_Max.h"
