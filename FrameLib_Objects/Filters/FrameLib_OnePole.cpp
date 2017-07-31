
#include "FrameLib_OnePole.h"

// Filter Class

// Filter Calculation

double FrameLib_OnePole::OnePole::calculateFilter(double x)
{
    double y = y1 + f0 * (x - y1);
    
    y1 = y;
    
    return y;
}

// Main Class

// Constructor

FrameLib_OnePole::FrameLib_OnePole(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner) : FrameLib_Processor(context, 2, 1)
{
    mParameters.addDouble(kFreq, "freq", 0.0, 0);
    mParameters.setMin(0.0);
    
    mParameters.addEnum(kMode, "mode", 1);
    mParameters.addEnumItem(kLPF, "lpf");
    mParameters.addEnumItem(kHPF, "hpf");
    
    mParameters.set(serialisedParameters);
    
    setParameterInput(1);
}

// Process

void FrameLib_OnePole::process()
{
    OnePole filter;
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
