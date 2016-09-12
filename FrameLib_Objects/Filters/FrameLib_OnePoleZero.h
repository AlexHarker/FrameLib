
#ifndef FRAMELIB_ONEPOLEZERO_H
#define FRAMELIB_ONEPOLEZERO_H

#include "FrameLib_DSP.h"
#include "FrameLib_Filter_Constants.h"

// FIX - All filters to templates
// FIX - time varying params

class FrameLib_OnePoleZero : public FrameLib_Processor
{
    class OnePoleZero
    {
        
    public:
        
        OnePoleZero() : a0(0.0), a1(0.0), r1(0.0)
        {
        }
        
        void reset()
        {
            r1 = 0.0;
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
            double fc = M_PI * freq / samplingRate;
            
            a0 = (2.0 * sin(fc)) / (cos(fc) + sin(fc));
            a1 = 1.0 - (a0 * 2.0);
        }
        
    private:
        
        inline double calculateFilter(double x)
        {
            double w = x * a0;
            double y = r1 + w;
            
            r1 = w + (y * a1);
            
            return y;
        }
        
        double a0, a1, r1;
    };
    
    enum AttributeList {kFreq, kMode};
    
    enum Modes {kLPF, kHPF};

public:
	
    FrameLib_OnePoleZero(DSPQueue *queue, FrameLib_Attributes::Serial *serialisedAttributes, void *owner) : FrameLib_Processor(queue, 1, 1)
    {
        mAttributes.addDouble(kFreq, "freq", 0.0, 0);
        mAttributes.setMin(0.0);
        
        mAttributes.addEnum(kMode, "mode", 1);
        mAttributes.addEnumItem(kLPF, "lpf");
        mAttributes.addEnumItem(kHPF, "hpf");
        
        mAttributes.set(serialisedAttributes);
    }
    
protected:
    
    void process ()
	{
        OnePoleZero filter;
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
