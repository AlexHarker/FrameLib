
#ifndef FRAMELIB_ONEPOLE_H
#define FRAMELIB_ONEPOLE_H

#include "FrameLib_DSP.h"
#include "FrameLib_Filter_Constants.h"

// FIX - All filters to templates
// FIX - time varying params

class FrameLib_OnePole : public FrameLib_Processor
{
    class OnePole
    {
        
    public:
        
        OnePole() : f0(0.0), y1(0.0)
        {
        }
        
        void reset()
        {
            y1 = 0.0;
        }
        
        double HPF(double x)
        {
            return x - calculateFilter(x);
        }
        
        double LPF(double x)
        {
            return calculateFilter(x);
        }
        
        void setParams(double freq, double samplingRate)
        {
            f0 = sin((freq * FILTER_TWO_PI) / samplingRate);
        }
        
    private:
        
        inline double calculateFilter(double x)
        {
            double y = y1 + f0 * (x - y1);
            
            y1 = y;
            
            return y;
        }
        
        double f0, y1;
    };
    
    enum AttributeList {kFreq, kMode};
    
    enum Modes {kLPF, kHPF};

public:
	
    FrameLib_OnePole(DSPQueue *queue, FrameLib_Attributes::Serial *serialisedAttributes) : FrameLib_Processor(queue, 1, 1, 0, 0)
    {
        mAttributes.addDouble(kFreq, "freq", 0.0, 0);
        mAttributes.setMin(0.0);
        
        mAttributes.addEnum(kMode, "mode", 1);
        mAttributes.addEnumItem(kLPF, "lpf");
        mAttributes.addEnumItem(kHPF, "hpf");
        
        mAttributes.set(serialisedAttributes);
    }
    
protected:
    
    void process()
	{
        OnePole filter;
        Modes mode = (Modes) mAttributes.getValue(kMode);
        
        bool staticParams = TRUE;
        
        double freq = mAttributes.getValue(kFreq);
        
        // Get Input
        
        unsigned long sizeIn, sizeOut;
        double *input = getInput(0, &sizeIn);

        requestOutputSize(0, sizeIn);
        allocateOutputs();
        
        double *output = getOutput(0, &sizeOut);
        
        filter.setParams(freq, mSamplingRate);
        
        if (staticParams)
        {
            switch (mode)
            {
                case kLPF:
                    for (unsigned long i = 0; i < sizeOut; i++)
                        output[i] = filter.LPF(input[i]);
                    break;
                    
                case kHPF:
                    for (unsigned long i = 0; i < sizeOut; i++)
                        output[i] = filter.HPF(input[i]);
                    break;
            }
        }
    }
};

#endif