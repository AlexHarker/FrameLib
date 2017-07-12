
#include "FrameLib_DSP.h"

class FrameLib_T2 : public FrameLib_Processor
{
    enum ParameterList {kOutputs};
        
public:
    
    FrameLib_T2(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner) : FrameLib_Processor(context)
    {
        mParameters.addDouble(kOutputs, "outputs", 1, 0);
        
        mParameters.set(serialisedParameters);
          
        setIO(2, mParameters.getValue(kOutputs));
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
#include "FrameLib_MaxClass.h"

extern "C" int C74_EXPORT main(void)
{
    FrameLib_MaxClass<FrameLib_Expand <FrameLib_T2> >::makeClass(CLASS_BOX, "fl.t2~");
}
