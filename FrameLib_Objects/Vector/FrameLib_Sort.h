
#ifndef FRAMELIB_SORT_H
#define FRAMELIB_SORT_H

#include "FrameLib_DSP.h"
#include "FrameLib_Sort_Functions.h"

class FrameLib_Sort : public FrameLib_Processor
{
    enum ParameterList {kOrder};
    enum Orders {kUp, kDown};

public:
    
    FrameLib_Sort (FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner) : FrameLib_Processor(context, 1, 1)
    {
        mParameters.addEnum(kOrder, "order", 0);
        mParameters.addEnumItem(kUp, "up");
        mParameters.addEnumItem(kDown, "down");
        
        mParameters.set(serialisedParameters);
        
        mOrder = (Orders) mParameters.getInt(kOrder);
    }
    
protected:
    
    void process()
    {
        unsigned long size;
        double *input = getInput(0, &size);
        
        requestOutputSize(0, size);
        allocateOutputs();
        
        double *output = getOutput(0, &size);
        
        switch (mOrder)
        {
            case kUp:
                sortAscending(output, input, size);
                break;
                
            case kDown:
                sortDescending(output, input, size);
                break;
        }
    }
    
private:
    
    Orders mOrder;
};

#endif
