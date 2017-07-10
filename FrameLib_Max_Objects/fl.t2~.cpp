
#include "FrameLib_DSP.h"

class FrameLib_T2 : public FrameLib_Processor
{
    enum ParameterList {kOutputs};
        
public:
    
    FrameLib_T2(FrameLib_Context context, FrameLib_Parameters::SerialBase *serialisedParameters, void *owner) : FrameLib_Processor(context)
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

#define OBJECT_CLASS FrameLib_Expand <FrameLib_T2>
#define OBJECT_NAME "fl.t2~"

#include "Framelib_Max.h"
