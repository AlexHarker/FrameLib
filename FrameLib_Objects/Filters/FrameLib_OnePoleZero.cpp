
#include "FrameLib_OnePoleZero.h"

// Filter Class

// Set Parameters

void FrameLib_OnePoleZero::OnePoleZero::setParams(double freq, double samplingRate)
{
    double fc = M_PI * freq / samplingRate;
    
    a0 = (2.0 * sin(fc)) / (cos(fc) + sin(fc));
    a1 = 1.0 - (a0 * 2.0);
}

// Filter Calculation

inline double FrameLib_OnePoleZero::OnePoleZero::calculateFilter(double x)
{
    double w = x * a0;
    double y = r1 + w;
    
    r1 = w + (y * a1);
    
    return y;
}

// Main Class

// Constructor

FrameLib_OnePoleZero::FrameLib_OnePoleZero(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner) : FrameLib_Processor(context, 2, 1)
{
    mParameters.addDouble(kFreq, "freq", 0.0, 0);
    mParameters.setMin(0.0);
    
    mParameters.addEnum(kMode, "mode", 1);
    mParameters.addEnumItem(kLPF, "lpf");
    mParameters.addEnumItem(kHPF, "hpf");
    
    mParameters.set(serialisedParameters);
    
    inputMode(1, true, false, false, kFrameTagged);
}

// Update and Process

void FrameLib_OnePoleZero::update()
{
    FrameLib_Parameters::Serial *serialised = getInput(1);
    
    if (serialised)
        mParameters.set(serialised);
}

void FrameLib_OnePoleZero::process()
{
    OnePoleZero filter;
    Modes mode = (Modes) mParameters.getValue(kMode);
    
    bool staticParams = true;
    
    double freq = mParameters.getValue(kFreq);
    
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