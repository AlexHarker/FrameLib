
#include "FrameLib_Resonant.h"

// Filter Class

// Set Parameters

void FrameLib_Resonant::Resonant::setParams(double freq, double reson, double samplingRate)
{
    double frad = cos(freq * M_PI * 2.0 / samplingRate);
    double res = 0.882497 * exp(reson * 0.125);
    
    scl = (frad * res) * -2.0;
    r2 = res * res;
}

// Filter Calculation

inline double FrameLib_Resonant::Resonant::calculateFilter(double x)
{
    x = x * ((scl + r2) + 1.0);
    double y = x - ((scl * y1) + (r2 * y2));
    
    y2 = y1;
    y1 = y;
    
    return y;
}

// Main Class

// Constructor

FrameLib_Resonant::FrameLib_Resonant(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner) : FrameLib_Processor(context, 2, 1)
{
    mParameters.addDouble(kFreq, "freq", 0.0, 0);
    mParameters.setMin(0.0);
    
    mParameters.addDouble(kReson, "reson", 0.0, 1);
    mParameters.setClip(0.0, 1.0);
    
    mParameters.addEnum(kMode, "mode", 2);
    mParameters.addEnumItem(kLPF, "lpf");
    mParameters.addEnumItem(kHPF, "hpf");
    
    mParameters.set(serialisedParameters);
    
    inputMode(1, true, false, false, kFrameTagged);
}

// Update and Process

void FrameLib_Resonant::update()
{
    FrameLib_Parameters::Serial *serialised = getInput(1);
    
    if (serialised)
        mParameters.set(serialised);
}

void FrameLib_Resonant::process()
{
    Resonant filter;
    Modes mode = (Modes) mParameters.getValue(kMode);
    
    bool staticParams = true;
    
    double freq = mParameters.getValue(kFreq);
    double reson = mParameters.getValue(kReson);
    
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
                
            case kHPF:
                for (unsigned long i = 0; i < sizeOut; i++)
                    output[i] = filter.HPF(input[i]);
                break;
        }
    }
}
