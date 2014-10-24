
#ifndef FRAMELIB_SALLENANDKEY_H
#define FRAMELIB_SALLENANDKEY_H

#include "FrameLib_DSP.h"
#include "FrameLib_Filter_Constants.h"

// FIX - All filters to templates
// FIX - time varying params

class FrameLib_SallenAndKey : public FrameLib_Processor
{
    class SallenAndKey
    {
        
    public:
        
        SallenAndKey() : k(0.0), a0(0.0), a1(0.0), a2(0.0), a3(0.0), a4(0.0), a5(0.0), v1(0.0), v2(0.0), ic1eq(0.0), ic2eq(0.0)
        {
        }
        
        void reset()
        {
            ic1eq = ic2eq = 0.0;
        }
        
        double HPF(double x)
        {
            calculateFilter(x);
            return x - v2;
        }
        
        double BPF(double x)
        {
            calculateFilter(x);
            return v1 - v2;
        }
        
        double LPF(double x)
        {
            calculateFilter(x);
            return v2;
        }
        
        void setParams(double freq, double resonance, double samplingRate)
        {
            double g = tan(M_PI * (freq / samplingRate));
            double gp1 = 1.0 + g;
            
            k = 2.0 * resonance;
            a0 = 1.0 / ((gp1 * gp1) - (g * k));
            a1 = k * a0;
            a2 = gp1 * a0;
            a3 = g * a2;
            a4 = 1.0 / gp1;
            a5 = g * a4;
        }
        
    private:
        
        inline void calculateFilter(double x)
        {
            v1 = (a1 * ic2eq) + (a2 * ic1eq) + (a3 * x);
            v2 = (a4 * ic2eq) + (a5 * v1);
            
            ic1eq = (2 * (v1 - (k * v2))) - ic1eq;
            ic2eq = (2 * v2) - ic2eq;
        }
        
        double k, a0, a1, a2, a3, a4, a5, v1, v2, ic1eq, ic2eq;
    };
    
    enum AttributeList {kFreq, kReson, kMode};
    
    enum Modes {kLPF, kBPF, kHPF};

public:
	
    FrameLib_SallenAndKey(DSPQueue *queue, FrameLib_Attributes::Serial *serialisedAttributes) : FrameLib_Processor(queue, 1, 1, 0, 0)
    {
        mAttributes.addDouble(kFreq, "freq", 0.0, 0);
        mAttributes.setMin(0.0);
        
        mAttributes.addDouble(kReson, "reson", 0.0, 1);
        mAttributes.setClip(0.0, 1.0);
        
        mAttributes.addEnum(kMode, "mode", 2);
        mAttributes.addEnumItem(kLPF, "lpf");
        mAttributes.addEnumItem(kBPF, "bpf");
        mAttributes.addEnumItem(kHPF, "hpf");
        
        mAttributes.set(serialisedAttributes);
    }
    
protected:
    
    void process ()
	{
        SallenAndKey filter;
        Modes mode = (Modes) mAttributes.getValue(kMode);
        
        bool staticParams = TRUE;
        
        double freq = mAttributes.getValue(kFreq);
        double reson = mAttributes.getValue(kReson);
        
        // Get Input
        
        unsigned long sizeIn, sizeOut;
        double *input = getInput(0, &sizeIn);

        requestOutputSize(0, sizeIn);
        allocateOutputs();
        
        double *output = getOutput(0, &sizeOut);
        
        filter.setParams(freq, reson, mSamplingRate);
        
        if (staticParams)
        {
            switch (mode)
            {
                case kLPF:
                    for (unsigned long i = 0; i < sizeOut; i++)
                        output[i] = filter.LPF(input[i]);
                    break;
                    
                case kBPF:
                    for (unsigned long i = 0; i < sizeOut; i++)
                        output[i] = filter.BPF(input[i]);
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
