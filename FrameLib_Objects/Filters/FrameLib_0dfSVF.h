
#ifndef FRAMELIB_0DFSVF_H
#define FRAMELIB_0DFSVF_H

#include "FrameLib_DSP.h"
#include "FrameLib_Filter_Constants.h"

// FIX - All filters to templates
// FIX - time varying params

class FrameLib_0dfSVF : public FrameLib_Processor
{
    class ZeroDelayFeedbackSVF
    {
        
        // Thanks to Oli Larkin for this code
        
    public:
        
        ZeroDelayFeedbackSVF() : lp(0.0), bp(0.0), hp(0.0), r(0.0), g(0.0), s1(0.0), s2(0.0)
        {
        }
        
        void reset()
        {
            s1 = s2 = 0.0;
        }
        
        double HPF(double x)
        {
            calculateFilter(x);
            return hp;
        }
        
        double BPF(double x)
        {
            calculateFilter(x);
            return bp;
        }
        
        double LPF(double x)
        {
            calculateFilter(x);
            return lp;
        }
        
        void setParams(double freq, double resonance, double samplingRate)
        {
            r = 1.0 - resonance;
            r = r < 0.005 ? 0.005 : r;
            r = r > 1.0 ? 1.0 : r;
            double srConst = 0.5 / samplingRate;
            g = ((2.0 * samplingRate) * tan((freq * FILTER_TWO_PI) * srConst) * srConst);
        }
        
    private:
        
        inline void calculateFilter(double x)
        {
            // Compute highpass then bandpass  by applying 1st integrator to highpass output and update state
            
            hp = (x - (2.0 * r * s1) - (g * s1) - s2) / (1.0 + (2.0 * r * g) + (g * g));
            bp = g * hp + s1;
            s1 = g * hp + bp;
            
            // Compute lowpass by applying 2nd integrator to bandpass output and update state
            
            lp = g * bp + s2;
            s2 = g * bp + lp;
        }
        
        double lp, bp, hp, r, g, s1, s2;
    };
    
    enum AttributeList {kFreq, kReson, kMode};
    
    enum Modes {kLPF, kBPF, kHPF};

public:
	
    FrameLib_0dfSVF(FrameLib_Context context, FrameLib_Attributes::Serial *serialisedAttributes, void *owner) : FrameLib_Processor(context, 2, 1)
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
        
        inputMode(1, TRUE, FALSE, FALSE);
    }
    
protected:
    
    void update()
    {
        FrameLib_Attributes::Serial *serialised = getInput(1);
        
        if (serialised)
            mAttributes.set(serialised);
    }
    
    void process ()
	{
        ZeroDelayFeedbackSVF filter;
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
